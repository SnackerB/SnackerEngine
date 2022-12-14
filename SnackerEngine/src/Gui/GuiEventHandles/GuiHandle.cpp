#include "Gui/GuiEventHandles/GuiHandle.h"
#include "Gui/GuiElement.h"

namespace SnackerEngine
{
    //--------------------------------------------------------------------------------------------------
    void GuiHandle::registerHandle(const GuiHandleID& guiHandleID, GuiElement& guiElement)
    {
        this->guiHandleIDs.push_back(guiHandleID);
        this->guiElements.push_back(&guiElement);
    }
    //--------------------------------------------------------------------------------------------------
    void GuiHandle::signOff()
    {
        guiHandleIDs.clear();
        guiElements.clear();
    }
    //--------------------------------------------------------------------------------------------------
    void GuiHandle::onMove(GuiElement* oldElement, GuiElement* newElement)
    {
        auto result = std::find(guiElements.begin(), guiElements.end(), oldElement);
        if (result != guiElements.end()) {
            *result = newElement;
        }
    }
    //--------------------------------------------------------------------------------------------------
    void GuiHandle::onHandleUpdate()
    {
        for (auto& element : guiElements) {
            element->onHandleUpdate(*this);
        }
    }
    //--------------------------------------------------------------------------------------------------
    void GuiHandle::onHandleUpdateFromElement(GuiElement& element)
    {
        for (auto& it : guiElements) {
            if (it != &element) it->onHandleUpdate(*this);
        }
    }
    //--------------------------------------------------------------------------------------------------
    void GuiHandle::signOff(GuiElement& element)
    {
        auto result = std::find(guiElements.begin(), guiElements.end(), &element);
        if (result != guiElements.end()) {
            guiHandleIDs.erase(guiHandleIDs.begin() + (result - guiElements.begin()));
            guiElements.erase(result);
        }
    }
    //--------------------------------------------------------------------------------------------------
    GuiHandle::GuiHandle()
        : guiHandleIDs{}, guiElements{} {}
    //--------------------------------------------------------------------------------------------------
    const std::vector<GuiHandle::GuiHandleID>& GuiHandle::getGuiHandleIDs() const
    {
        return guiHandleIDs;
    }
    //--------------------------------------------------------------------------------------------------
    GuiHandle::GuiHandle(GuiHandle&& other) noexcept
        : guiHandleIDs(other.guiHandleIDs), guiElements(other.guiElements)
    {
        other.signOff();
        for (auto& element : guiElements) {
            element->onHandleMove(*this);
        }
    }
    //--------------------------------------------------------------------------------------------------
    GuiHandle& GuiHandle::operator=(GuiHandle&& other) noexcept
    {
        guiHandleIDs = other.guiHandleIDs;
        guiElements = other.guiElements;
        other.signOff();
        for (auto& element : guiElements) {
            element->onHandleMove(*this);
        }
        return *this;
    }
    //--------------------------------------------------------------------------------------------------
    GuiHandle::~GuiHandle()
    {
        for (auto& element : guiElements) {
            element->onHandleDestruction(*this);
        }
    }
    //--------------------------------------------------------------------------------------------------
}