#include "Gui\GuiElements\GuiClickableTextBox.h"
#include "Utility\Keys.h"
#include "Graphics\Renderer.h"

namespace SnackerEngine
{
	//--------------------------------------------------------------------------------------------------
	void GuiClickableTextBox::onTextBoxClicked()
	{
		observableTextBoxClicked.trigger();
	}
	//--------------------------------------------------------------------------------------------------
	GuiClickableTextBox::GuiClickableTextBox(const Vec2i& position, const Vec2i& size, const std::string& text)
		: GuiTextBox(position, size, text), observableTextBoxClicked{}
	{
	}
	//--------------------------------------------------------------------------------------------------
	GuiClickableTextBox::GuiClickableTextBox(const std::string& label)
		: GuiClickableTextBox(Vec2i(0, 0), Vec2i(0, 0), label)
	{
	}
	//--------------------------------------------------------------------------------------------------
	GuiClickableTextBox::GuiClickableTextBox(const nlohmann::json& json, const nlohmann::json* data, std::set<std::string>* parameterNames)
		: GuiTextBox(json, data, parameterNames), observableTextBoxClicked{}
	{
	}
	//--------------------------------------------------------------------------------------------------
	GuiClickableTextBox::~GuiClickableTextBox()
	{
	}
	//--------------------------------------------------------------------------------------------------
	GuiClickableTextBox::GuiClickableTextBox(const GuiClickableTextBox& other) noexcept
		: GuiTextBox(other), observableTextBoxClicked{}
	{
	}
	//--------------------------------------------------------------------------------------------------
	GuiClickableTextBox& GuiClickableTextBox::operator=(const GuiClickableTextBox& other) noexcept
	{
		GuiTextBox::operator=(std::move(other));
		return *this;
	}
	//--------------------------------------------------------------------------------------------------
	GuiClickableTextBox::GuiClickableTextBox(GuiClickableTextBox&& other) noexcept
		: GuiTextBox(std::move(other)), observableTextBoxClicked{std::move(other.observableTextBoxClicked)}
	{
	}
	//--------------------------------------------------------------------------------------------------
	void GuiClickableTextBox::onRegister()
	{
		GuiTextBox::onRegister();
		signUpEvent(SnackerEngine::GuiElement::CallbackType::MOUSE_BUTTON_ON_ELEMENT);
		signUpEvent(SnackerEngine::GuiElement::CallbackType::MOUSE_ENTER);
		signUpEvent(SnackerEngine::GuiElement::CallbackType::MOUSE_LEAVE);
	}
	//--------------------------------------------------------------------------------------------------
	void GuiClickableTextBox::subscribeToEventTextBoxClicked(EventHandle& eventHandle)
	{
		observableTextBoxClicked.subscribe(eventHandle);
	}
	//--------------------------------------------------------------------------------------------------
	GuiElement::IsCollidingResult GuiClickableTextBox::isColliding(const Vec2i& offset) const
	{
		return (offset.x > 0 && offset.x < getWidth()
			&& offset.y > 0 && offset.y < getHeight()) ?
			IsCollidingResult::COLLIDE_IF_CHILD_DOES_NOT : IsCollidingResult::NOT_COLLIDING;
	}
	//--------------------------------------------------------------------------------------------------
	void GuiClickableTextBox::callbackMouseButtonOnElement(const int& button, const int& action, const int& mods)
	{
		if (button == MOUSE_BUTTON_LEFT && action == ACTION_PRESS)
		{
			onTextBoxClicked();
		}
	}
	//--------------------------------------------------------------------------------------------------
	void GuiClickableTextBox::callbackMouseEnter(const Vec2d& position)
	{
		Renderer::setCursorShape(Renderer::CursorShape::HAND);
	}
	//--------------------------------------------------------------------------------------------------
	void GuiClickableTextBox::callbackMouseLeave(const Vec2d& position)
	{
		Renderer::setCursorShape(Renderer::CursorShape::DEFAULT);
	}
	//--------------------------------------------------------------------------------------------------
}