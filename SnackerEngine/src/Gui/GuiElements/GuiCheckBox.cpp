#include "Gui/GuiElements/GuiCheckBox.h"

namespace SnackerEngine
{
	void GuiCheckBox::determineState()
	{
		if (!button) return;
		if (getValue())
		{
			button->setDefaultColor(colorDefaultTrue);
			button->setHoverColor(colorHoverTrue);
			button->setPressedCOlor(colorPressedTrue);
			button->setPressedHoverColor(ColorHoverPressedTrue);
		}
		else
		{
			button->setDefaultColor(colorDefaultFalse);
			button->setHoverColor(colorHoverFalse);
			button->setPressedCOlor(colorPressedFalse);
			button->setPressedHoverColor(ColorHoverPressedFalse);
		}
	}

	void GuiCheckBox::draw(const Vec2i& parentPosition)
	{
		Vec2i nextPosition = parentPosition + position;
		drawElement(label->getGuiID(), nextPosition);
		drawElement(button->getGuiID(), nextPosition);
		GuiElement::draw(parentPosition);
	}

	void GuiCheckBox::onRegister()
	{
		registerElementAsChild(*label);
		registerElementAsChild(*button);
	}

	void GuiCheckBox::onSizeChange()
	{
		// TODO?
	}

	GuiCheckBox::IsCollidingResult GuiCheckBox::isColliding(const Vec2i& position)
	{
		const Vec2i& myPosition = getPosition();
		const Vec2i& mySize = getSize();
		return (position.x > myPosition.x && position.x < myPosition.x + mySize.x
			&& position.y > myPosition.y && position.y < myPosition.y + mySize.y) ?
			IsCollidingResult::COLLIDE_CHILD : IsCollidingResult::NOT_COLLIDING;
	}

	GuiCheckBox::GuiID GuiCheckBox::getCollidingChild(const Vec2i& position)
	{
		IsCollidingResult result = GuiElement::isColliding(button->getGuiID(), position - getPosition());
		if (result != IsCollidingResult::NOT_COLLIDING) {
			GuiID childCollision = GuiElement::getCollidingChild(result, button->getGuiID(), position);
			if (childCollision > 0) return childCollision;
		}
		result = GuiElement::isColliding(label->getGuiID(), position - getPosition());
		if (result != IsCollidingResult::NOT_COLLIDING) {
			GuiID childCollision = GuiElement::getCollidingChild(result, label->getGuiID(), position);
			if (childCollision > 0) return childCollision;
		}
		return GuiElement::getCollidingChild(position);
	}

	void GuiCheckBox::onHandleDestruction(GuiHandle& guiHandle)
	{
		boolHandle = nullptr;
	}

	void GuiCheckBox::onHandleMove(GuiHandle& guiHandle)
	{
		// Update pointer
		boolHandle = static_cast<GuiVariableHandle<bool>*>(&guiHandle);
	}

	void GuiCheckBox::onHandleUpdate(GuiHandle& guiHandle)
	{
		determineState();
	}

	void GuiCheckBox::setValue(const bool& value)
	{
		if (boolHandle) {
			setVariableHandleValue<bool>(*boolHandle, value);
		}
	}

	const bool& GuiCheckBox::getValue() const
	{
		if (boolHandle) return boolHandle->get();
		return false;
	}

	void GuiCheckBox::toggle()
	{
	}

	GuiCheckBox::GuiCheckBox(const Vec2i& position, const Vec2i& size, const GuiElement::ResizeMode& resizeMode, const std::string& label, const Font& font, const double& fontSize, const Color4f& labelTextColor, const Color4f& labelTextBackgroundColor, const int& border, const Color4f& CheckBoxButtonDefaultColor, const Color4f& CheckBoxButtonHoverColor, const Color4f& CheckBoxButtonPressedColor, const Color4f& CheckBoxButtonPressedHoverColor)
	{
	}

	GuiCheckBox::GuiCheckBox(GuiVariableHandle<bool>& boolHandle, const Vec2i& position, const Vec2i& size, const GuiElement::ResizeMode& resizeMode, const std::string& label, const Font& font, const double& fontSize, const Color4f& labelTextColor, const Color4f& labelTextBackgroundColor, const int& border, const Color4f& CheckBoxButtonDefaultColor, const Color4f& CheckBoxButtonHoverColor, const Color4f& CheckBoxButtonPressedColor, const Color4f& CheckBoxButtonPressedHoverColor)
	{
	}

	GuiCheckBox::GuiCheckBox(const std::string& label, GuiVariableHandle<bool>& handle, const GuiStyle& style)
	{
	}

	GuiCheckBox::GuiCheckBox(const std::string& label, const GuiStyle& style)
	{
	}

	GuiCheckBox::GuiCheckBox(const std::string& label, const double& fontSize, GuiVariableHandle<bool>& handle, const GuiStyle& style)
	{
	}

	GuiCheckBox::GuiCheckBox(const std::string& label, const double& fontSize, const GuiStyle& style)
	{
	}

	void GuiCheckBox::setBoolHandle(GuiVariableHandle<bool>& variableHandle)
	{
	}

	GuiCheckBox::GuiCheckBox(const GuiCheckBox& other) noexcept
	{
	}

	GuiCheckBox& GuiCheckBox::operator=(const GuiCheckBox& other) noexcept
	{
		// TODO: insert return statement here
	}

	GuiCheckBox::GuiCheckBox(GuiCheckBox&& other) noexcept
	{
	}

	GuiCheckBox& GuiCheckBox::operator=(GuiCheckBox&& other) noexcept
	{
		// TODO: insert return statement here
	}

	GuiCheckBox::~GuiCheckBox()
	{
	}

}