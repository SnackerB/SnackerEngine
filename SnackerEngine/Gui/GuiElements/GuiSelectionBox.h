#pragma once

#include "Gui\GuiElement.h"
#include "Gui\GuiElements\GuiButton.h"
#include "Gui\Layouts\VerticalListLayout.h"
#include "Utility\Handles\VariableHandle.h"

namespace SnackerEngine
{

	class GuiSelectionBox : public GuiElement
	{
	public:
		/// Static default Attributes
		/// Color of the option buttons
		static Color4f defaultOptionButtonColor;
		static Color4f defaultOptionButtonPressedColor;
		static Color4f defaultOptionButtonHoverColor;
		static Color4f defaultOptionButtonPressedHoverColor;
		static /// defaultColor of the selected option button
		Color4f defaultSelectedOptionButtonColor;
		static Color4f defaultSelectedOptionButtonPressedColor;
		static Color4f defaultSelectedOptionButtonHoverColor;
		static Color4f defaultSelectedOptionButtonPressedHoverColor;
		/// defaultColor of the arrow button when the selection menu is closed
		static Color4f defaultArrowButtonMenuClosedColor;
		static Color4f defaultArrowButtonMenuClosedPressedColor;
		static Color4f defaultArrowButtonMenuClosedHoverColor;
		static Color4f defaultArrowButtonMenuClosedPressedHoverColor;
		/// defaultColor of the arrow button when the selection menu is open
		static Color4f defaultArrowButtonMenuOpenColor;
		static Color4f defaultArrowButtonMenuOpenPressedColor;
		static Color4f defaultArrowButtonMenuOpenHoverColor;
		static Color4f defaultArrowButtonMenuOpenPressedHoverColor;
		/// Text Color
		static Color4f defaultTextColor;
	private:
		/// EventHandle that notifies this selectionBox that an options button or the arrow button was pressed
		class GuiSelectionBoxEventHandle : public EventHandle
		{
		private:
			int eventID;
			GuiSelectionBox* guiSelectionBox;
			friend class GuiSelectionBox;
		protected:
			/// Function that is called when the event happens. Can be overwritten
			virtual void onEvent() override
			{
				if (guiSelectionBox) guiSelectionBox->notifyEvent(eventID);
			}
		public:
			/// Constructor
			GuiSelectionBoxEventHandle(GuiSelectionBox& guiSelectionBox, int eventID)
				: EventHandle(), guiSelectionBox(&guiSelectionBox), eventID(eventID) {}
		};
		/// Special child variable Handle for the selected option index
		class GuiSelectionBoxSelectedOptionIndexVariableHandle : public VariableHandle<std::size_t>
		{
		private:
			friend class GuiSelectionBox;
			GuiSelectionBox* guiSelectionBox; 
		protected:
				/// Function that is called when the event happens. Can be overwritten
				virtual void onEvent() override
				{
					if (guiSelectionBox) guiSelectionBox->updateSelection();
				}
		public:
			/// Constructor
			GuiSelectionBoxSelectedOptionIndexVariableHandle(GuiSelectionBox& guiSelectionBox)
				: VariableHandle<std::size_t>(), guiSelectionBox(&guiSelectionBox) {}
		};
		/// All possible options
		std::vector<std::string> options{};
		/// The currently selected option
		GuiButton selectedOption{};
		/// The arrow button
		GuiButton arrowButton{};
		/// Index of the currently selected option
		GuiSelectionBoxSelectedOptionIndexVariableHandle selectedOptionIndex{ *this };
		/// List element displaying buttons for all options. This element gets created/deleted
		/// when the arrow button is pressed
		std::unique_ptr<GuiVerticalListLayout> optionListLayout{};
		/// Vector of eventHandles for the option buttons
		std::vector<GuiSelectionBoxEventHandle> optionButtonEventHandles{};
		/// eventHandle for the arrow button
		GuiSelectionBoxEventHandle arrowButtonEventHandle{ *this, -1 };
		/// Minimum size of the longest option
		int minimumWidthOfLongestOption = 0;
		/// Color of the option buttons
		Color4f optionButtonColor = defaultOptionButtonColor;
		Color4f optionButtonPressedColor = defaultOptionButtonPressedColor;
		Color4f optionButtonHoverColor = defaultOptionButtonHoverColor;
		Color4f optionButtonPressedHoverColor = defaultOptionButtonPressedHoverColor;
		/// Color of the selected option button
		Color4f selectedOptionButtonColor = defaultSelectedOptionButtonColor;
		Color4f selectedOptionButtonPressedColor = defaultSelectedOptionButtonPressedColor;
		Color4f selectedOptionButtonHoverColor = defaultSelectedOptionButtonHoverColor;
		Color4f selectedOptionButtonPressedHoverColor = defaultSelectedOptionButtonPressedHoverColor;
		/// Color of the arrow button when the selection menu is closed
		Color4f arrowButtonMenuClosedColor = defaultArrowButtonMenuClosedColor;
		Color4f arrowButtonMenuClosedPressedColor = defaultArrowButtonMenuClosedPressedColor;
		Color4f arrowButtonMenuClosedHoverColor = defaultArrowButtonMenuClosedHoverColor;
		Color4f arrowButtonMenuClosedPressedHoverColor = defaultArrowButtonMenuClosedPressedHoverColor;
		/// Color of the arrow button when the selection menu is open
		Color4f arrowButtonMenuOpenColor = defaultArrowButtonMenuOpenColor;
		Color4f arrowButtonMenuOpenPressedColor = defaultArrowButtonMenuOpenPressedColor;
		Color4f arrowButtonMenuOpenHoverColor = defaultArrowButtonMenuOpenHoverColor;
		Color4f arrowButtonMenuOpenPressedHoverColor = defaultArrowButtonMenuOpenPressedHoverColor;
		/// Text colors
		Color4f optionButtonTextColor = defaultTextColor;
		Color4f selectedOptionButtonTextColor = defaultTextColor;
		Color4f arrowButtonMenuClosedTextColor = defaultTextColor;
		Color4f arrowButtonMenuOpenTextColor = defaultTextColor;
		/// border between the box and the text in pixels
		int border = GuiElement::defaultBorderSmall;
		/// The normal font size of the text (without any rescaling)
		double fontSize = GuiElement::defaultFontSizeNormal;
		/// The font of the text
		Font font = GuiElement::defaultFont;
		/// SizeHintModes of the text
		GuiTextBox::SizeHintModes sizeHintModes{ GuiTextBox::SizeHintMode::SET_TO_TEXT_SIZE, GuiTextBox::SizeHintMode::ARBITRARY, GuiTextBox::SizeHintMode::SET_TO_TEXT_SIZE };
		/// Helper function that gets called by eventHandles to notify this element that
		/// one of its buttons was pressed.
		void notifyEvent(int eventID);
		/// Helper function to compute the size hints
		void computeSizeHints();
		/// Helper function that opens the selectionBox
		void openSelectionBox();
		/// Helper function that closes the selectionBox
		void closeSelectionBox();
	protected:
		/// Draws this GuiElement object relative to its parent element. Will also recursively
		/// draw all children of this element.
		/// worldPosition:		position of the upper left corner of the guiElement in world space
		virtual void draw(const Vec2i& worldPosition) override;
		/// Callback function for mouse button input. Parameters the same as in Scene.h
		virtual void callbackMouseButton(const int& button, const int& action, const int& mods) override;
		/// This function is called by the guiManager after registering this GuiElement object.
		/// When this function is called, the guiManager pointer was already set.
		/// This function can e.g. be used for registering callbacks at the guiManager
		virtual void onRegister() override;
		/// This function is called when the size changes. Can eg. be. be used to
		/// compute model matrices. Not called by the constructor. Do not enforce layouts
		/// in this function!
		virtual void onSizeChange() override;
		/// Returns the first colliding child which collides with the given offset vector. The offset
		/// vector is relative to the top left corner of the guiElement. If zero is returned, this means that
		/// none of this elements children is colliding. This function will call isColliding() on its children
		/// recursively.
		virtual GuiID getCollidingChild(const Vec2i& offset) const override;
		/// Helper function that updates the selection. This function is called by the variable Handle
		/// for the selected option index
		void updateSelection();
	public:
		/// name of this GuiElementType for JSON parsing
		static constexpr std::string_view typeName = "GUI_SELECTION_BOX";
		virtual std::string_view getTypeName() const override { return typeName; }
		/// Default constructor
		GuiSelectionBox(const std::vector<std::string>& options = {});
		/// Constructor from JSON
		GuiSelectionBox(const nlohmann::json& json, const nlohmann::json* data = nullptr, std::set<std::string>* parameterNames = nullptr);
		/// Destructor
		virtual ~GuiSelectionBox();
		/// Copy constructor and assignment operator
		GuiSelectionBox(const GuiSelectionBox& other) noexcept;
		GuiSelectionBox& operator=(const GuiSelectionBox& other) noexcept;
		/// Move constructor and assignment operator
		GuiSelectionBox(GuiSelectionBox&& other) noexcept;
		GuiSelectionBox& operator=(GuiSelectionBox&& other) noexcept;
		/// Getters
		std::size_t getSelectedOptionIndex() const { return selectedOptionIndex; }
		VariableHandle<std::size_t>& getSelectedOptionIndexVariableHandle() { return selectedOptionIndex; }
		void addOption(const std::string& option);
		void setOptions(const std::vector<std::string>& options);
		/// Setters
		void setSelectedOptionIndex(std::size_t selectedOptionIndex);
	};

}