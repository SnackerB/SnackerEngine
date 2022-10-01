#include "GuiButton.h"
#include "Core/Keys.h"

namespace SnackerEngine
{
	void GuiButton::onHandleDestruction(GuiHandle& guiHandle)
	{
		eventHandle = nullptr;
	}

	void GuiButton::onHandleMove(GuiHandle& guiHandle)
	{
		// Update pointer
		eventHandle = static_cast<GuiEventHandle*>(&guiHandle);
	}

	void GuiButton::onRegister()
	{
		signUpEvent(SnackerEngine::GuiElement::CallbackType::MOUSE_BUTTON_ON_ELEMENT);
		signUpEvent(SnackerEngine::GuiElement::CallbackType::MOUSE_BUTTON_ON_ELEMENT);
	}

	void GuiButton::callbackMouseButtonOnElement(const int& button, const int& action, const int& mods)
	{
		if (button == MOUSE_BUTTON_LEFT && action == ACTION_PRESS)
		{
			if (eventHandle)
				activate(*eventHandle);
		}
	}

	GuiButton::GuiButton(const Vec2i& position, const Vec2i& size, const Color3f& backgroundColor)
		: GuiPanel(position, size, backgroundColor), eventHandle(nullptr) {}

	GuiButton::GuiButton(GuiEventHandle& eventHandle, const Vec2i& position, const Vec2i& size, const Color3f& backgroundColor)
		: GuiButton(position, size, backgroundColor)
	{
		setEventHandle(eventHandle);
	}

	void GuiButton::setEventHandle(GuiEventHandle& eventHandle)
	{
		this->eventHandle = &eventHandle;
	}

	GuiButton::GuiButton(GuiButton& other) noexcept
		: GuiPanel(other), eventHandle(nullptr) {}

	GuiButton& GuiButton::operator=(GuiButton& other) noexcept
	{
		GuiPanel::operator=(other);
		eventHandle = nullptr;
		return *this;
	}

	GuiButton::GuiButton(GuiButton&& other) noexcept
		: GuiPanel(other), eventHandle(other.eventHandle)
	{
		other.eventHandle = nullptr;
	}

	GuiButton& GuiButton::operator=(GuiButton&& other) noexcept
	{
		GuiPanel::operator=(other);
		eventHandle = other.eventHandle;
		other.eventHandle = nullptr;
		return *this;
	}

	GuiButton::~GuiButton()
	{
		if (eventHandle)
			signOffHandle(*eventHandle);
	}

}
