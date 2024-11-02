#pragma once

#include "Gui/Layouts/VerticalListLayout.h"
#include "Graphics/Color.h"
#include "Gui/GuiElements/GuiButton.h"
#include "Gui/GuiElements/GuiPanel.h"

namespace SnackerEngine
{
	//--------------------------------------------------------------------------------------------------
	class GuiVerticalScrollingListLayout : public GuiVerticalListLayout
	{
	public:
		/// Static default Attributes
		static Color4f defaultBackgroundColor;
		static unsigned defaultScrollBarWidth;
		static Color4f defaultScrollbarColor;
		static Color4f defaultScrollbarBackgroundColor;
		static unsigned defaultScrollbarBorderRight;
		static unsigned defaultScrollbarBorderTop;
		static unsigned defaultScrollbarBorderBottom;
		static float defaultScrollSpeed;
	private:
		/// current total vertical size of the list, including offset at the beginning and end
		unsigned currentVerticalHeight = 0;
		/// current position in the list (offset of upper left corner of the list
		/// to the current upper corner of the guiElement. Used for scrolling through the list)
		int currentVerticalOffset = 0;
		/// Speed when scrolling
		float scrollSpeed = defaultScrollSpeed;
		/// Boolean that controls wether the scrollbar is drawn
		bool drawScrollBar = false;
		/// Percentage the visible scection holds of the total list
		float visiblePercentage = 1.0f;
		/// Percentage the offset holds of the total list
		float offsetPercentage = 0.0f;
		/// Colors for the scrollbar
		Color4f scrollbarBackgroundColor = defaultScrollbarBackgroundColor;
		Color4f scrollbarColor = defaultScrollbarColor;
		/// model matrix for the scrollbar and its background
		Mat4f modelMatrixScrollbar{};
		Mat4f modelMatrixScrollbarBackground{};
		/// The width of the scrollbar
		unsigned scrollbarWidth = defaultScrollBarWidth;
		/// Offset from the top, bottom and right border to the scroll bar
		unsigned scrollbarBorderRight = defaultScrollbarBorderRight;
		unsigned scrollbarBorderTop = defaultScrollbarBorderTop;
		unsigned scrollbarBorderBottom = defaultScrollbarBorderBottom;
		/// vertical offset from the top of the scroll bar to the mouse (used for scrolling through
		/// the list with the mouse)
		float mouseOffsetFromScrollBar = 0.0f;
		/// The first and last visible element in the children vector
		unsigned firstVisibleElement = 0;
		unsigned lastVisibleElement = 0;
		/// If this is set to true, the layouts minHeight and preferredHeight are set according
		/// to child elements
		bool shrinkHeightToChildren = false;
	public:
		/// name of this GuiElementType for JSON parsing
		static constexpr std::string_view typeName = "GUI_VERTICAL_SCROLLING_LIST_LAYOUT";
		virtual std::string_view getTypeName() const override { return typeName; }
		/// Default constructor
		GuiVerticalScrollingListLayout(Color4f backgroundColor = defaultBackgroundColor, Color4f scrollbarColor = defaultScrollbarColor, Color4f scrollbarBackgroundColor = defaultScrollbarBackgroundColor);
		/// Constructor from JSON.
		GuiVerticalScrollingListLayout(const nlohmann::json& json, const nlohmann::json* data, std::set<std::string>* parameterNames);
		/// Destructor
		virtual ~GuiVerticalScrollingListLayout() {};
		/// Copy constructor and assignment operator
		GuiVerticalScrollingListLayout(const GuiVerticalScrollingListLayout& other) noexcept;
		GuiVerticalScrollingListLayout& operator=(const GuiVerticalScrollingListLayout& other) noexcept;
		/// Move constructor and assignment operator
		GuiVerticalScrollingListLayout(GuiVerticalScrollingListLayout&& other) noexcept;
		GuiVerticalScrollingListLayout& operator=(GuiVerticalScrollingListLayout&& other) noexcept;
		/// Getters
		float getScrollSpeed() const { return scrollSpeed; }
		float getTotalOffsetPercentage() const { return offsetPercentage / (1.0f - visiblePercentage); }
		const Color4f& getScrollbarBackgroundColor() const { return scrollbarBackgroundColor; }
		const Color4f& getScrollbarColor() const { return scrollbarColor; }
		unsigned getScrollbarBorderRight() const { return scrollbarBorderRight; }
		unsigned getScrollbarBorderTop() const { return scrollbarBorderTop; }
		unsigned getScrollbarBorderBottom() const { return scrollbarBorderBottom; }
		bool isShrinkHeightToChildren() const { return shrinkHeightToChildren; }
		/// Setters
		void setScrollSpeed(float scrollSpeed) { this->scrollSpeed = scrollSpeed; };
		void setTotalOffsetPercentage(float totalOffsetPercentage);
		void setScrollbarBackgroundColor(const Color4f& scrollbarBackgroundColor) { this->scrollbarBackgroundColor = scrollbarBackgroundColor; }
		void setScrollbarColor(const Color4f& scrollbarColor) { this->scrollbarColor = scrollbarColor; }
		void setScrollbarBorderRight(unsigned scrollbarBorderRight);
		void setScrollbarBorderTop(unsigned scrollbarBorderTop);
		void setScrollbarBorderBottom(unsigned scrollbarBorderBottom);
		void setShrinkHeightToChildren(bool shrinkHeightToChildren);
		//==============================================================================================
		// Animatables
		//==============================================================================================
		std::unique_ptr<GuiElementAnimatable> animateScrollSpeed(const float& startVal, const float& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear);
		std::unique_ptr<GuiElementAnimatable> animateTotalOffsetPercentage(const float& startVal, const float& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear);
		std::unique_ptr<GuiElementAnimatable> animateScrollbarBackgroundColor(const Color4f& startVal, const Color4f& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear);
		std::unique_ptr<GuiElementAnimatable> animateScrollbarColor(const Color4f& startVal, const Color4f& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear);
		std::unique_ptr<GuiElementAnimatable> animateScrollbarBorderRight(const unsigned& startVal, const unsigned& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear);
		std::unique_ptr<GuiElementAnimatable> animateScrollbarBorderTop(const unsigned& startVal, const unsigned& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear);
		std::unique_ptr<GuiElementAnimatable> animateScrollbarBorderBottom(const unsigned& startVal, const unsigned& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear);
	protected:
		/// Computes the percentages and wether the scrollbar should be visible
		void computeScrollBar();
		/// Computes the scrollbarModelMatrix and scrollbarBackgroundModelMatrix
		void computeScrollbarModelMatrices();
		/// Computes the firstVisibleElement and lastVisibleElement member variables
		void computeFirstAndLastVisibleElements();
		/// Draws this GuiElement object relative to its parent element. Will also recursively
		/// draw all children of this element.
		/// worldPosition:		position of the upper left corner of the guiElement in world space
		virtual void draw(const Vec2i& worldPosition) override;
		/// This function is called by the guiManager after registering this GuiElement object.
		/// When this function is called, the guiManager pointer was already set.
		/// This function can e.g. be used for registering callbacks at the guiManager
		virtual void onRegister() override;
		/// Sets the position and size of the children of this element according to
		/// layout rules, if there are any. May recursively call setLayout() on the
		/// children as well
		virtual void enforceLayout() override;
		/// Returns the mouse offset of a child element from this element. Can be
		/// overwritten if the children are displayed at a different place than they
		/// are (eg. in a scrolling list etc)
		Vec2i getChildOffset(const GuiID& childID) const override;

		/// Returns how the given offset vector (relative to the top left corner of the guiElement)
		/// collides with this element
		virtual IsCollidingResult isColliding(const Vec2i& offset) const override;
		/// Returns the first colliding child which collides with the given offset vector. The offset
		/// vector is relative to the top left corner of the guiElement. If zero is returned, this means that
		/// none of this elements children is colliding. This function will call isColliding() on its children
		/// recursively.
		virtual GuiID getCollidingChild(const Vec2i& offset) const override;

		//==============================================================================================
		// Events
		//==============================================================================================

		/// Callback function for mouse button input. Parameters the same as in Scene.h
		virtual void callbackMouseButton(const int& button, const int& action, const int& mods) override;
		/// Callback function for mouse motion. Parameter the same as in Scene.h
		/// position:	position relative to this elements top left corner
		virtual void callbackMouseMotion(const Vec2d& position) override;
		/// Callback function for mouse button input on this GuiElement object. Parameters the same as in Scene.h
		virtual void callbackMouseButtonOnElement(const int& button, const int& action, const int& mods) override;
		/// Callback function for scrolling the mouse wheel. Parameter the same as in Scene.h
		virtual void callbackMouseScrollOnElement(const Vec2d& offset) override;
	};
	//--------------------------------------------------------------------------------------------------
}