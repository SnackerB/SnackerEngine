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
	GuiTextBox::SizeHintModes GuiTextBox::defaultSizeHintModes = { GuiTextBox::SizeHintMode::ARBITRARY, GuiTextBox::SizeHintMode::ARBITRARY, GuiTextBox::SizeHintMode::ARBITRARY };
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
			json == "SCALE_DOWN_HORIZONTAL" ||
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
		if (json == "SCALE_DOWN_HORIZONTAL") return GuiTextBox::TextScaleMode::SCALE_DOWN_HORIZONTAL;
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
	template<> std::string to_string(const GuiTextBox::SizeHintMode& sizeHintMode)
	{
		switch (sizeHintMode)
		{
			case GuiTextBox::SizeHintMode::ARBITRARY: return "ARBITRATRY";
			case GuiTextBox::SizeHintMode::SET_TO_TEXT_WIDTH: return "SET_TO_TEXT_WIDTH";
			case GuiTextBox::SizeHintMode::SET_TO_TEXT_HEIGHT: return "SET_TO_TEXT_HEIGHT";
			case GuiTextBox::SizeHintMode::SET_TO_TEXT_SIZE: return "SET_TO_TEXT_SIZE";
			default: return "ARBITRATRY";
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiTextBox::computeModelMatrices()
	{
		if (!dynamicText) return;
		scaleFactor = 1.0;
		switch (textScaleMode)
		{
		case SnackerEngine::GuiTextBox::TextScaleMode::SCALE_UP:
		{
			double textWidth = pointsToPixels((dynamicText->getRight() - dynamicText->getLeft()));
			if (textWidth > getWidth() - 2 * border) break;
			double textHeight = pointsToPixels((dynamicText->getTop() - dynamicText->getBottom()));
			if (textHeight > getHeight() - 2 * border) break;
			scaleFactor = std::min((getWidth() - 2 * border) / textWidth, (getHeight() - 2 * border) / textHeight);
			break;
		}
		case SnackerEngine::GuiTextBox::TextScaleMode::SCALE_DOWN:
		{
			double textWidth = pointsToPixels((dynamicText->getRight() - dynamicText->getLeft()));
			double textHeight = pointsToPixels((dynamicText->getTop() - dynamicText->getBottom()));
			if (textWidth < getWidth() - 2 * border && textHeight < getHeight() - 2 * border) break;
			scaleFactor = std::min((getWidth() - 2 * border) / textWidth, (getHeight() - 2 * border) / textHeight);
			break;
		}
		case SnackerEngine::GuiTextBox::TextScaleMode::SCALE_DOWN_HORIZONTAL:
		{
			double textWidth = pointsToPixels((dynamicText->getRight() - dynamicText->getLeft()));
			if (textWidth < getWidth() - 2 * border) break;
			scaleFactor = (getWidth() - 2 * border) / textWidth;
			break;
		}
		case SnackerEngine::GuiTextBox::TextScaleMode::SCALE_UP_DOWN:
		{
			double textWidth = pointsToPixels((dynamicText->getRight() - dynamicText->getLeft()));
			double textHeight = pointsToPixels((dynamicText->getTop() - dynamicText->getBottom()));
			scaleFactor = std::min((getWidth() - 2 * border) / textWidth, (getHeight() - 2 * border) / textHeight);
			break;
		}
		default:
		{
			break;
		}
		}
		Vec2f textPosition = computeTextPosition();
		if (dynamicText) modelMatrixText = Mat4f::TranslateAndScale(Vec3f(textPosition.x, textPosition.y, 0), Vec3f(static_cast<float>(pointsToPixels(dynamicText->getFontSize()) * scaleFactor)));
	}
	//--------------------------------------------------------------------------------------------------
	void GuiTextBox::recomputeText()
	{
		if (!dynamicText) return;
		if (textScaleMode == TextScaleMode::RECOMPUTE_DOWN) {
			dynamicText->setTextWidth((getWidth() - 2 * border) / pointsToPixels(1.0f), false);
			dynamicText->setFontSize(fontSize, false);
			dynamicText->setLineHeight(lineHeight, false);
			double maxFontSize = 0.0;
			double minFontSize = 0.0;
			for (unsigned int i = 0; i < recomputeTries; ++i)
			{
				dynamicText->recompute();
				double textHeight = pointsToPixels((dynamicText->getTop() - dynamicText->getBottom()));
				// Check text width only in single line parse mode!
				if (textHeight < (getHeight() - 2 * border) && (getParseMode() != StaticText::ParseMode::SINGLE_LINE || pointsToPixels((dynamicText->getRight() - dynamicText->getLeft())) < getWidth() - 2 * border)) {
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
			dynamicText->setTextWidth((getWidth() - 2 * border) / pointsToPixels(1.0f), false);
			dynamicText->setFontSize(fontSize, false);
			double maxFontSize = 0.0;
			double minFontSize = 0.0;
			//infoLogger << "recomputing text, starting with font size " << text->getFontSize() << LOGGER::ENDL;
			for (unsigned int i = 0; i < recomputeTries; ++i)
			{
				dynamicText->recompute();
				double textHeight = pointsToPixels((dynamicText->getTop() - dynamicText->getBottom()));
				// Check text width only in single line parse mode!
				if (textHeight < (getHeight() - 2 * border) && (getParseMode() != StaticText::ParseMode::SINGLE_LINE || pointsToPixels((dynamicText->getRight() - dynamicText->getLeft())) < (getHeight() - 2 * border))) {
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
			dynamicText->setTextWidth((getWidth() - 2 * border) / pointsToPixels(1.0f), false);
			dynamicText->recompute();
			computeModelMatrices();
		}
		//infoLogger << LOGGER::BEGIN << "Recomputed text! size: " << dynamicText->getTextSize() << LOGGER::ENDL;
		computeHeightHints();
		comouteWidthHints();
	}
	//--------------------------------------------------------------------------------------------------
	void GuiTextBox::onRegister(std::unique_ptr<DynamicText>&& dynamicText)
	{
		GuiPanel::onRegister();
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
		// Compute offset due to centering
		Vec2f textOffset = Vec2f(0.0f, 0.0f);
		// Compute horizontal alignment
		switch (dynamicText->getAlignment())
		{
		case AlignmentHorizontal::LEFT:
		{
			// Is already aligned to the left!
			break;
		}
		case AlignmentHorizontal::CENTER:
		{
			// Align to the center of the text box
			textOffset.x = static_cast<float>(static_cast<double>(getWidth() - 2 * border) / 2.0 - pointsToPixels((dynamicText->getRight() - dynamicText->getLeft()) / 2.0) * scaleFactor);
			break;
		}
		case AlignmentHorizontal::RIGHT:
		{
			// Align to the right of the text box
			textOffset.x = static_cast<float>(static_cast<double>(getWidth() - 2 * border) - pointsToPixels((dynamicText->getRight() - dynamicText->getLeft())) * scaleFactor);
			break;
		}
		default:
			break;
		}
		// Compute vertical alignment
		switch (alignmentVertical)
		{
		case AlignmentVertical::TOP:
		{
			// Align to the top of the text box
			textOffset.y = static_cast<float>(-pointsToPixels(dynamicText->getTop()) * scaleFactor);
			break;
		}
		case AlignmentVertical::CENTER:
		{
			// Align to the center of the text box
			textOffset.y = static_cast<float>(-pointsToPixels((dynamicText->getTop() + dynamicText->getBottom()) / 2.0) * scaleFactor - static_cast<double>(getHeight() - 2 * border) / 2.0);
			break;
		}
		case AlignmentVertical::BOTTOM:
		{
			// Align to the bottom of the text box
			textOffset.y = static_cast<float>(-pointsToPixels((dynamicText->getTop() - dynamicText->getBottom())) * scaleFactor - static_cast<double>(getHeight() - 2 * border));
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
	void GuiTextBox::drawText(const Vec2i& worldPosition, const Mat4f& transformMatrix)
	{
		GuiManager* const& guiManager = getGuiManager();
		if (!guiManager) return;
		if (dynamicText) {
			Mat4f translationMatrix = Mat4f::Translate(Vec3f(static_cast<float>(worldPosition.x), static_cast<float>(-worldPosition.y), 0.0f));
			material.bind();
			guiManager->setUniformViewAndProjectionMatrices(material.getShader());
			material.getShader().setUniform<Mat4f>("u_model", translationMatrix * transformMatrix * modelMatrixText);
			material.getShader().setUniform<float>("u_pxRange", static_cast<float>(dynamicText->getFont().getPixelRange()));
			SnackerEngine::Renderer::draw(dynamicText->getModel(), material);
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiTextBox::computeHeightHints()
	{
		if (!dynamicText) return;
		int textHeight = static_cast<int>(std::ceil((dynamicText->getTop() - dynamicText->getBottom()) * pointsToPixels(1.0)));
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
		//infoLogger << LOGGER::BEGIN << "height hints: " << getMinHeight() << " " << getMaxHeight() << " " << getPreferredHeight() << LOGGER::ENDL;
	}
	//--------------------------------------------------------------------------------------------------
	void GuiTextBox::comouteWidthHints()
	{
		if (!dynamicText) return;
		int textWidth{};
		if (parseMode == StaticText::ParseMode::SINGLE_LINE) {
			textWidth = static_cast<int>(std::ceil((dynamicText->getRight() - dynamicText->getLeft()) * pointsToPixels(1.0)));
		}
		else {
			DynamicText tempText(this->text, this->font, this->fontSize, 0.0f, this->lineHeight, StaticText::ParseMode::SINGLE_LINE, this->alignmentHorizontal);
			textWidth = static_cast<int>(std::ceil((tempText.getRight() - tempText.getLeft()) * pointsToPixels(1.0)));
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
		//infoLogger << LOGGER::BEGIN << "widht hints: " << getMinWidth() << " " << getMaxWidth() << " " << getPreferredWidth() << LOGGER::ENDL;
	}
	//--------------------------------------------------------------------------------------------------
	Material GuiTextBox::constructTextMaterial(const Font& font, const Color4f& textColor, const Color4f& backgroundColor)
	{
		return Material(std::move(std::make_unique<SimpleTextMaterialData>(Shader("shaders/gui/basic2DText.shader"),
			font, textColor)));
	}
	//--------------------------------------------------------------------------------------------------
	GuiTextBox::GuiTextBox(const Vec2i& position, const Vec2i& size, const std::string& text, const Font& font, const double& fontSize, std::optional<double> lineHeight, const Color4f& backgroundColor)
		: GuiPanel(position, size, ResizeMode::RESIZE_RANGE, backgroundColor), fontSize(fontSize), lineHeight(lineHeight), text(text), font(font) {}
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
		if (textScaleMode == TextScaleMode::SCALE_DOWN || 
			textScaleMode == TextScaleMode::SCALE_DOWN_HORIZONTAL || 
			textScaleMode == TextScaleMode::RECOMPUTE_DOWN) {
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
		lineHeight = parseJsonOrReadFromData<double>("lineHeight", json, data, parameterNames);
		parseJsonOrReadFromData(recomputeTries, "recomputeTries", json, data, parameterNames);
		parseJsonOrReadFromData(doRecomputeOnSizeChange, "doRecomputeOnSizeChange", json, data, parameterNames);
		parseJsonOrReadFromData(text, "text", json, data, parameterNames);
		parseJsonOrReadFromData(parseMode, "parseMode", json, data, parameterNames);
		parseJsonOrReadFromData(alignmentHorizontal, "alignmentHorizontal", json, data, parameterNames);
		parseJsonOrReadFromData(alignmentVertical, "alignmentVertical", json, data, parameterNames);
		parseJsonOrReadFromData(font, "font", json, data, parameterNames);
		if (!json.contains("backgroundColor")) setBackgroundColor(defaultBackgroundColor);
		if (json.contains("size") || (json.contains("width") && json.contains("height"))) {
			sizeHintModes.sizeHintModeMinSize = SizeHintMode::ARBITRARY;
			sizeHintModes.sizeHintModeMaxSize = SizeHintMode::ARBITRARY;
			sizeHintModes.sizeHintModePreferredSize = SizeHintMode::ARBITRARY;
		}
		if (!json.contains("sizeHintModePreferredSize")) {
			if (json.contains("preferredWidth")) sizeHintModes.sizeHintModePreferredSize = SizeHintMode::SET_TO_TEXT_HEIGHT;
			else if (json.contains("preferredHeight")) sizeHintModes.sizeHintModePreferredSize = SizeHintMode::SET_TO_TEXT_WIDTH;
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
		//SnackerEngine::infoLogger << SnackerEngine::LOGGER::BEGIN << SnackerEngine::to_string(sizeHintModes.sizeHintModeMinSize) << " " << SnackerEngine::to_string(sizeHintModes.sizeHintModeMaxSize) << " " << SnackerEngine::to_string(sizeHintModes.sizeHintModePreferredSize) << SnackerEngine::LOGGER::ENDL; // DEBUG
		computeHeightHints();
		comouteWidthHints();
	}
	//--------------------------------------------------------------------------------------------------
	GuiTextBox::GuiTextBox(const GuiTextBox& other) noexcept
		: GuiPanel(other), dynamicText(other.dynamicText ? std::make_unique<DynamicText>(*other.dynamicText) : nullptr),
		material(other.material), textColor(other.textColor),
		modelMatrixText(other.modelMatrixText), textScaleMode(other.textScaleMode),
		sizeHintModes(other.sizeHintModes), border(other.border), scaleFactor(other.scaleFactor),
		fontSize(other.fontSize), lineHeight(other.lineHeight), recomputeTries(other.recomputeTries), 
		doRecomputeOnSizeChange(other.doRecomputeOnSizeChange),
		lastSizeOnRecomputeText(other.lastSizeOnRecomputeText), text(other.text),
		parseMode(other.parseMode), alignmentHorizontal(other.alignmentHorizontal),
		alignmentVertical(other.alignmentVertical), font(other.font) {}
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
		lineHeight = other.lineHeight;
		recomputeTries = other.recomputeTries;
		doRecomputeOnSizeChange = other.doRecomputeOnSizeChange;
		lastSizeOnRecomputeText = other.lastSizeOnRecomputeText;
		text = other.text;
		parseMode = other.parseMode;
		alignmentHorizontal = other.alignmentHorizontal;
		alignmentVertical = other.alignmentVertical;
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
	void GuiTextBox::setLineHeight(std::optional<double> lineHeight)
	{
		if (this->lineHeight != lineHeight) {
			this->lineHeight = lineHeight;
			if (this->dynamicText) {
				this->dynamicText->setLineHeight(lineHeight, false);
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
	void GuiTextBox::setAlignmentHorizontal(AlignmentHorizontal alignmentHorizontal)
	{
		if (this->alignmentHorizontal != alignmentHorizontal) {
			this->alignmentHorizontal = alignmentHorizontal;
			if (dynamicText) {
				dynamicText->setAlignment(alignmentHorizontal, false);
				recomputeText();
				computeModelMatrices();
			}
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiTextBox::setAlignmentVertical(AlignmentVertical alignmentVertical)
	{
		if (this->alignmentVertical != alignmentVertical) {
			this->alignmentVertical = alignmentVertical;
			recomputeText();
			computeModelMatrices();
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
	void GuiTextBox::draw(const Vec2i& worldPosition, const Mat4f& transformMatrix)
	{
		GuiManager* const& guiManager = getGuiManager();
		if (!guiManager) return;
		// Draw background
		GuiPanel::draw(worldPosition, transformMatrix);
		pushClippingBox(worldPosition);
		// Draw text
		drawText(worldPosition, transformMatrix);
		popClippingBox();
	}
	//--------------------------------------------------------------------------------------------------
	void GuiTextBox::draw(const Vec2i& worldPosition, const Mat4f& transformMatrix, const Vec2i& clippingBoxOffset, const Vec2i& clippingBoxSize)
	{
		GuiManager* const& guiManager = getGuiManager();
		if (!guiManager) return;
		// Draw background
		GuiPanel::draw(worldPosition, transformMatrix);
		pushClippingBox(worldPosition, clippingBoxOffset, clippingBoxSize);
		// Draw text
		drawText(worldPosition, transformMatrix);
		popClippingBox();
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
		onRegister(std::move(std::make_unique<DynamicText>(text, font, fontSize, pixelsToPoints(static_cast<double>(getWidth() - 2 * border)), lineHeight, parseMode, alignmentHorizontal)));
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