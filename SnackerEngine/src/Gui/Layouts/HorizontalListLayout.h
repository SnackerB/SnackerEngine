#pragma once

#include "Gui/GuiLayout.h"
#include "Utility/Alignment.h"
#include "Graphics/Color.h"
#include "Math/Mat.h"
#include "Graphics/Shader.h"

namespace SnackerEngine
{

	class HorizontalListLayout : public GuiLayout
	{
		/// size of the border vertically between elements and to the edges, in pixels
		unsigned border;
		/// If this is set to true, the height of the layout snaps to the largest preferredHeight
		/// of its children
		bool snapHeightToPreferred;
		/// Horizontal and vertical alignment. Default: LEFT, TOP
		AlignmentHorizontal alignmentHorizontal;
		AlignmentVertical alignmentVertical;
		/// Background color
		Color4f backgroundColor;
		/// Model of background matrix
		Mat4f modelMatrixBackground;
		/// Shader for drawing the background
		Shader backgroundShader;
		/// Removes the given child guiElement from this layout
		void removeChild(GuiElement& guiElement) override;
		/// Enforces this layout by possibly changing position and size of the children guiElements
		void enforceLayout() override;
		/// Computes the background model matrix
		void computeModelMatrix();
		/// Draws this GuiPanel object relative to its parent element. Will also recursively
		/// draw all children of this element.
		/// parentPosition:		position of the upper left corner of the parent element
		virtual void draw(const Vec2i& parentPosition) override;
		/// This function is called when the size changes. Can eg. be. be used to
		/// compute model matrices. Not called by the constructor. Do not enforce layouts
		/// in this function!
		virtual void onSizeChange() override;
	public:
		/// Constructor
		HorizontalListLayout(const unsigned& border = 0, const bool& snapHeightToPreferred = false, AlignmentHorizontal alignmentHorizontal = AlignmentHorizontal::LEFT, AlignmentVertical alignmentVertical = AlignmentVertical::TOP);
		// Copy constructor and assignment operator
		HorizontalListLayout(const HorizontalListLayout& other) noexcept;
		HorizontalListLayout& operator=(const HorizontalListLayout& other) noexcept;
		/// Move constructor and assignment operator
		HorizontalListLayout(HorizontalListLayout&& other) noexcept;
		HorizontalListLayout& operator=(HorizontalListLayout&& other) noexcept;
		/// Getters & Setters
		const bool& isSnapHeightToPreferred() const { return snapHeightToPreferred; };
		void setSnapheightToPreferred(const bool& snapHeightToPreferred);
		void setBackgroundColor(const Color3f& backgroundColor);
	};

}