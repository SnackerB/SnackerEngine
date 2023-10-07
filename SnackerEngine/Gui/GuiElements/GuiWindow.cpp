#include "Gui\GuiElements\GuiWindow.h"
#include "Utility\Keys.h"
#include "Gui\GuiManager.h"
#include "Graphics\Renderer.h"

namespace SnackerEngine
{
	//--------------------------------------------------------------------------------------------------
	int GuiWindow::defaultResizeButtonSize = -1;
	Color4f GuiWindow::defaultResizeButtonColor = Color4f(0.1f, 0.1f, 1.0f, 0.8f);
	Color4f GuiWindow::defaultTopBarBackgroundColor = Color4f(0.2f, 0.2f, 1.0f, 1.0f);
	Color4f GuiWindow::defaultBackgroundColor = Color4f(0.1f, 0.1f, 0.1f, 1.0f);
	//--------------------------------------------------------------------------------------------------
	void GuiWindow::computeResizeButtonModelMatrix()
	{
		resizeButtonModelMatrix = Mat4f::TranslateAndScale(Vec3f(static_cast<float>(getWidth() - resizeButtonSize), static_cast<float>(-getHeight()), 0.0f), Vec3f(static_cast<float>(resizeButtonSize), static_cast<float>(resizeButtonSize), 0.0f));
	}
	//--------------------------------------------------------------------------------------------------
	bool GuiWindow::isCollidingWithResizeButton(const Vec2i& offset) const
	{
		return offset.x >= getWidth() - resizeButtonSize && offset.y >= getHeight() - offset.x + getWidth() - resizeButtonSize;
	}
	//--------------------------------------------------------------------------------------------------
	GuiWindow::GuiWindow(const Vec2i& position, const Vec2i& size, const std::string& name)
		: GuiPanel(position, size, ResizeMode::RESIZE_RANGE, defaultBackgroundColor)
	{
		topBar = GuiTextBox(Vec2i(), Vec2i(), name);
		topBar.setParseMode(StaticText::ParseMode::SINGLE_LINE);
		topBar.setBackgroundColor(defaultTopBarBackgroundColor);
	}
	//--------------------------------------------------------------------------------------------------
	GuiWindow::GuiWindow(const nlohmann::json& json, const nlohmann::json* data, std::set<std::string>* parameterNames)
		: GuiPanel(json, data, parameterNames)
	{
		topBar = GuiTextBox(Vec2i(), Vec2i(), "");
		topBar.setParseMode(StaticText::ParseMode::SINGLE_LINE);
		topBar.setBackgroundColor(defaultTopBarBackgroundColor);
		parseJsonOrReadFromData(resizeButtonSize, "resizeButtonSize", json, data, parameterNames);
		std::optional<int> topBarHeight = parseJsonOrReadFromData<int>("topBarHeight", json, data, parameterNames);
		if (topBarHeight.has_value()) topBar.setHeight(topBarHeight.value());
		std::optional<double> fontSize = parseJsonOrReadFromData<double>("fontSize", json, data, parameterNames);
		if (fontSize.has_value()) topBar.setFontSize(fontSize.value());
		std::optional<Font> font = parseJsonOrReadFromData<Font>("font", json, data, parameterNames);
		if (font.has_value()) topBar.setFont(font.value());
		std::optional<std::string> name = parseJsonOrReadFromData<std::string>("name", json, data, parameterNames);
		if (name.has_value()) topBar.setText(name.value());
		std::optional<Color4f> topBarColor = parseJsonOrReadFromData<Color4f>("topBarColor", json, data, parameterNames);
		if (topBarColor.has_value()) topBar.setBackgroundColor(topBarColor.value());
		std::optional<Color4f> textColor = parseJsonOrReadFromData<Color4f>("textColor", json, data, parameterNames);
		if (textColor.has_value()) topBar.setTextColor(textColor.value());
		parseJsonOrReadFromData(resizeButtonColor, "resizeButtonColor", json, data, parameterNames);
		if (!json.contains("backgroundColor")) setBackgroundColor(defaultBackgroundColor);
	}
	//--------------------------------------------------------------------------------------------------
	void GuiWindow::setResizeButtonSize(int resizeButtonSize)
	{
		this->resizeButtonSize = resizeButtonSize;
		computeResizeButtonModelMatrix();
	}
	//--------------------------------------------------------------------------------------------------
	void GuiWindow::draw(const Vec2i& worldPosition)
	{
		GuiManager* const& guiManager = getGuiManager();
		// Draw the panel first. Also draws all children.
		GuiPanel::draw(worldPosition);
		if (!guiManager) return;
		// Draw top bar
		drawElement(topBar.getGuiID(), worldPosition);
		// Draw resize button
		getPanelShader().bind();
		guiManager->setUniformViewAndProjectionMatrices(getPanelShader());
		Mat4f translationMatrix = Mat4f::Translate(Vec3f(static_cast<float>(worldPosition.x), static_cast<float>(-worldPosition.y), 0.0f));
		getPanelShader().setUniform<Mat4f>("u_model", translationMatrix * resizeButtonModelMatrix);
		getPanelShader().setUniform<Color4f>("u_color", resizeButtonColor);
		Renderer::draw(guiManager->getModelTriangle());
	}
	//--------------------------------------------------------------------------------------------------
	void GuiWindow::onRegister()
	{
		signUpEvent(CallbackType::MOUSE_BUTTON_ON_ELEMENT);
		registerElementAsChild(topBar);
		topBar.setSizeHintModeMinSize(GuiTextBox::SizeHintMode::SET_TO_TEXT_HEIGHT);
		topBar.setSizeHintModePreferredSize(GuiTextBox::SizeHintMode::SET_TO_TEXT_HEIGHT);
		topBar.setSizeHintModeMaxSize(GuiTextBox::SizeHintMode::SET_TO_TEXT_HEIGHT);
		if (resizeButtonSize == SIZE_HINT_ARBITRARY) setResizeButtonSize(topBar.getPreferredHeight());
		onSizeChange();
	}
	//--------------------------------------------------------------------------------------------------
	void GuiWindow::onSizeChange()
	{
		GuiPanel::onSizeChange();
		computeResizeButtonModelMatrix();
		topBar.setSize(Vec2i(getWidth(), topBar.getPreferredHeight()));
	}
	//--------------------------------------------------------------------------------------------------
	void GuiWindow::enforceLayout()
	{
		/// this is similar to enforceLayout() from the base GuiElement class,
		/// but only for the part below the topBar!
		Vec2i position = Vec2i(0, topBar.getHeight());
		Vec2i size = getSize() - Vec2i(0, position.y);
		for (const auto& childID : getChildren()) {
			auto child = getElement(childID);
			if (!child) continue;
			switch (child->getResizeMode())
			{
			case ResizeMode::SAME_AS_PARENT: setPositionAndSizeOfChild(childID, position, size); break;
			case ResizeMode::RESIZE_RANGE:
			{
				Vec2i childSize = Vec2i(0, 0);
				if (child->getPreferredWidth() >= 0) childSize.x = child->getPreferredWidth();
				else {
					if (child->getPreferredWidth() == SIZE_HINT_ARBITRARY) childSize.x = child->clampToMinMaxWidth(child->getWidth());
					else if (child->getPreferredWidth() == SIZE_HINT_AS_LARGE_AS_POSSIBLE) childSize.x = child->getMaxWidth();
				}
				if (child->getPreferredHeight() >= 0) childSize.y = child->getPreferredHeight();
				else {
					if (child->getPreferredHeight() == SIZE_HINT_ARBITRARY) childSize.y = child->clampToMinMaxHeight(child->getHeight());
					else if (child->getPreferredHeight() == SIZE_HINT_AS_LARGE_AS_POSSIBLE) childSize.y = child->getMaxHeight();
				}
				setSizeOfChild(childID, childSize);
				break;
			}
			default: break;
			}
		}
	}
	//--------------------------------------------------------------------------------------------------
	GuiElement::IsCollidingResult GuiWindow::isColliding(const Vec2i& offset) const
	{
		if (isCollidingBoundingBox(offset)) {
			// Check if the resize button or top bar was hit
			if (isCollidingWithResizeButton(offset) || topBar.isCollidingBoundingBox(offset - getPosition())) {
				return IsCollidingResult::COLLIDE_STRONG;
			}
			else {
				return IsCollidingResult::COLLIDE_IF_CHILD_DOES_NOT;
			}
		}
		return IsCollidingResult::NOT_COLLIDING;
	}
	//--------------------------------------------------------------------------------------------------
	void GuiWindow::callbackMouseButton(const int& button, const int& action, const int& mods)
	{
		if (button == MOUSE_BUTTON_LEFT && action == ACTION_RELEASE) {
			isMoving = false;
			isResizing = false;
			signOffEvent(CallbackType::MOUSE_MOTION);
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiWindow::callbackMouseMotion(const Vec2d& position)
	{
		if (isMoving) {
			GuiElement* parent = getElement(getParentID());
			if (parent) {
				Vec2f newPosition = parent->getMouseOffset() - mouseOffset;
				setPosition(newPosition);
			}
		}
		else if (isResizing) {
			Vec2f newSize = getMouseOffset() + Vec2f(static_cast<float>(resizeButtonSize)) - mouseOffset;
			// Clip size
			if (newSize.x < resizeButtonSize) newSize.x = static_cast<float>(resizeButtonSize);
			if (newSize.y < resizeButtonSize) newSize.y = static_cast<float>(resizeButtonSize);
			setSize(newSize);
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiWindow::callbackMouseButtonOnElement(const int& button, const int& action, const int& mods)
	{
		if (button == MOUSE_BUTTON_LEFT && action == ACTION_PRESS) {
			bringToForeground();
			// Figure out if the user clicked on the resize button or on the window
			mouseOffset = getMouseOffset();
			if (isCollidingWithResizeButton(mouseOffset)) {
				isResizing = true;
				// When were resizing the mouse offset is relative to the top left corner of the resize button
				mouseOffset = mouseOffset - getSize() + Vec2f(static_cast<float>(resizeButtonSize));
			}
			else {
				isMoving = true;
			}
			signUpEvent(CallbackType::MOUSE_MOTION);
			signUpEvent(CallbackType::MOUSE_BUTTON);
		}
	}
	//--------------------------------------------------------------------------------------------------
}