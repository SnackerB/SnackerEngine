#include "Gui/GuiEventHandles/GuiHandle2.h"
#include "Gui/GuiElement2.h"

namespace SnackerEngine
{
    //--------------------------------------------------------------------------------------------------
    void GuiHandle2::registerHandle(const GuiHandleID& guiHandleID, GuiElement2& guiElement)
    {
        this->guiHandleID = guiHandleID;
        this->guiElement = &guiElement;
    }
    //--------------------------------------------------------------------------------------------------
    void GuiHandle2::signOff()
    {
        guiHandleID = 0;
        guiElement = nullptr;
    }
    //--------------------------------------------------------------------------------------------------
    void GuiHandle2::onMove(GuiElement2& guiElement)
    {
        this->guiElement = &guiElement;
    }
    //--------------------------------------------------------------------------------------------------
    void GuiHandle2::onHandleUpdate()
    {
        if (guiElement) guiElement->onHandleUpdate(*this);
    }
    //--------------------------------------------------------------------------------------------------
    GuiHandle2::GuiHandle2()
        : guiHandleID(0), guiElement(nullptr) {}
    //--------------------------------------------------------------------------------------------------
    const GuiHandle2::GuiHandleID& GuiHandle2::getGuiHandleID() const
    {
        return guiHandleID;
    }
    //--------------------------------------------------------------------------------------------------
    GuiHandle2::GuiHandle2(GuiHandle2&& other) noexcept
        : guiHandleID(other.guiHandleID), guiElement(other.guiElement)
    {
        other.signOff();
        guiElement->onHandleMove(*this);
    }
    //--------------------------------------------------------------------------------------------------
    GuiHandle2& GuiHandle2::operator=(GuiHandle2&& other) noexcept
    {
        guiHandleID = other.guiHandleID;
        guiElement = other.guiElement;
        other.signOff();
        guiElement->onHandleMove(*this);
        return *this;
    }
    //--------------------------------------------------------------------------------------------------
    GuiHandle2::~GuiHandle2()
    {
        if (guiElement)
            guiElement->onHandleDestruction(*this);
    }
    //--------------------------------------------------------------------------------------------------
}