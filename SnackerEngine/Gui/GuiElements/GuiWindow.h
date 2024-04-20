#pragma once

#include "Gui/GuiElements/GuiPanel.h"
#include "Gui/GuiElements/GuiTextBox.h"

namespace SnackerEngine
{
	//--------------------------------------------------------------------------------------------------
	class GuiWindow : public GuiPanel
	{
	public:
		/// Static default Attributes
		static int defaultResizeButtonSize;
		static Color4f defaultResizeButtonColor;
		static Color4f defaultTopBarBackgroundColor;
		static Color4f defaultBackgroundColor;
	private:
		/// The size of the resize button in pixels
		int resizeButtonSize = defaultResizeButtonSize;
		/// The top bar
		GuiTextBox topBar{};
		/// The color of the resize button
		Color4f resizeButtonColor = defaultResizeButtonColor;
		/// The model matrix of the resize button
		Mat4f resizeButtonModelMatrix{};
		/// if this is set to true, the window is currently being moved
		bool isMoving = false;
		/// If this is set to true, the window is currently being resized
		bool isResizing = false;
		/// Original mouse position when the moving/resizing started
		Vec2f mouseOffset{};
		/// Computes the model matrices
		void computeResizeButtonModelMatrix();
		/// Helper function that returns true if the given position vector 
		/// (relative to the top left corner of this element) collides with the resize button, 
		/// given that a collision with this window element was already detected
		bool isCollidingWithResizeButton(const Vec2i& offset) const;
	public:
		/// name of this GuiElementType for JSON parsing
		static constexpr std::string_view typeName = "GUI_WINDOW";
		virtual std::string_view getTypeName() const override { return typeName; }
		/// Default constructor
		GuiWindow(const Vec2i& position = Vec2i(), const Vec2i& size = Vec2i(), const std::string& name = "");
		/// Constructor from JSON
		GuiWindow(const nlohmann::json& json, const nlohmann::json* data = nullptr, std::set<std::string>* parameterNames = nullptr);
		/// Destructor
		virtual ~GuiWindow() = default;
		/// Copy constructor and assignment operator
		GuiWindow(const GuiWindow& other) noexcept = default;
		GuiWindow& operator=(const GuiWindow& other) noexcept = default;
		/// Move constructor and assignment operator
		GuiWindow(GuiWindow&& other) noexcept = default;
		GuiWindow& operator=(GuiWindow&& other) noexcept = default;
		/// Getters
		int getResizeButtonSize() const { return resizeButtonSize; }
		int getTopBarHeight() const { return topBar.getHeight(); }
		double getFontSize() const { return topBar.getFontSize(); }
		const Font& getFont() const { return topBar.getFont(); }
		const std::string& getName() const { return topBar.getText(); }
		const Color4f& getTopBarColor() const { return topBar.getBackgroundColor(); }
		const Color4f& getTopBarTextColor() const { return topBar.getTextColor(); }
		const Color4f& getResizeButtonColor() const { return resizeButtonColor; }
		/// Setters
		void setResizeButtonSize(int resizeButtonSize);
		void setTopBarHeight(int topBarHeight) { topBar.setHeight(topBarHeight); }
		void setFontSize(double fontSize) { topBar.setFontSize(fontSize); }
		void setFont(const Font& font) { topBar.setFont(font); }
		void setName(const std::string& name) { topBar.setText(name); }
		void setTopBarColor(const Color4f& color) { topBar.setBackgroundColor(color); }
		void setTopBarTextColor(const Color4f& color) { topBar.setTextColor(color); }
		void setResizeButtonColor(const Color4f& color) { this->resizeButtonColor = resizeButtonColor; }

		//==============================================================================================
		// Animatables
		//==============================================================================================

		std::unique_ptr<GuiElementAnimatable> animateResizeButtonSize(const int& startVal, const int& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear);
		std::unique_ptr<GuiElementAnimatable> animateTopBarHeight(const int& startVal, const int& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear);
		std::unique_ptr<GuiElementAnimatable> animateFontSize(const double& startVal, const double& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear);
		std::unique_ptr<GuiElementAnimatable> animateTopBarColor(const Color4f& startVal, const Color4f& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear);
		std::unique_ptr<GuiElementAnimatable> animateTopBarTextColor(const Color4f& startVal, const Color4f& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear);
		std::unique_ptr<GuiElementAnimatable> animateResizeButtonColor(const Color4f& startVal, const Color4f& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear);

	protected:
		/// Draws this GuiElement object relative to its parent element. Will also recursively
		/// draw all children of this element.
		/// worldPosition:		position of the upper left corner of the guiElement in world space
		virtual void draw(const Vec2i& worldPosition) override;
		/// This function is called by the guiManager after registering this GuiElement object.
		/// When this function is called, the guiManager pointer was already set.
		/// This function can e.g. be used for registering callbacks at the guiManager
		virtual void onRegister() override;
		/// This function is called when the size changes. Can eg. be. be used to
		/// compute model matrices. Not called by the constructor. Do not enforce layouts
		/// in this function!
		virtual void onSizeChange() override;
		/// Sets the position and size of the children of this element according to
		/// layout rules, if there are any. May register children and/or parent for
		/// enforcing layouts as well
		virtual void enforceLayout() override;

		//==============================================================================================
		// Collisions
		//==============================================================================================

	public:
		/// Returns how the given offset vector (relative to the top left corner of the guiElement)
		/// collides with this element
		virtual IsCollidingResult isColliding(const Vec2i& offset) const override;

		//==============================================================================================
		// Events
		//==============================================================================================

	protected:
		/// Callback function for mouse button input. Parameters the same as in Scene.h
		virtual void callbackMouseButton(const int& button, const int& action, const int& mods) override;
		/// Callback function for mouse motion. Parameter the same as in Scene.h
		/// position:	position relative to this elements top left corner
		virtual void callbackMouseMotion(const Vec2d& position) override;
		/// Callback function for mouse button input on this GuiElement object. Parameters the same as in Scene.h
		virtual void callbackMouseButtonOnElement(const int& button, const int& action, const int& mods) override;
	};
	//--------------------------------------------------------------------------------------------------
}