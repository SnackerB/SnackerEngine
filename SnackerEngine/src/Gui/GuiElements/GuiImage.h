#pragma once

#include "Gui/GuiElement.h"
#include "Graphics/Color.h"
#include "Graphics/Texture.h"
#include "Math/Mat.h"
#include "Graphics/Shader.h"

namespace SnackerEngine
{

	/// Forward declaration of GuiStyle
	struct GuiStyle;

	class GuiImage : public GuiElement 
	{
	public:
		/// Enum that specifies wether the GuiElement is resized to the image size or
		/// the other way around!
		enum class GuiImageMode
		{
			RESIZE_TO_IMAGE_SIZE,
			FIT_IMAGE_TO_SIZE,
		};
	private:
		/// Model matrices for the background and for the image
		Mat4f modelMatrixBackground;
		Mat4f modelMatrixImage;
		/// Shader for the background and for the image
		Shader backgroundShader;
		Shader imageShader;
		/// The color used to draw the background
		Color4f backgroundColor;
		/// The texture/image that is displayed
		Texture texture;
		/// the gui image mode
		GuiImageMode guiImageMode;
	protected:
		/// Draws this GuiElement object relative to its parent element. Will also recursively
		/// draw all children of this element.
		/// worldPosition:		position of the upper left corner of the guiElement in world space
		virtual void draw(const Vec2i& worldPosition) override;
		/// This function is called when the size changes. Can eg. be. be used to
		/// compute model matrices. Not called by the constructor. Do not enforce layouts
		/// in this function!
		virtual void onSizeChange() override;
	public:
		/// Constructor
		GuiImage(const Vec2i& position = Vec2i(), const Vec2i& size = Vec2i(),
			const ResizeMode& resizeMode = ResizeMode::DO_NOT_RESIZE,
			const Color4f& backgroundColor = Color4f(0.0f, 0.0f), const Texture& texture = Texture(),
			const GuiImageMode& guiImageMode = GuiImageMode::FIT_IMAGE_TO_SIZE);
		/// Constructors using GuiStyle
		GuiImage(const GuiStyle& style, const Texture& texture);
		/// Getters
		const Color4f& getBackgroundColor() const;
		const Texture& getTexture() const;
		const GuiImageMode& getGuImageMode() const;
		/// Setters
		void setBackgroundColor(const Color4f& backgroundColor);
		void setTexture(const Texture& texture);
		void setGuiImageMode(const GuiImageMode& guiImageMode);
		/// Copy constructor and assignment operator
		GuiImage(const GuiImage& other) noexcept;
		GuiImage& operator=(const GuiImage& other) noexcept;
		/// Move constructor and assignment operator
		GuiImage(GuiImage&& other) noexcept;
		GuiImage& operator=(GuiImage&& other) noexcept;
		/// Destructor
		~GuiImage();
	};
	 
}