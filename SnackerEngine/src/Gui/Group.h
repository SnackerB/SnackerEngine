#pragma once

#include "Gui\GuiElement.h"

namespace SnackerEngine
{
	//--------------------------------------------------------------------------------------------------
	using GuiGroupID = int;
	//--------------------------------------------------------------------------------------------------
	class GuiGroup
	{
	public:
		/// The group type, necessary for efficient casting
		virtual std::string getGroupType() const { return "BASIC_GROUP"; }
	private:
		friend class GuiManager;
		/// Pointer to the GuiManager
		GuiManager* guiManager = nullptr;
		/// The name of the group
		std::string name = "";
		/// The ID of the group
		GuiGroupID groupID = -1;
		/// A vector of GuiIDs of the elements belonging to this group
		std::vector<GuiElement::GuiID> elements{};
		/// Called by the guiManager when an element wants to join the group.
		/// Returns true on success and false if the element has already joined the group previously.
		bool join(GuiElement::GuiID element);
		/// Called by the guiManager when an element wants to leave the group
		void leave(GuiElement::GuiID element);
	protected:
		/// This function gets called when a new element is added to the group
		virtual void onElementJoin(GuiElement::GuiID element, std::size_t position) {};
		/// This function gets called when a new element leaves the group
		virtual void onElementLeave(GuiElement::GuiID element, std::size_t position) {};
		/// Getters
		GuiManager* getGuiManager() const { return guiManager; }
		const std::string& getName() const { return name; }
		GuiGroupID getGroupID() const { return groupID; }
		const std::vector<GuiElement::GuiID>& getElements() { return elements; }
	public:
		/// Constructor
		GuiGroup(const std::string& name = "") 
			: name(name) {}
	};
	//--------------------------------------------------------------------------------------------------
}