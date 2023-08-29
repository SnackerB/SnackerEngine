#pragma once

#include "Gui\GuiElements\GuiButton.h"

namespace SnackerEngine
{

	class GuiCheckBox : public GuiButton
	{
	private:
		/// Wether the GuiCheckBox is currently checked or not
		bool checked = false;
		/// Variable Handle for bool
		GuiVariableHandle<bool>* boolHandle = nullptr;
		/// Button colors for when the bool is set to true
		Color4f colorDefaultTrue = Color4f(0.0f, 1.0f, 0.0f, 1.0f);
		Color4f colorHoverTrue = Color4f(0.0f, 0.8f, 0.0f, 1.0f);
		Color4f colorPressedTrue = Color4f(0.0f, 0.6f, 0.0f, 1.0f);
		Color4f colorHoverPressedTrue = Color4f(0.0f, 0.4f, 0.0f, 1.0f);
		/// Button colors for when the bool is set to false
		Color4f colorDefaultFalse = Color4f(1.0f, 0.0f, 0.0f, 1.0f);
		Color4f colorHoverFalse = Color4f(0.8f, 0.0f, 0.0f, 1.0f);
		Color4f colorPressedFalse = Color4f(0.6f, 0.0f, 0.0f, 1.0f);
		Color4f colorHoverPressedFalse = Color4f(0.4f, 0.0f, 0.0f, 1.0f);
	protected:
		/// Helper function that is called when the button is pressed.
		/// Overwritten from class GuiButton
		void onButtonPress() override;
		/// Helper function that sets the button color to the correct color,
		/// based on the value of the variable "checked"
		void updateButtonColor();
	public:
		/// name of this GuiElementType for JSON parsing
		static constexpr std::string_view typeName = "GUI_CHECK_BOX";
		/// Default constructor
		GuiCheckBox(int size = 30); // NOTE: All parameters must have default values!
		/// Constructor from JSON
		GuiCheckBox(const nlohmann::json& json, const nlohmann::json* data = nullptr, std::set<std::string>* parameterNames = nullptr);
		/// Destructor
		virtual ~GuiCheckBox() {};
		/// Copy constructor and assignment operator
		GuiCheckBox(const GuiCheckBox& other) noexcept;
		GuiCheckBox& operator=(const GuiCheckBox& other) noexcept;
		/// Move constructor and assignment operator
		GuiCheckBox(GuiCheckBox&& other) noexcept;
		GuiCheckBox& operator=(GuiCheckBox&& other) noexcept;
		/// Getters
		bool isChecked() const { return checked; }
		const Color4f& getColorDefaultTrue() const { return colorDefaultTrue; }
		const Color4f& getColorHoverTrue() const { return colorHoverTrue; }
		const Color4f& getColorPressedTrue() const { return colorPressedTrue; }
		const Color4f& getColorHoverPressedTrue() const { return colorHoverPressedTrue; }
		const Color4f& getColorDefaultFalse() const { return colorDefaultFalse; }
		const Color4f& getColorHoverFalse() const { return colorHoverFalse; }
		const Color4f& getColorPressedFalse() const { return colorPressedFalse; }
		const Color4f& getColorHoverPressedFalse() const { return colorHoverPressedFalse; }
		/// Setters
		void setChecked(bool checked);
		void setColorDefaultTrue(const Color4f& colorDefaultTrue) { this->colorDefaultTrue = colorDefaultTrue; updateButtonColor(); }
		void setColorHoverTrue(const Color4f& colorHoverTrue) { this->colorHoverTrue = colorHoverTrue; updateButtonColor(); }
		void setColorPressedTrue(const Color4f& colorPressedTrue) { this->colorPressedTrue = colorPressedTrue; updateButtonColor(); }
		void setColorHoverPressedTrue(const Color4f& colorHoverPressedTrue) { this->colorHoverPressedTrue = colorHoverPressedTrue; updateButtonColor(); }
		void setColorDefaultFalse(const Color4f& colorDefaultFalse) { this->colorDefaultFalse = colorDefaultFalse; updateButtonColor(); }
		void setColorHoverFalse(const Color4f& colorHoverFalse) { this->colorHoverFalse = colorHoverFalse; updateButtonColor(); }
		void setColorPressedFalse(const Color4f& colorPressedFalse) { this->colorPressedFalse = colorPressedFalse; updateButtonColor(); }
		void setColorHoverPressedFalse(const Color4f& colorHoverPressedFalse) { this->colorHoverPressedFalse = colorHoverPressedFalse; updateButtonColor(); }
	protected:
		/// Draws this GuiElement object relative to its parent element. Will also recursively
		/// draw all children of this element.
		/// worldPosition:		position of the upper left corner of the guiElement in world space
		virtual void draw(const Vec2i& worldPosition) override;

		//==============================================================================================
		// GuiHandles
		//==============================================================================================

		/// Overwrite this function if the guiElement owns handles. This function should update the
		/// handle pointer when the handle is moved. Called by the handle after it is moved.
		virtual void onHandleMove(GuiHandle& guiHandle);
		/// This function is called by a handle right before the handle is destroyed
		virtual void onHandleDestruction(GuiHandle& guiHandle);
		/// This function can be called by a handle if something occurs/changes with the handle
		/// example: value of a variable handle changes!
		virtual void onHandleUpdate(GuiHandle& guiHandle);
	public:
		/// Sets the event handle. Cannot be done if an event handle is already set, 
		/// delete the previous event handle first! Returns true on success
		bool setBoolHandle(GuiVariableHandle<bool>& boolHandle);
	};
}