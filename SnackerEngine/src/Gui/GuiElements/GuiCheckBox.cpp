#include "Gui/GuiElements/GuiCheckBox.h"
#include "Gui/GuiStyle.h"

namespace SnackerEngine
{

	void GuiCheckBox::determineState()
	{
		if (!button) return;
		if (getValue())
		{
			button->setDefaultColor(colorDefaultTrue);
			button->setHoverColor(colorHoverTrue);
			button->setPressedColor(colorPressedTrue);
			button->setPressedHoverColor(ColorHoverPressedTrue);
		}
		else
		{
			button->setDefaultColor(colorDefaultFalse);
			button->setHoverColor(colorHoverFalse);
			button->setPressedColor(colorPressedFalse);
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
		setValue(!getValue());
		determineState();
	}

	GuiCheckBox::GuiCheckBox(const Vec2i& position, const Vec2i& size, const GuiElement::ResizeMode& resizeMode, const std::string& label, const Font& font, const double& fontSize, const Color4f& labelTextColor, const Color4f& labelTextBackgroundColor, const int& border, const Color4f& checkBoxButtonDefaultColorTrue, const Color4f& checkBoxButtonHoverColorTrue, const Color4f& checkBoxButtonPressedColorTrue, const Color4f& checkBoxButtonPressedHoverColorTrue, const Color4f& checkBoxButtonDefaultColorFalse, const Color4f& checkBoxButtonHoverColorFalse, const Color4f& checkBoxButtonPressedColorFalse, const Color4f& checkBoxButtonPressedHoverColorFalse)
		: GuiElement(position, size, resizeMode), boolHandle(nullptr), button(std::make_unique<GuiButton>(Vec2i(), Vec2i(), GuiDynamicTextBox::ResizeMode::DO_NOT_RESIZE, checkBoxButtonDefaultColorFalse, checkBoxButtonHoverColorFalse, checkBoxButtonPressedColorFalse, checkBoxButtonPressedHoverColorFalse, "", font, fontSize, labelTextColor, StaticText::ParseMode::SINGLE_LINE)),
		label(std::make_unique<GuiDynamicTextBox>(Vec2i(), Vec2i(), GuiDynamicTextBox::ResizeMode::DO_NOT_RESIZE, label, font, fontSize, labelTextColor, labelTextBackgroundColor, DynamicText::ParseMode::SINGLE_LINE, DynamicText::Alignment::LEFT, border, GuiDynamicTextBox::TextScaleMode::DONT_SCALE, GuiDynamicTextBox::SizeHintModes(GuiDynamicTextBox::SizeHintMode::SET_TO_TEXT_SIZE), false)),
		colorDefaultTrue(checkBoxButtonDefaultColorTrue), colorHoverTrue(checkBoxButtonHoverColorTrue), colorPressedTrue(checkBoxButtonPressedColorTrue), ColorHoverPressedTrue(checkBoxButtonPressedHoverColorTrue),
		colorDefaultFalse(checkBoxButtonDefaultColorFalse), colorHoverFalse(checkBoxButtonHoverColorFalse), colorPressedFalse(checkBoxButtonPressedColorFalse), ColorHoverPressedFalse(checkBoxButtonPressedHoverColorFalse),
		toggleEventHandle(*this)
	{
		button->setEventHandle(toggleEventHandle);
		button->setSize(Vec2i(this->label->getPreferredSize().y, this->label->getPreferredSize().y));
		this->label->setPosition(Vec2i(this->label->getPreferredSize().y + border, 0));
		preferredSize = Vec2i(this->label->getPreferredSize().x + this->label->getPreferredSize().y + border, this->label->getPreferredSize().y);
	}

	GuiCheckBox::GuiCheckBox(GuiVariableHandle<bool>& boolHandle, const Vec2i& position, const Vec2i& size, const GuiElement::ResizeMode& resizeMode, const std::string& label, const Font& font, const double& fontSize, const Color4f& labelTextColor, const Color4f& labelTextBackgroundColor, const int& border, const Color4f& checkBoxButtonDefaultColorTrue, const Color4f& checkBoxButtonHoverColorTrue, const Color4f& checkBoxButtonPressedColorTrue, const Color4f& checkBoxButtonPressedHoverColorTrue, const Color4f& checkBoxButtonDefaultColorFalse, const Color4f& checkBoxButtonHoverColorFalse, const Color4f& checkBoxButtonPressedColorFalse, const Color4f& checkBoxButtonPressedHoverColorFalse)
		: GuiCheckBox(position, size, resizeMode, label, font, fontSize, labelTextColor, labelTextBackgroundColor, border, checkBoxButtonDefaultColorTrue, checkBoxButtonHoverColorTrue, checkBoxButtonPressedColorTrue, checkBoxButtonPressedHoverColorTrue, checkBoxButtonDefaultColorFalse, checkBoxButtonHoverColorFalse, checkBoxButtonPressedColorFalse, checkBoxButtonPressedHoverColorFalse)
	{
		setBoolHandle(boolHandle);
	}

	GuiCheckBox::GuiCheckBox(const std::string& label, GuiVariableHandle<bool>& handle, const GuiStyle& style)
		: GuiCheckBox(handle, Vec2i(), Vec2i(), GuiElement::ResizeMode::DO_NOT_RESIZE, label, style.defaultFont, style.fontSizeNormal, style.guiButtonTextColor, Color4f(0.0f), style.guiTextBoxBorder, style.checkBoxButtonDefaultColorTrue, style.checkBoxButtonHoverColorTrue, style.checkBoxButtonPressedColorTrue, style.checkBoxButtonHoverColorTrue, style.checkBoxButtonDefaultColorFalse, style.checkBoxButtonHoverColorFalse, style.checkBoxButtonPressedColorFalse, style.checkBoxButtonPressedHoverColorFalse) {}

	GuiCheckBox::GuiCheckBox(const std::string& label, const GuiStyle& style)
		: GuiCheckBox(Vec2i(), Vec2i(), GuiElement::ResizeMode::DO_NOT_RESIZE, label, style.defaultFont, style.fontSizeNormal, style.guiButtonTextColor, Color4f(0.0f), style.guiTextBoxBorder, style.checkBoxButtonDefaultColorTrue, style.checkBoxButtonHoverColorTrue, style.checkBoxButtonPressedColorTrue, style.checkBoxButtonHoverColorTrue, style.checkBoxButtonDefaultColorFalse, style.checkBoxButtonHoverColorFalse, style.checkBoxButtonPressedColorFalse, style.checkBoxButtonPressedHoverColorFalse) {}

	GuiCheckBox::GuiCheckBox(const std::string& label, const double& fontSize, GuiVariableHandle<bool>& handle, const GuiStyle& style)
		: GuiCheckBox(handle, Vec2i(), Vec2i(), GuiElement::ResizeMode::DO_NOT_RESIZE, label, style.defaultFont, fontSize, style.guiButtonTextColor, Color4f(0.0f), style.guiTextBoxBorder, style.checkBoxButtonDefaultColorTrue, style.checkBoxButtonHoverColorTrue, style.checkBoxButtonPressedColorTrue, style.checkBoxButtonHoverColorTrue, style.checkBoxButtonDefaultColorFalse, style.checkBoxButtonHoverColorFalse, style.checkBoxButtonPressedColorFalse, style.checkBoxButtonPressedHoverColorFalse) {}

	GuiCheckBox::GuiCheckBox(const std::string& label, const double& fontSize, const GuiStyle& style)
		: GuiCheckBox(Vec2i(), Vec2i(), GuiElement::ResizeMode::DO_NOT_RESIZE, label, style.defaultFont, fontSize, style.guiButtonTextColor, Color4f(0.0f), style.guiTextBoxBorder, style.checkBoxButtonDefaultColorTrue, style.checkBoxButtonHoverColorTrue, style.checkBoxButtonPressedColorTrue, style.checkBoxButtonHoverColorTrue, style.checkBoxButtonDefaultColorFalse, style.checkBoxButtonHoverColorFalse, style.checkBoxButtonPressedColorFalse, style.checkBoxButtonPressedHoverColorFalse) {}

	void GuiCheckBox::setBoolHandle(GuiVariableHandle<bool>& boolHandle)
	{
		if (!this->boolHandle) {
			this->boolHandle = &boolHandle;
			signUpHandle(boolHandle, 0);
			determineState();
		}
	}

	GuiCheckBox::GuiCheckBox(const GuiCheckBox& other) noexcept
		: GuiElement(other), boolHandle(nullptr), button(std::make_unique<GuiButton>(*other.button)), label(std::make_unique<GuiDynamicTextBox>(*other.label)),
		colorDefaultTrue(other.colorDefaultTrue), colorHoverTrue(other.colorHoverTrue), colorPressedTrue(other.colorPressedTrue),
		ColorHoverPressedTrue(other.ColorHoverPressedTrue), colorDefaultFalse(other.colorDefaultFalse), colorHoverFalse(other.colorHoverFalse),
		colorPressedFalse(other.colorPressedFalse), ColorHoverPressedFalse(other.ColorHoverPressedFalse), toggleEventHandle(*this) {}

	GuiCheckBox& GuiCheckBox::operator=(const GuiCheckBox& other) noexcept
	{
		GuiElement::operator=(other);
		if (boolHandle) {
			signOffHandle(*boolHandle);
		}
		boolHandle = nullptr;
		button = std::make_unique<GuiButton>(*other.button);
		label = std::make_unique<GuiDynamicTextBox>(*other.label);
		colorDefaultTrue = other.colorDefaultTrue;
		colorHoverTrue = other.colorHoverTrue;
		colorPressedTrue = other.colorPressedTrue;
		ColorHoverPressedTrue = other.ColorHoverPressedTrue;
		colorDefaultFalse = other.colorDefaultFalse;
		colorHoverFalse = other.colorHoverFalse;
		colorPressedFalse = other.colorPressedFalse;
		ColorHoverPressedFalse = other.ColorHoverPressedFalse;
		toggleEventHandle = GuiCheckBoxToggleEventHandle(*this);
		return *this;
	}

	GuiCheckBox::GuiCheckBox(GuiCheckBox&& other) noexcept
		: GuiElement(std::move(other)), boolHandle(other.boolHandle), button(std::move(other.button)), label(std::move(other.label)),
		colorDefaultTrue(other.colorDefaultTrue), colorHoverTrue(other.colorHoverTrue), colorPressedTrue(other.colorPressedTrue),
		ColorHoverPressedTrue(other.ColorHoverPressedTrue), colorDefaultFalse(other.colorDefaultFalse), colorHoverFalse(other.colorHoverFalse),
		colorPressedFalse(other.colorPressedFalse), ColorHoverPressedFalse(other.ColorHoverPressedFalse), toggleEventHandle(std::move(other.toggleEventHandle))
	{
		toggleEventHandle.guiCheckBox = this;
		if (boolHandle) notifyHandleOnGuiElementMove(&other, *boolHandle);
		// Set the other checkBox to a well-defined state
		other.boolHandle = nullptr;
		other.button = nullptr;
		other.label = nullptr;
	}

	GuiCheckBox& GuiCheckBox::operator=(GuiCheckBox&& other) noexcept
	{
		GuiElement::operator=(std::move(other));
		if (boolHandle) {
			signOffHandle(*boolHandle);
		}
		boolHandle = other.boolHandle;
		button = std::move(other.button);
		label = std::move(other.label);
		colorDefaultTrue = other.colorDefaultTrue;
		colorHoverTrue = other.colorHoverTrue;
		colorPressedTrue = other.colorPressedTrue;
		ColorHoverPressedTrue = other.ColorHoverPressedTrue;
		colorDefaultFalse = other.colorDefaultFalse;
		colorHoverFalse = other.colorHoverFalse;
		colorPressedFalse = other.colorPressedFalse;
		ColorHoverPressedFalse = other.ColorHoverPressedFalse;
		toggleEventHandle = std::move(other.toggleEventHandle);
		toggleEventHandle.guiCheckBox = this;
		if (boolHandle) notifyHandleOnGuiElementMove(&other, *boolHandle);
		// Set the other checkBox to a well-defined state
		other.boolHandle = nullptr;
		other.button = nullptr;
		other.label = nullptr;
		return *this;
	}

	GuiCheckBox::~GuiCheckBox()
	{
		if (button) signOffWithoutNotifyingParents(button->getGuiID());
		button = nullptr;
		if (label) signOffWithoutNotifyingParents(label->getGuiID());
		label = nullptr;
		if (boolHandle) signOffHandle(*boolHandle);
	}

}