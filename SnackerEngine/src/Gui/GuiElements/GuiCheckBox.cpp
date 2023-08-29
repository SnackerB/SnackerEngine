#include "Gui\GuiElements\GuiCheckBox.h"

namespace SnackerEngine
{

	void GuiCheckBox::onButtonPress()
	{
		checked = !checked;
		if (boolHandle) boolHandle->set(checked);
		updateButtonColor();
		GuiButton::onButtonPress();
	}

	void GuiCheckBox::updateButtonColor()
	{
		if (checked) {
			setDefaultColor(colorDefaultTrue);
			setHoverColor(colorHoverTrue);
			setPressedColor(colorPressedTrue);
			setPressedHoverColor(colorHoverPressedTrue);
		}
		else {
			setDefaultColor(colorDefaultFalse);
			setHoverColor(colorHoverFalse);
			setPressedColor(colorPressedFalse);
			setPressedHoverColor(colorHoverPressedFalse);
		}
	}

	GuiCheckBox::GuiCheckBox(int size)
		: GuiButton(Vec2i(), Vec2i(size, size), "")
	{
		updateButtonColor();
	}

	GuiCheckBox::GuiCheckBox(const nlohmann::json& json, const nlohmann::json* data, std::set<std::string>* parameterNames)
	{
		parseJsonOrReadFromData(checked, "checked", json, data, parameterNames);
		parseJsonOrReadFromData(colorDefaultTrue, "colorDefaultTrue", json, data, parameterNames);
		parseJsonOrReadFromData(colorHoverTrue, "colorHoverTrue", json, data, parameterNames);
		parseJsonOrReadFromData(colorPressedTrue, "colorPressedTrue", json, data, parameterNames);
		parseJsonOrReadFromData(colorHoverPressedTrue, "colorHoverPressedTrue", json, data, parameterNames);
		parseJsonOrReadFromData(colorDefaultFalse, "colorDefaultFalse", json, data, parameterNames);
		parseJsonOrReadFromData(colorHoverFalse, "colorHoverFalse", json, data, parameterNames);
		parseJsonOrReadFromData(colorPressedFalse, "colorPressedFalse", json, data, parameterNames);
		parseJsonOrReadFromData(colorHoverPressedFalse, "colorHoverPressedFalse", json, data, parameterNames);
		auto size = parseJsonOrReadFromData<int>("size", json, data, parameterNames);
		if (size.has_value()) {
			setSize(Vec2i(size.value()));
			setPreferredSize(Vec2i(size.value()));
			setMinSize(Vec2i(size.value()));
			setMaxSize(Vec2i(size.value()));
		}
		auto preferredSize = parseJsonOrReadFromData<int>("preferredSize", json, data, parameterNames);
		if (preferredSize.has_value()) setPreferredSize(Vec2i(preferredSize.value()));
		auto minSize = parseJsonOrReadFromData<int>("minSize", json, data, parameterNames);
		if (minSize.has_value()) setMinSize(Vec2i(minSize.value()));
		auto maxSize = parseJsonOrReadFromData<int>("maxSize", json, data, parameterNames);
		if (maxSize.has_value()) setMaxSize(Vec2i(maxSize.value()));
		updateButtonColor();
	}

	GuiCheckBox::GuiCheckBox(const GuiCheckBox& other) noexcept
		: GuiButton(other), checked(other.checked), boolHandle(nullptr), colorDefaultTrue(other.colorDefaultTrue),
		colorHoverTrue(other.colorHoverTrue), colorPressedTrue(other.colorPressedTrue),
		colorHoverPressedTrue(other.colorHoverPressedTrue), colorDefaultFalse(other.colorDefaultFalse),
		colorHoverFalse(other.colorHoverFalse), colorPressedFalse(other.colorPressedFalse),
		colorHoverPressedFalse(other.colorHoverPressedFalse) {}

	GuiCheckBox& GuiCheckBox::operator=(const GuiCheckBox& other) noexcept
	{
		GuiButton::operator=(other);
		checked = other.checked; 
		if (boolHandle) signOffHandle(*boolHandle);
		boolHandle = nullptr;
		colorDefaultTrue = other.colorDefaultTrue;
		colorHoverTrue = other.colorHoverTrue; 
		colorPressedTrue = other.colorPressedTrue;
		colorHoverPressedTrue = other.colorHoverPressedTrue;
		colorDefaultFalse = other.colorDefaultFalse;
		colorHoverFalse = other.colorHoverFalse;  
		colorPressedFalse = other.colorPressedFalse;
		colorHoverPressedFalse = other.colorHoverPressedFalse;
		return *this;
	}

	GuiCheckBox::GuiCheckBox(GuiCheckBox&& other) noexcept
		: GuiButton(std::move(other)), checked(std::move(other.checked)), boolHandle(std::move(other.boolHandle)), 
		colorDefaultTrue(std::move(other.colorDefaultTrue)), colorHoverTrue(std::move(other.colorHoverTrue)), 
		colorPressedTrue(std::move(other.colorPressedTrue)), colorHoverPressedTrue(std::move(other.colorHoverPressedTrue)), 
		colorDefaultFalse(std::move(other.colorDefaultFalse)), colorHoverFalse(std::move(other.colorHoverFalse)), 
		colorPressedFalse(std::move(other.colorPressedFalse)), colorHoverPressedFalse(std::move(other.colorHoverPressedFalse))
	{
		other.boolHandle = nullptr;
		if (boolHandle) notifyHandleOnGuiElementMove(&other, *boolHandle);
	}

	GuiCheckBox& GuiCheckBox::operator=(GuiCheckBox&& other) noexcept
	{
		GuiButton::operator=(std::move(other));
		checked = std::move(other.checked);
		if (boolHandle) signOffHandle(*boolHandle);
		boolHandle = std::move(other.boolHandle);
		colorDefaultTrue = std::move(other.colorDefaultTrue);
		colorHoverTrue = std::move(other.colorHoverTrue);
		colorPressedTrue = std::move(other.colorPressedTrue);
		colorHoverPressedTrue = std::move(other.colorHoverPressedTrue);
		colorDefaultFalse = std::move(other.colorDefaultFalse);
		colorHoverFalse = std::move(other.colorHoverFalse);
		colorPressedFalse = std::move(other.colorPressedFalse);
		colorHoverPressedFalse = std::move(other.colorHoverPressedFalse);
		other.boolHandle = nullptr;
		if (boolHandle) notifyHandleOnGuiElementMove(&other, *boolHandle);
		return *this;
	}

	void GuiCheckBox::setChecked(bool checked)
	{
		if (this->checked != checked) {
			this->checked = checked;
			updateButtonColor();
			if (boolHandle) boolHandle->set(checked);
		}
	}

	bool GuiCheckBox::setBoolHandle(GuiVariableHandle<bool>& boolHandle)
	{
		if (this->boolHandle) return false;
		this->boolHandle = &boolHandle;
		signUpHandle(boolHandle, 1);
		return true;
	}

	void GuiCheckBox::draw(const Vec2i& worldPosition)
	{
		GuiButton::draw(worldPosition);
		// TODO: Draw check mark!
	}

	void GuiCheckBox::onHandleMove(GuiHandle& guiHandle)
	{
		auto result = guiHandle.getHandleID(*this);
		if (result.has_value() && result.value() == boolHandle->getHandleID(*this)) boolHandle = static_cast<GuiVariableHandle<bool>*>(&guiHandle);
		else GuiButton::onHandleMove(guiHandle);
	}

	void GuiCheckBox::onHandleDestruction(GuiHandle& guiHandle)
	{
		auto result = guiHandle.getHandleID(*this);
		if (result.has_value() && result.value() == boolHandle->getHandleID(*this)) boolHandle = nullptr;
		else GuiButton::onHandleDestruction(guiHandle);
	}

	void GuiCheckBox::onHandleUpdate(GuiHandle& guiHandle)
	{
		if (boolHandle) {
			checked = boolHandle->get();
			updateButtonColor();
		}
	}

}