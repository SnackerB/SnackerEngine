#pragma once

#include "Gui\GuiElement.h"
#include "Gui\GuiElements\GuiScaleButton.h"
#include "Gui\Layouts\VerticalListLayout.h"
#include "Utility\Handles\VariableHandle.h"

namespace SnackerEngine
{

	class GuiSelectionBox : public GuiScaleButton
	{
	public:
		/// Static default Attributes
		/// Color of the option buttons
		static Color4f defaultOptionButtonColor;
		static Color4f defaultOptionButtonPressedColor;
		static Color4f defaultOptionButtonHoverColor;
		static Color4f defaultOptionButtonPressedHoverColor;
		/// defaultColor of the selected option button
		static Color4f defaultSelectedOptionButtonColor;
		static Color4f defaultSelectedOptionButtonPressedColor;
		static Color4f defaultSelectedOptionButtonHoverColor;
		static Color4f defaultSelectedOptionButtonPressedHoverColor;
		/// Text Color
		static Color4f defaultTextColor;
		/// Divider color
		static Color4f defaultDividerColor;
		/// Dropdown symbol
		static Color4f defaultDropDownSymbolColorClosed;
		static Color4f defaultDropDownSymbolColorOpen;
		static Texture defaultDropdownSymbolTextureClosed;
		static Texture defaultDropdownSymbolTextureOpen;
		static Shader defaultDropdownSymbolShader;
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
		/// Index of the currently selected option
		GuiSelectionBoxSelectedOptionIndexVariableHandle selectedOptionIndex{ *this };
		/// List element displaying buttons for all options. This element gets created/deleted
		/// when the arrow button is pressed
		std::unique_ptr<GuiVerticalListLayout> optionListLayout{};
		/// Vector of eventHandles for the option buttons
		std::vector<GuiSelectionBoxEventHandle> optionButtonEventHandles{};
		/// Event handle that gets triggered when the selectedOptionsButton is pressed
		GuiSelectionBoxEventHandle selectedOptionButtonEventHandle{ *this, -1 };
		/// Minimum size of the longest option
		int minimumWidthOfLongestOption = 0;
		/// Rounded corners
		float roundedCorners = 0.0f;
		/// Button scales
		float selectedOptionHoverButtonScale = 1.0f;
		float selectedOptionPressedButtonScale = 1.0f;
		float selectedOptionPressedHoverButtonScale = 1.0f;
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
		/// Text colors
		Color4f optionButtonTextColor = defaultTextColor;
		Color4f selectedOptionButtonTextColor = defaultTextColor;
		/// Parameters for the divider between the currently selected item and other items
		int dividerThickness = 0;
		float dividerRoundedCorners = 0.0f;
		float dividerWidthPercentage = 0.8f;
		Color4f dividerColor = defaultDividerColor;
		/// Variables for the dropdown symbol
		Color4f dropDownSymbolColorClosed = defaultDropDownSymbolColorClosed;
		Color4f dropDownSymbolColorOpen = defaultDropDownSymbolColorOpen;
		Texture dropDownSymbolTextureOpen = defaultDropdownSymbolTextureOpen;
		Texture dropDownSymbolTextureClosed = defaultDropdownSymbolTextureClosed;
		Shader dropDownSymbolShader = defaultDropdownSymbolShader;
		bool drawDropDownSymbol = true;
		Vec2i dropDownSymbolSize = Vec2i(-2, -2); // If the size of the dropDownSymbol is set to (-2, -2), it is set according to the height of the selection box.
		/// Helper function that gets called by eventHandles to notify this element that
		/// one of its buttons was pressed.
		void notifyEvent(int eventID);
		/// Helper functions that computes size hints
		virtual void computeWidthHints() override;
		virtual void computeHeightHints() override;
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
		Vec4f getRoundedCorners() const override { return Vec4f(roundedCorners); };
		float getHoverButtonScale() const override { return selectedOptionHoverButtonScale; }
		float getPressedButtonScale() const override { return selectedOptionPressedButtonScale; }
		float getPressedHoverButtonScale() const override { return selectedOptionPressedHoverButtonScale; }
		const Color4f& getDropDownSymbolColorOpen() const { return dropDownSymbolColorOpen; }
		const Color4f& getDropDownSymbolColorClosed() const { return dropDownSymbolColorClosed; }
		const Texture& getDropDownSymbolTextureOpen() const { return dropDownSymbolTextureOpen; }
		const Texture& getDropDownSymbolTextureClosed() const { return dropDownSymbolTextureClosed; }
		const Shader& getDropDownSymbolShader() const { return dropDownSymbolShader; }
		bool isDrawDropDownSymbol() const { return drawDropDownSymbol; }
		const Vec2i& getDropDownSymbolSize() const { return dropDownSymbolSize; }
		/// Setters
		void setSelectedOptionIndex(std::size_t selectedOptionIndex);
		void setOptions(const std::vector<std::string>& options);
		void setRoundedCorners(float roundedCorners) override;
		void setHoverButtonScale(float hoverButtonScale) override;
		void setPressedButtonScale(float pressedButtonScale) override;
		void setPressedHoverButtonScale(float pressedHoverButtonScale) override;
		void setDropDownSymbolColorOpen(const Color4f& dropDownSymbolColorOpen) { this->dropDownSymbolColorOpen = dropDownSymbolColorOpen; }
		void setDropDownSymbolColorClosed(const Color4f& dropDownSymbolColorClosed) { this->dropDownSymbolColorClosed = dropDownSymbolColorClosed; }
		void setDropDownSymbolTextureOpen(const Texture& dropDownSymbolTextureOpen) { this->dropDownSymbolTextureOpen = dropDownSymbolTextureOpen; }
		void setDropDownSymbolTextureClosed(const Texture& dropDownSymbolTextureClosed) { this->dropDownSymbolTextureClosed = dropDownSymbolTextureClosed; }
		void setDropDownSymbolShader(const Shader& dropDownSymbolShader) { this->dropDownSymbolShader = dropDownSymbolShader; }
		void setDrawDropDownSymbol(bool drawDropDownSymbol);
		void getDropDownSymbolSize(const Vec2i& dropDownSymbolSize);
	};

}