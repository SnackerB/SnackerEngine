#include "Gui/GuiEventHandles/GuiHandle.h"
#include "Gui/GuiElement.h"

namespace SnackerEngine
{
    //--------------------------------------------------------------------------------------------------
    void GuiHandle::registerHandle(const GuiHandleID& guiHandleID, GuiElement& guiElement)
    {
        this->guiHandleID = guiHandleID;
        this->guiElement = &guiElement;
    }
    //--------------------------------------------------------------------------------------------------
    void GuiHandle::signOff()
    {
        guiHandleID = 0;
        guiElement = nullptr;
    }
    //--------------------------------------------------------------------------------------------------
    void GuiHandle::onMove(GuiElement& guiElement)
    {
        this->guiElement = &guiElement;
    }
    //--------------------------------------------------------------------------------------------------
    GuiHandle::GuiHandle()
        : guiHandleID(0), guiElement(nullptr) {}
    //--------------------------------------------------------------------------------------------------
    const GuiHandle::GuiHandleID& GuiHandle::getGuiHandleID() const
    {
        return guiHandleID;
    }
    //--------------------------------------------------------------------------------------------------
    GuiHandle::GuiHandle(GuiHandle&& other) noexcept
        : guiHandleID(other.guiHandleID), guiElement(other.guiElement)
    {
        other.signOff();
        guiElement->onHandleMove(*this);
    }
    //--------------------------------------------------------------------------------------------------
    GuiHandle& GuiHandle::operator=(GuiHandle&& other) noexcept
    {
        guiHandleID = other.guiHandleID;
        guiElement = other.guiElement;
        other.signOff();
        guiElement->onHandleMove(*this);
        return *this;
    }
    //--------------------------------------------------------------------------------------------------
    GuiHandle::~GuiHandle()
    {
        if (guiElement)
            guiElement->onHandleDestruction(*this);
    }
    //--------------------------------------------------------------------------------------------------
}