#pragma once

#include "Gui/GuiLayout.h"
#include "Utility/Alignment.h"
#include "Graphics/Color.h"
#include "Math/Mat.h"
#include "Graphics/Shader.h"

namespace SnackerEngine
{

	class VerticalListLayout : public GuiLayout
	{
	protected:
		/// size of the border vertically between elements and to the edges, in pixels
		unsigned border;
		/// If this is set to true, the width of the layout snaps to the largest preferredWidth
		/// of its children
		bool snapWidthToPreferred;
		/// If this is set to true, the height of the layout snaps to the total width needed
		bool snapHeight;
		/// If this is set to true, the layout tries to make every child the same width, if possible
		bool makeChildrenSameWidth;
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
		void removeChild(GuiID guiElement) override;
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
		/// Helper function that computes the best common width of all children. 
		/// Useful when makeChildrenSameWidth is set to true!
		int computeBestCommonWidth();
	public:
		/// Constructor
		VerticalListLayout(const unsigned border = 0, const bool snapWidthToPreferred = false, const bool snapHeight = false, const bool makeChildrenSameWidth = false, AlignmentHorizontal alignmentHorizontal = AlignmentHorizontal::LEFT, AlignmentVertical alignmentVertical = AlignmentVertical::TOP);
		/// Adds a child to this guiElement (using row = 0, column = 0). Returns true on success
		bool registerChild(GuiElement& guiElement) override;
		// Copy constructor and assignment operator
		VerticalListLayout(const VerticalListLayout& other) noexcept;
		VerticalListLayout& operator=(const VerticalListLayout& other) noexcept;
		/// Move constructor and assignment operator
		VerticalListLayout(VerticalListLayout&& other) noexcept;
		VerticalListLayout& operator=(VerticalListLayout&& other) noexcept;
		/// Clears all children
		void clear();
		/// Getters & Setters
		const bool& isSnapWidthToPreferred() const { return snapWidthToPreferred; };
		void setSnapWidthToPreferred(const bool& snapWidthToPreferred);
		const bool& isMakeChildrenSameWidth() const { return makeChildrenSameWidth; }
		void setMakeChildrenSameWidth(const bool& makeChildrenSameWidth);
		void setBackgroundColor(const Color4f& backgroundColor);
	};

}