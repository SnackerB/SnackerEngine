#pragma once

#include "Gui\GuiElements\GuiButton.h"
#include "Utility\Handles\VariableHandle.h"

namespace SnackerEngine
{
	//--------------------------------------------------------------------------------------------------
	class GuiCheckBox : public GuiButton
	{
	public:
		/// Static default Attributes
		static int defaultCheckBoxSize;
		static Color4f defaultColorDefaultTrue;
		static Color4f defaultColorHoverTrue;
		static Color4f defaultColorPressedTrue;
		static Color4f defaultColorHoverPressedTrue;
		static Color4f defaultColorDefaultFalse;
		static Color4f defaultColorHoverFalse;
		static Color4f defaultColorPressedFalse;
		static Color4f defaultColorHoverPressedFalse;
		static Color4f defaultCheckMarkColor;
		static Texture defaultCheckMarkTexture;
		static Shader defaultCheckMarkShader;
		static bool defaultDrawCheckMark;
	private:
		class GuiCheckBoxVariableHandle : public VariableHandle<bool>
		{
		private:
			friend class GuiCheckBox;
			GuiCheckBox* parentElement;
		protected:
			void onEvent() override
			{
				parentElement->updateButtonColor();
			}
		public:
			GuiCheckBoxVariableHandle(GuiCheckBox* parentElement)
				: VariableHandle<bool>(), parentElement(parentElement) {}
		};
		/// VariableHandle to the boolean that decides wether the GuiCheckBox 
		// is currently checked or not
		GuiCheckBoxVariableHandle checked{ this };
		/// Button colors for when the bool is set to true
		Color4f colorDefaultTrue = defaultColorDefaultTrue;
		Color4f colorHoverTrue = defaultColorHoverTrue;
		Color4f colorPressedTrue = defaultColorPressedTrue;
		Color4f colorHoverPressedTrue = defaultColorHoverPressedTrue;
		/// Button colors for when the bool is set to false
		Color4f colorDefaultFalse = defaultColorDefaultFalse;
		Color4f colorHoverFalse = defaultColorHoverFalse;
		Color4f colorPressedFalse = defaultColorPressedFalse;
		Color4f colorHoverPressedFalse = defaultColorHoverPressedFalse;
		/// Checkmark color
		Color4f checkMarkColor = defaultCheckMarkColor;
		/// Checkmark texture
		Texture checkMarkTexture = defaultCheckMarkTexture;
		/// Shader for srawing the ckeck mark
		Shader checkMarkShader = defaultCheckMarkShader;
		/// If this is set to true, a checkmark will be drawn if the ckeckbox is checked
		bool drawCheckMark = defaultDrawCheckMark;
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
		virtual std::string_view getTypeName() const override { return typeName; }
		/// Default constructor
		GuiCheckBox(int size = defaultCheckBoxSize);
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
		const Color4f& getCheckMarkColor() const { return checkMarkColor; }
		const Texture& getCheckMarkTexture() const { return checkMarkTexture; }
		const Shader& getCheckMarkShader() const { return checkMarkShader; }
		const bool isDrawCheckMark() const { return drawCheckMark; }
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
		void setCheckMarkColor(const Color4f& checkMarkColor) { this->checkMarkColor = checkMarkColor; }
		void setCheckMarkTexture(const Texture& checkMarkTexture) { this->checkMarkTexture = checkMarkTexture; }
		void setCheckMarkShader(const Shader& checkMarkShader) { this->checkMarkShader = checkMarkShader; }
		void setDrawCheckMark(bool drawCheckMark) { this->drawCheckMark = drawCheckMark; }

		std::unique_ptr<GuiElementAnimatable> animateColorDefaultTrue(const Color4f& startVal, const Color4f& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear);
		std::unique_ptr<GuiElementAnimatable> animateColorHoverTrue(const Color4f& startVal, const Color4f& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear);
		std::unique_ptr<GuiElementAnimatable> animateColorPressedTrue(const Color4f& startVal, const Color4f& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear);
		std::unique_ptr<GuiElementAnimatable> animateColorHoverPressedTrue(const Color4f& startVal, const Color4f& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear);
		std::unique_ptr<GuiElementAnimatable> animateColorDefaultFalse(const Color4f& startVal, const Color4f& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear);
		std::unique_ptr<GuiElementAnimatable> animateColorHoverFalse(const Color4f& startVal, const Color4f& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear);
		std::unique_ptr<GuiElementAnimatable> animateColorPressedFalse(const Color4f& startVal, const Color4f& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear);
		std::unique_ptr<GuiElementAnimatable> animateColorHoverPressedFalse(const Color4f& startVal, const Color4f& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear);
		std::unique_ptr<GuiElementAnimatable> animateCheckMarkColor(const Color4f& startVal, const Color4f& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear);

	protected:
		/// Draws this GuiElement object relative to its parent element. Will also recursively
		/// draw all children of this element.
		/// worldPosition:		position of the upper left corner of the guiElement in world space
		virtual void draw(const Vec2i& worldPosition) override;

		//==============================================================================================
		// GuiHandles
		//==============================================================================================

	public:
		/// Returns a reference to the variable handle
		VariableHandle<bool>& getVariableHandle() { return checked; }
	};
	//--------------------------------------------------------------------------------------------------
}