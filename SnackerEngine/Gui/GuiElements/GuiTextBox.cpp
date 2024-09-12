#include "Gui/GuiElements/GuiTextBox.h"
#include "Gui/Text/Text.h"
#include "Gui/Text/TextMaterial.h"
#include "Math/Utility.h"
#include "Core/Engine.h"
#include "Gui/GuiManager.h"
#include "Graphics/Renderer.h"

namespace SnackerEngine
{
	//--------------------------------------------------------------------------------------------------
	Color4f GuiTextBox::defaultTextColor = Color4f(1.0f, 1.0f);
	Color4f GuiTextBox::defaultBackgroundColor = Color4f(0.0f, 0.0f);
	unsigned GuiTextBox::defaultRecomputeTries = 10;
	GuiTextBox::SizeHintModes GuiTextBox::defaultSizeHintModes = { GuiTextBox::SizeHintMode::SET_TO_TEXT_SIZE, GuiTextBox::SizeHintMode::ARBITRARY, GuiTextBox::SizeHintMode::SET_TO_TEXT_SIZE };
	//--------------------------------------------------------------------------------------------------
	/// Helper functions for parsing JSON
	template<> bool isOfType(const nlohmann::json& json, JsonTag<GuiTextBox::TextScaleMode> tag)
	{
		if (!json.is_string()) {
			warningLogger << LOGGER::BEGIN << "GuiDynamicTextBox option \"textScaleMode\" has to be given as string." << LOGGER::ENDL;
			return false;
		}
		if (json == "DONT_SCALE" ||
			json == "SCALE_UP" ||
			json == "SCALE_DOWN" ||
			json == "SCALE_UP_DOWN" ||
			json == "RECOMPUTE_DOWN" ||
			json == "RECOMPUTE_UP_DOWN") return true;
		warningLogger << LOGGER::BEGIN << "\"" << json << "\" is not a valid textScaleMode." << LOGGER::ENDL;
		return false;
	}
	template<> GuiTextBox::TextScaleMode parseJSON(const nlohmann::json& json, JsonTag<GuiTextBox::TextScaleMode> tag)
	{
		if (json == "DONT_SCALE") return GuiTextBox::TextScaleMode::DONT_SCALE;
		if (json == "SCALE_UP") return GuiTextBox::TextScaleMode::SCALE_UP;
		if (json == "SCALE_DOWN") return GuiTextBox::TextScaleMode::SCALE_DOWN;
		if (json == "SCALE_UP_DOWN") return GuiTextBox::TextScaleMode::SCALE_UP_DOWN;
		if (json == "RECOMPUTE_DOWN") return GuiTextBox::TextScaleMode::RECOMPUTE_DOWN;
		if (json == "RECOMPUTE_UP_DOWN") return GuiTextBox::TextScaleMode::RECOMPUTE_UP_DOWN;
		return GuiTextBox::TextScaleMode::DONT_SCALE;
	}
	template<> bool isOfType(const nlohmann::json& json, JsonTag<GuiTextBox::SizeHintMode> tag)
	{
		if (!json.is_string()) {
			warningLogger << LOGGER::BEGIN << "GuiDynamicTextBox option \"sizeHintMode\" has to be given as string." << LOGGER::ENDL;
			return false;
		}
		if (json == "ARBITRARY" ||
			json == "SET_TO_TEXT_SIZE" ||
			json == "SET_TO_TEXT_WIDTH" ||
			json == "SET_TO_TEXT_HEIGHT") return true;
		warningLogger << LOGGER::BEGIN << "\"" << json << "\" is not a valid sizeHintMode." << LOGGER::ENDL;
		return false;
	}
	template<> GuiTextBox::SizeHintMode parseJSON(const nlohmann::json& json, JsonTag<GuiTextBox::SizeHintMode> tag)
	{
		if (json == "ARBITRARY") return GuiTextBox::SizeHintMode::ARBITRARY;
		if (json == "SET_TO_TEXT_SIZE") return GuiTextBox::SizeHintMode::SET_TO_TEXT_SIZE;
		if (json == "SET_TO_TEXT_WIDTH") return GuiTextBox::SizeHintMode::SET_TO_TEXT_WIDTH;
		if (json == "SET_TO_TEXT_HEIGHT") return GuiTextBox::SizeHintMode::SET_TO_TEXT_HEIGHT;
		return GuiTextBox::SizeHintMode::ARBITRARY;
	}
	template<> bool isOfType(const nlohmann::json& json, JsonTag<DynamicText::Alignment> tag)
	{
		if (!json.is_string()) {
			warningLogger << LOGGER::BEGIN << "GuiDynamicTextBox option \"alignment\" has to be given as string." << LOGGER::ENDL;
			return false;
		}
		if (json == "LEFT" ||
			json == "CENTER" ||
			json == "RIGHT") return true;
		warningLogger << LOGGER::BEGIN << "\"" << json << "\" is not a valid alignment." << LOGGER::ENDL;
		return false;
	}
	template<> DynamicText::Alignment parseJSON(const nlohmann::json& json, JsonTag<DynamicText::Alignment> tag)
	{
		if (json == "LEFT") return DynamicText::Alignment::LEFT;
		if (json == "CENTER") return DynamicText::Alignment::CENTER;
		if (json == "RIGHT") return DynamicText::Alignment::RIGHT;
		return DynamicText::Alignment::LEFT;
	}
	template<> bool isOfType(const nlohmann::json& json, JsonTag<DynamicText::ParseMode> tag)
	{
		if (!json.is_string()) {
			warningLogger << LOGGER::BEGIN << "GuiDynamicTextBox option \"parseMode\" has to be given as string." << LOGGER::ENDL;
			return false;
		}
		if (json == "WORD_BY_WORD" ||
			json == "CHARACTERS" ||
			json == "SINGLE_LINE") return true;
		warningLogger << LOGGER::BEGIN << "\"" << json << "\" is not a valid parseMode." << LOGGER::ENDL;
		return false;
	}
	template<> DynamicText::ParseMode parseJSON(const nlohmann::json& json, JsonTag<DynamicText::ParseMode> tag)
	{
		if (json == "WORD_BY_WORD") return DynamicText::ParseMode::WORD_BY_WORD;
		if (json == "CHARACTERS") return DynamicText::ParseMode::CHARACTERS;
		if (json == "SINGLE_LINE") return DynamicText::ParseMode::SINGLE_LINE;
		return DynamicText::ParseMode::WORD_BY_WORD;
	}
	//--------------------------------------------------------------------------------------------------
	void GuiTextBox::computeModelMatrices()
	{
		if (!dynamicText) return;
		unsigned int DPI = Engine::getDPI().y;
		scaleFactor = 1.0;
		switch (textScaleMode)
		{
		case SnackerEngine::GuiTextBox::TextScaleMode::SCALE_UP:
		{
			double textWidth = pointsToInches((dynamicText->getRight() - dynamicText->getLeft())) * DPI;
			if (textWidth > getWidth() - 2 * border) break;
			double textHeight = pointsToInches((dynamicText->getTop() - dynamicText->getBottom())) * DPI;
			if (textHeight > getHeight() - 2 * border) break;
			scaleFactor = std::min((getWidth() - 2 * border) / textWidth, (getHeight() - 2 * border) / textHeight);
			break;
		}
		case SnackerEngine::GuiTextBox::TextScaleMode::SCALE_DOWN:
		{
			double textWidth = pointsToInches((dynamicText->getRight() - dynamicText->getLeft())) * DPI;
			double textHeight = pointsToInches((dynamicText->getTop() - dynamicText->getBottom())) * DPI;
			if (textWidth < getWidth() - 2 * border && textHeight < getHeight() - 2 * border) break;
			scaleFactor = std::min((getWidth() - 2 * border) / textWidth, (getHeight() - 2 * border) / textHeight);
			break;
		}
		case SnackerEngine::GuiTextBox::TextScaleMode::SCALE_UP_DOWN:
		{
			double textWidth = pointsToInches((dynamicText->getRight() - dynamicText->getLeft())) * DPI;
			double textHeight = pointsToInches((dynamicText->getTop() - dynamicText->getBottom())) * DPI;
			scaleFactor = std::min((getWidth() - 2 * border) / textWidth, (getHeight() - 2 * border) / textHeight);
			break;
		}
		default:
		{
			break;
		}
		}
		Vec2f textPosition = computeTextPosition();
		if (dynamicText) modelMatrixText = Mat4f::TranslateAndScale(Vec3f(textPosition.x, textPosition.y, 0), Vec3f(static_cast<float>(pointsToInches(dynamicText->getFontSize()) * static_cast<double>(DPI) * scaleFactor)));
	}
	//--------------------------------------------------------------------------------------------------
	void GuiTextBox::recomputeText()
	{
		if (!dynamicText) return;
		if (textScaleMode == TextScaleMode::RECOMPUTE_DOWN) {
			dynamicText->setTextWidth((getWidth() - 2 * border) / static_cast<float>(Engine::getDPI().y) / pointsToInches(1.0f), false);
			dynamicText->setFontSize(fontSize, false);
			unsigned int DPI = Engine::getDPI().y;
			double maxFontSize = 0.0;
			double minFontSize = 0.0;
			for (unsigned int i = 0; i < recomputeTries; ++i)
			{
				dynamicText->recompute();
				double textHeight = pointsToInches((dynamicText->getTop() - dynamicText->getBottom())) * DPI;
				// Check text width only in single line parse mode!
				if (textHeight < (getHeight() - 2 * border) && (getParseMode() != StaticText::ParseMode::SINGLE_LINE || pointsToInches((dynamicText->getRight() - dynamicText->getLeft())) * DPI < getWidth() - 2 * border)) {
					minFontSize = dynamicText->getFontSize();
					if (minFontSize == fontSize) break;
					if (maxFontSize == 0) {
						dynamicText->setFontSize(std::min(minFontSize * 2.0, fontSize), false);
					}
					else {
						dynamicText->setFontSize((minFontSize + maxFontSize) / 2.0, false);
					}
				}
				else {
					maxFontSize = dynamicText->getFontSize();
					if (minFontSize == 0) {
						dynamicText->setFontSize(maxFontSize / 2.0, false);
					}
					else {
						dynamicText->setFontSize((minFontSize + maxFontSize) / 2.0, false);
					}
				}
				//infoLogger << "font size:" << text->getFontSize() << ", minFontSize: " << minFontSize << LOGGER::ENDL;
			}
			dynamicText->setFontSize(minFontSize, false);
			dynamicText->recompute();
			computeModelMatrices();
		}
		else if (textScaleMode == TextScaleMode::RECOMPUTE_UP_DOWN) {
			dynamicText->setTextWidth((getWidth() - 2 * border) / static_cast<float>(Engine::getDPI().y) / pointsToInches(1.0f), false);
			dynamicText->setFontSize(fontSize, false);
			unsigned int DPI = Engine::getDPI().y;
			double maxFontSize = 0.0;
			double minFontSize = 0.0;
			//infoLogger << "recomputing text, starting with font size " << text->getFontSize() << LOGGER::ENDL;
			for (unsigned int i = 0; i < recomputeTries; ++i)
			{
				dynamicText->recompute();
				double textHeight = pointsToInches((dynamicText->getTop() - dynamicText->getBottom())) * DPI;
				// Check text width only in single line parse mode!
				if (textHeight < (getHeight() - 2 * border) && (getParseMode() != StaticText::ParseMode::SINGLE_LINE || pointsToInches((dynamicText->getRight() - dynamicText->getLeft())) * DPI < (getHeight() - 2 * border))) {
					minFontSize = dynamicText->getFontSize();
					if (maxFontSize == 0) {
						dynamicText->setFontSize(minFontSize * 2.0, false);
					}
					else {
						dynamicText->setFontSize((minFontSize + maxFontSize) / 2.0, false);
					}
				}
				else {
					maxFontSize = dynamicText->getFontSize();
					if (minFontSize == 0) {
						dynamicText->setFontSize(maxFontSize / 2.0, false);
					}
					else {
						dynamicText->setFontSize((minFontSize + maxFontSize) / 2.0, false);
					}
				}
			}
			dynamicText->setFontSize(minFontSize, false);
			dynamicText->recompute();
			computeModelMatrices();
		}
		else {
			// Just compute the text with the size.x as textWidth:
			dynamicText->setTextWidth((getWidth() - 2 * border) / static_cast<float>(Engine::getDPI().y) / pointsToInches(1.0f), false);
			dynamicText->recompute();
			computeModelMatrices();
		}
		computeHeightHints();
	}
	//--------------------------------------------------------------------------------------------------
	void GuiTextBox::onRegister(std::unique_ptr<DynamicText>&& dynamicText)
	{
		GuiElement::onRegister();
		this->dynamicText = std::move(dynamicText);
		material = constructTextMaterial(font, textColor, getBackgroundColor());
		computeModelMatrices();
		computeHeightHints();
		comouteWidthHints();
	}
	//--------------------------------------------------------------------------------------------------
	void GuiTextBox::recomputeTextOnSizeChange()
	{
		if (doRecomputeOnSizeChange && getParseMode() != StaticText::ParseMode::SINGLE_LINE && lastSizeOnRecomputeText != getSize())
		{
			recomputeText();
			comouteWidthHints();
			lastSizeOnRecomputeText = getSize();
		}
		else
		{
			computeModelMatrices();
		}
	}
	//--------------------------------------------------------------------------------------------------
	Vec2f GuiTextBox::computeTextPosition()
	{
		if (!dynamicText) return Vec2i();
		unsigned int DPI = Engine::getDPI().y;
		// Compute offset due to centering
		Vec2f textOffset = Vec2f(0.0f, 0.0f);
		switch (dynamicText->getAlignment())
		{
		case StaticText::Alignment::LEFT:
		{
			// Align to the top of the text box
			textOffset.y = static_cast<float>(-pointsToInches(dynamicText->getTop()) * static_cast<double>(DPI) * scaleFactor);
			// Is already aligned to the left!
			break;
		}
		case StaticText::Alignment::CENTER:
		{
			// Align to the center of the text box
			textOffset.y = static_cast<float>(-pointsToInches((dynamicText->getTop() + dynamicText->getBottom()) / 2.0) * static_cast<double>(DPI) * scaleFactor - static_cast<double>(getHeight() - 2 * border) / 2.0);
			textOffset.x = static_cast<float>(static_cast<double>(getWidth() - 2 * border) / 2.0 - pointsToInches((dynamicText->getRight() - dynamicText->getLeft()) / 2.0) * DPI * scaleFactor);
			break;
		}
		case StaticText::Alignment::RIGHT:
		{
			// Align to the top of the text box
			textOffset.y = static_cast<float>(-pointsToInches(dynamicText->getTop()) * static_cast<double>(DPI) * scaleFactor);
			// Align to the right of the text box
			textOffset.x = static_cast<float>(static_cast<double>(getWidth() - 2 * border) - pointsToInches((dynamicText->getRight() - dynamicText->getLeft())) * static_cast<double>(DPI) * scaleFactor);
			break;
		}
		default:
			break;
		}
		Vec2f position = Vec2f(static_cast<float>(textOffset.x + border), -static_cast<float>(border) + static_cast<float>(textOffset.y));
		return position;
	}
	//--------------------------------------------------------------------------------------------------
	void GuiTextBox::drawText(const Vec2i& worldPosition)
	{
		GuiManager* const& guiManager = getGuiManager();
		if (!guiManager) return;
		if (dynamicText) {
			Mat4f translationMatrix = Mat4f::Translate(Vec3f(static_cast<float>(worldPosition.x), static_cast<float>(-worldPosition.y), 0.0f));
			material.bind();
			guiManager->setUniformViewAndProjectionMatrices(material.getShader());
			material.getShader().setUniform<Mat4f>("u_model", translationMatrix * modelMatrixText);
			material.getShader().setUniform<float>("u_pxRange", static_cast<float>(dynamicText->getFont().getPixelRange()));
			SnackerEngine::Renderer::draw(dynamicText->getModel(), material);
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiTextBox::drawText(const Vec2i& worldPosition, const Mat4f& scale)
	{
		GuiManager* const& guiManager = getGuiManager();
		if (!guiManager) return;
		if (dynamicText) {
			Mat4f translationMatrix = Mat4f::Translate(Vec3f(static_cast<float>(worldPosition.x), static_cast<float>(-worldPosition.y), 0.0f));
			material.bind();
			guiManager->setUniformViewAndProjectionMatrices(material.getShader());
			material.getShader().setUniform<Mat4f>("u_model", translationMatrix * scale * modelMatrixText);
			material.getShader().setUniform<float>("u_pxRange", static_cast<float>(dynamicText->getFont().getPixelRange()));
			SnackerEngine::Renderer::draw(dynamicText->getModel(), material);
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiTextBox::computeHeightHints()
	{
		if (!dynamicText) return;
		int textHeight = static_cast<int>(std::ceil((dynamicText->getTop() - dynamicText->getBottom()) * pointsToInches(1.0) * static_cast<double>(Engine::getDPI().y)));
		textHeight += 2 * border;
		switch (sizeHintModes.sizeHintModeMinSize)
		{
		case SizeHintMode::ARBITRARY: break;
		case SizeHintMode::SET_TO_TEXT_SIZE: setMinHeight(textHeight); break;
		case SizeHintMode::SET_TO_TEXT_WIDTH: break;
		case SizeHintMode::SET_TO_TEXT_HEIGHT: setMinHeight(textHeight); break;
		default:
			break;
		}
		switch (sizeHintModes.sizeHintModeMaxSize)
		{
		case SizeHintMode::ARBITRARY: break;
		case SizeHintMode::SET_TO_TEXT_SIZE: setMaxHeight(textHeight); break;
		case SizeHintMode::SET_TO_TEXT_WIDTH: break;
		case SizeHintMode::SET_TO_TEXT_HEIGHT: setMaxHeight(textHeight); break;
		default:
			break;
		}
		switch (sizeHintModes.sizeHintModePreferredSize)
		{
		case SizeHintMode::ARBITRARY: break;
		case SizeHintMode::SET_TO_TEXT_SIZE: setPreferredHeight(textHeight); break;
		case SizeHintMode::SET_TO_TEXT_WIDTH: break;
		case SizeHintMode::SET_TO_TEXT_HEIGHT: setPreferredHeight(textHeight); break;
		default:
			break;
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiTextBox::comouteWidthHints()
	{
		if (!dynamicText) return;
		int textWidth{};
		if (parseMode == StaticText::ParseMode::SINGLE_LINE) {
			textWidth = static_cast<int>(std::ceil((dynamicText->getRight() - dynamicText->getLeft()) * pointsToInches(1.0) * static_cast<double>(Engine::getDPI().x)));
		}
		else {
			DynamicText tempText(this->text, this->font, this->fontSize, 0.0f, StaticText::ParseMode::SINGLE_LINE, this->alignment);
			textWidth = static_cast<int>(std::ceil((tempText.getRight() - tempText.getLeft()) * pointsToInches(1.0) * static_cast<double>(Engine::getDPI().x)));
		}
		textWidth += 2 * border;
		switch (sizeHintModes.sizeHintModeMinSize)
		{
		case SizeHintMode::ARBITRARY: break;
		case SizeHintMode::SET_TO_TEXT_SIZE: setMinWidth(textWidth); break;
		case SizeHintMode::SET_TO_TEXT_WIDTH: setMinWidth(textWidth); break;
		case SizeHintMode::SET_TO_TEXT_HEIGHT: break;
		default:
			break;
		}
		switch (sizeHintModes.sizeHintModeMaxSize)
		{
		case SizeHintMode::ARBITRARY: break;
		case SizeHintMode::SET_TO_TEXT_SIZE: setMaxWidth(textWidth); break;
		case SizeHintMode::SET_TO_TEXT_WIDTH: setMaxWidth(textWidth); break;
		case SizeHintMode::SET_TO_TEXT_HEIGHT: break;
		default:
			break;
		}
		switch (sizeHintModes.sizeHintModePreferredSize)
		{
		case SizeHintMode::ARBITRARY: break;
		case SizeHintMode::SET_TO_TEXT_SIZE: setPreferredWidth(textWidth); break;
		case SizeHintMode::SET_TO_TEXT_WIDTH: setPreferredWidth(textWidth); break;
		case SizeHintMode::SET_TO_TEXT_HEIGHT: break;
		default:
			break;
		}
	}
	//--------------------------------------------------------------------------------------------------
	Material GuiTextBox::constructTextMaterial(const Font& font, const Color4f& textColor, const Color4f& backgroundColor)
	{
		return Material(std::move(std::make_unique<SimpleTextMaterialData>(Shader("shaders/gui/basic2DText.shader"),
			font, textColor)));
	}
	//--------------------------------------------------------------------------------------------------
	GuiTextBox::GuiTextBox(const Vec2i& position, const Vec2i& size, const std::string& text, const Font& font, const double& fontSize, const Color4f& backgroundColor)
		: GuiPanel(position, size, ResizeMode::RESIZE_RANGE, backgroundColor), fontSize(fontSize), text(text), font(font) {}
	//--------------------------------------------------------------------------------------------------
	GuiTextBox::GuiTextBox(const std::string& text)
		: GuiPanel({}, {}, ResizeMode::RESIZE_RANGE, defaultBackgroundColor), text(text) {}
	//--------------------------------------------------------------------------------------------------
	GuiTextBox::GuiTextBox(const nlohmann::json& json, const nlohmann::json* data, std::set<std::string>* parameterNames)
		: GuiPanel(json, data, parameterNames)
	{
		parseJsonOrReadFromData(textColor, "textColor", json, data, parameterNames);
		parseJsonOrReadFromData(textScaleMode, "textScaleMode", json, data, parameterNames);
		parseJsonOrReadFromData(sizeHintModes.sizeHintModeMinSize, "sizeHintModeMinSize", json, data, parameterNames);
		parseJsonOrReadFromData(sizeHintModes.sizeHintModeMaxSize, "sizeHintModeMaxSize", json, data, parameterNames);
		parseJsonOrReadFromData(sizeHintModes.sizeHintModePreferredSize, "sizeHintModePreferredSize", json, data, parameterNames);
		if (textScaleMode == TextScaleMode::SCALE_DOWN || textScaleMode == TextScaleMode::RECOMPUTE_DOWN) {
			if (!json.contains("sizeHintModeMinSize")) sizeHintModes.sizeHintModeMinSize = SizeHintMode::ARBITRARY;
			if (!json.contains("sizeHintModePreferredSize")) sizeHintModes.sizeHintModePreferredSize = SizeHintMode::ARBITRARY;
		}
		else if (textScaleMode == TextScaleMode::SCALE_UP) {
			if (!json.contains("sizeHintModePreferredSize")) sizeHintModes.sizeHintModePreferredSize = SizeHintMode::ARBITRARY;
			if (!json.contains("sizeHintModeMaxSize")) sizeHintModes.sizeHintModeMaxSize = SizeHintMode::ARBITRARY;
		}
		else if (textScaleMode == TextScaleMode::SCALE_UP_DOWN || textScaleMode == TextScaleMode::RECOMPUTE_UP_DOWN) {
			if (!json.contains("sizeHintModeMinSize")) sizeHintModes.sizeHintModeMinSize = SizeHintMode::ARBITRARY;
			if (!json.contains("sizeHintModePreferredSize")) sizeHintModes.sizeHintModePreferredSize = SizeHintMode::ARBITRARY;
			if (!json.contains("sizeHintModeMaxSize")) sizeHintModes.sizeHintModeMaxSize = SizeHintMode::ARBITRARY;
		}
		parseJsonOrReadFromData(border, "border", json, data, parameterNames);
		parseJsonOrReadFromData(fontSize, "fontSize", json, data, parameterNames);
		parseJsonOrReadFromData(recomputeTries, "recomputeTries", json, data, parameterNames);
		parseJsonOrReadFromData(doRecomputeOnSizeChange, "doRecomputeOnSizeChange", json, data, parameterNames);
		parseJsonOrReadFromData(text, "text", json, data, parameterNames);
		parseJsonOrReadFromData(parseMode, "parseMode", json, data, parameterNames);
		parseJsonOrReadFromData(alignment, "alignment", json, data, parameterNames);
		parseJsonOrReadFromData(font, "font", json, data, parameterNames);
		if (!json.contains("backgroundColor")) setBackgroundColor(defaultBackgroundColor);
		if (!json.contains("sizeHintModePreferredSize")) {
			if (json.contains("preferredWidth")) sizeHintModes.sizeHintModePreferredSize = SizeHintMode::SET_TO_TEXT_HEIGHT;
			if (json.contains("preferredHeight")) sizeHintModes.sizeHintModePreferredSize = SizeHintMode::SET_TO_TEXT_WIDTH;
			if (json.contains("preferredSize") || (json.contains("preferredWidth") && json.contains("preferredHeight"))) sizeHintModes.sizeHintModePreferredSize = SizeHintMode::ARBITRARY;
		}
		if (!json.contains("sizeHintModeMinSize")) {
			if (json.contains("minWidth")) sizeHintModes.sizeHintModeMinSize = SizeHintMode::SET_TO_TEXT_HEIGHT;
			if (json.contains("minHeight")) sizeHintModes.sizeHintModeMinSize = SizeHintMode::SET_TO_TEXT_WIDTH;
			if (json.contains("minSize") || (json.contains("minWidth") && json.contains("minHeight"))) sizeHintModes.sizeHintModeMinSize = SizeHintMode::ARBITRARY;
		}
		if (!json.contains("sizeHintModeMaxSize")) {
			if (json.contains("maxWidth")) sizeHintModes.sizeHintModeMaxSize = SizeHintMode::SET_TO_TEXT_HEIGHT;
			if (json.contains("maxHeight")) sizeHintModes.sizeHintModeMaxSize = SizeHintMode::SET_TO_TEXT_WIDTH;
			if (json.contains("maxSize") || (json.contains("maxWidth") && json.contains("maxHeight"))) sizeHintModes.sizeHintModeMaxSize = SizeHintMode::ARBITRARY;
		}
		computeHeightHints();
		comouteWidthHints();
	}
	//--------------------------------------------------------------------------------------------------
	GuiTextBox::GuiTextBox(const GuiTextBox& other) noexcept
		: GuiPanel(other), dynamicText(other.dynamicText ? std::make_unique<DynamicText>(*other.dynamicText) : nullptr),
		material(other.material), textColor(other.textColor),
		modelMatrixText(other.modelMatrixText), textScaleMode(other.textScaleMode),
		sizeHintModes(other.sizeHintModes), border(other.border), scaleFactor(other.scaleFactor),
		fontSize(other.fontSize), recomputeTries(other.recomputeTries), 
		doRecomputeOnSizeChange(other.doRecomputeOnSizeChange),
		lastSizeOnRecomputeText(other.lastSizeOnRecomputeText), text(other.text),
		parseMode(other.parseMode), alignment(other.alignment), font(other.font) {}
	//--------------------------------------------------------------------------------------------------
	GuiTextBox& GuiTextBox::operator=(const GuiTextBox& other) noexcept
	{
		GuiPanel::operator=(other);
		dynamicText = other.dynamicText ? std::make_unique<DynamicText>(*other.dynamicText) : nullptr;
		material = other.material;
		textColor = other.textColor;
		modelMatrixText = other.modelMatrixText;
		textScaleMode = other.textScaleMode;
		sizeHintModes = other.sizeHintModes;
		border = other.border;
		scaleFactor = other.scaleFactor;
		fontSize = other.fontSize;
		recomputeTries = other.recomputeTries;
		doRecomputeOnSizeChange = other.doRecomputeOnSizeChange;
		lastSizeOnRecomputeText = other.lastSizeOnRecomputeText;
		text = other.text;
		parseMode = other.parseMode;
		alignment = other.alignment;
		font = other.font;
		return *this;
	}
	//--------------------------------------------------------------------------------------------------
	Vec2i GuiTextBox::getTextSize() const
	{
		if (dynamicText) return dynamicText->getTextSize();
		return Vec2i();
	}
	//--------------------------------------------------------------------------------------------------
	void GuiTextBox::setTextColor(const Color4f& textColor)
	{
		this->textColor = textColor;
		material = constructTextMaterial(font, textColor, getBackgroundColor());
	}
	//--------------------------------------------------------------------------------------------------
	void GuiTextBox::setTextScaleMode(TextScaleMode textScaleMode)
	{
		if (this->textScaleMode != textScaleMode) {
			this->textScaleMode = textScaleMode;
			recomputeText();
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiTextBox::setSizeHintModeMinSize(const SizeHintMode& sizeHintModeMinSize)
	{
		sizeHintModes.sizeHintModeMinSize = sizeHintModeMinSize;
		computeHeightHints();
		comouteWidthHints();
	}
	//--------------------------------------------------------------------------------------------------
	void GuiTextBox::setSizeHintModeMaxSize(const SizeHintMode& sizeHintModeMaxSize)
	{
		sizeHintModes.sizeHintModeMaxSize = sizeHintModeMaxSize;
		computeHeightHints();
		comouteWidthHints();
	}
	//--------------------------------------------------------------------------------------------------
	void GuiTextBox::setSizeHintModePreferredSize(const SizeHintMode& sizeHintModePreferredSize)
	{
		sizeHintModes.sizeHintModePreferredSize = sizeHintModePreferredSize;
		computeHeightHints();
		comouteWidthHints();
	}
	//--------------------------------------------------------------------------------------------------
	void GuiTextBox::setSizeHintModes(const SizeHintModes& sizeHintModes)
	{
		this->sizeHintModes = sizeHintModes;
		computeHeightHints();
		comouteWidthHints();
	}
	//--------------------------------------------------------------------------------------------------
	void GuiTextBox::setBorder(const int& border)
	{
		if (this->border != border) {
			this->border = border;
			recomputeText();
			comouteWidthHints();
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiTextBox::setFontSize(const double& fontSize)
	{
		if (this->fontSize != fontSize) {
			this->fontSize = fontSize;
			if (this->dynamicText) {
				this->dynamicText->setFontSize(fontSize, false);
				recomputeText();
				comouteWidthHints();
				computeModelMatrices();
			}
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiTextBox::setRecomputeTries(int recomputeTries)
	{
		this->recomputeTries = recomputeTries;
		recomputeText();
		computeModelMatrices();
	}
	//--------------------------------------------------------------------------------------------------
	void GuiTextBox::setDoRecomputeOnSizeChange(const bool& doRecomputeOnSizeChange)
	{
		if (this->doRecomputeOnSizeChange != doRecomputeOnSizeChange)
		{
			this->doRecomputeOnSizeChange = doRecomputeOnSizeChange;
			if (doRecomputeOnSizeChange) {
				recomputeTextOnSizeChange();
			}
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiTextBox::setText(const std::string& text)
	{
		this->text = text;
		if (this->dynamicText) {
			this->dynamicText->setText(text, false);
			recomputeText();
			comouteWidthHints();
			computeModelMatrices();
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiTextBox::setParseMode(const StaticText::ParseMode& parseMode)
	{
		if (this->parseMode != parseMode) {
			this->parseMode = parseMode;
			if (dynamicText) {
				dynamicText->setParseMode(parseMode, false);
				recomputeText();
				computeModelMatrices();
			}
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiTextBox::setAlignment(const StaticText::Alignment& alignment)
	{
		if (this->alignment != alignment) {
			this->alignment = alignment;
			if (dynamicText) {
				dynamicText->setAlignment(alignment, false);
				recomputeText();
				computeModelMatrices();
			}
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiTextBox::setFont(const Font& font)
	{
		if (this->font.fontID != font.fontID) {
			this->font = font;
			if (this->dynamicText) {
				this->dynamicText->setFont(font, false);
				material = constructTextMaterial(font, textColor, getBackgroundColor());
				recomputeText();
				comouteWidthHints();
				computeModelMatrices();
			}
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiTextBox::setBackgroundColor(const Color4f& backgroundColor)
	{
		GuiPanel::setBackgroundColor(backgroundColor);
		material = constructTextMaterial(font, textColor, backgroundColor);
	}
	//--------------------------------------------------------------------------------------------------
	std::unique_ptr<GuiElementAnimatable> GuiTextBox::animateTextColor(const Color4f& startVal, const Color4f& stopVal, double duration, std::function<double(double)> animationFunction)
	{
		class GuiTextBoxTextColorAnimatable : public GuiElementValueAnimatable<Color4f>
		{
			virtual void onAnimate(const Color4f& currentVal) override { if (element) static_cast<GuiTextBox*>(element)->setTextColor(currentVal); };
		public:
			GuiTextBoxTextColorAnimatable(GuiElement& element, const Color4f& startVal, const Color4f& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear)
				: GuiElementValueAnimatable<Color4f>(element, startVal, stopVal, duration, animationFunction) {}
		};
		return std::make_unique<GuiTextBoxTextColorAnimatable>(*this, startVal, stopVal, duration, animationFunction);
	}
	//--------------------------------------------------------------------------------------------------
	std::unique_ptr<GuiElementAnimatable> GuiTextBox::animateBorder(const int& startVal, const int& stopVal, double duration, std::function<double(double)> animationFunction)
	{
		class GuiTextBoxBorderAnimatable : public GuiElementValueAnimatable<int>
		{
			virtual void onAnimate(const int& currentVal) override { if (element) static_cast<GuiTextBox*>(element)->setBorder(currentVal); };
		public:
			GuiTextBoxBorderAnimatable(GuiElement& element, const int& startVal, const int& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear)
				: GuiElementValueAnimatable<int>(element, startVal, stopVal, duration, animationFunction) {}
		};
		return std::make_unique<GuiTextBoxBorderAnimatable>(*this, startVal, stopVal, duration, animationFunction);
	}
	//--------------------------------------------------------------------------------------------------
	std::unique_ptr<GuiElementAnimatable> GuiTextBox::animateFontSize(const double& startVal, const double& stopVal, double duration, std::function<double(double)> animationFunction)
	{
		class GuiTextBoxFontSizeAnimatable : public GuiElementValueAnimatable<double>
		{
			virtual void onAnimate(const double& currentVal) override { if (element) static_cast<GuiTextBox*>(element)->setFontSize(currentVal); };
		public:
			GuiTextBoxFontSizeAnimatable(GuiElement& element, const double& startVal, const double& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear)
				: GuiElementValueAnimatable<double>(element, startVal, stopVal, duration, animationFunction) {}
		};
		return std::make_unique<GuiTextBoxFontSizeAnimatable>(*this, startVal, stopVal, duration, animationFunction);
	}
	//--------------------------------------------------------------------------------------------------
	std::unique_ptr<GuiElementAnimatable> GuiTextBox::animateBackgroundColor(const Color4f& startVal, const Color4f& stopVal, double duration, std::function<double(double)> animationFunction)
	{
		class GuiTextBoxBackgroundColorAnimatable : public GuiElementValueAnimatable<Color4f>
		{
			virtual void onAnimate(const Color4f& currentVal) override { if (element) static_cast<GuiTextBox*>(element)->setBackgroundColor(currentVal); };
		public:
			GuiTextBoxBackgroundColorAnimatable(GuiElement& element, const Color4f& startVal, const Color4f& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear)
				: GuiElementValueAnimatable<Color4f>(element, startVal, stopVal, duration, animationFunction) {}
		};
		return std::make_unique<GuiTextBoxBackgroundColorAnimatable>(*this, startVal, stopVal, duration, animationFunction);
	}
	//--------------------------------------------------------------------------------------------------
	void GuiTextBox::draw(const Vec2i& worldPosition)
	{
		GuiManager* const& guiManager = getGuiManager();
		if (!guiManager) return;
		// Draw background
		GuiPanel::draw(worldPosition);
		pushClippingBox(worldPosition);
		// Draw text
		drawText(worldPosition);
		popClippingBox();
	}
	//--------------------------------------------------------------------------------------------------
	void GuiTextBox::onRegister()
	{
		onRegister(std::move(std::make_unique<DynamicText>(text, font, fontSize, inchesToPoints(static_cast<double>(getWidth() - 2 * border) / Engine::getDPI().y), parseMode, alignment)));
		comouteWidthHints();
		computeHeightHints();
	}
	//--------------------------------------------------------------------------------------------------
	void GuiTextBox::onSizeChange()
	{
		GuiPanel::onSizeChange();
		recomputeTextOnSizeChange();
	}
	//--------------------------------------------------------------------------------------------------
}