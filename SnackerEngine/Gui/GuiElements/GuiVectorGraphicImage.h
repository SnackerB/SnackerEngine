#pragma once

#include "Gui\GuiElements\GuiImage.h"

namespace SnackerEngine
{

	class GuiVectorGraphicImage : public GuiImage
	{
	public:
		static Shader defaultVectorGraphicImageShader;
	private:
		// The color of the vector graphic
		Color4f color{};
	public:
		/// name of this GuiElementType for JSON parsing
		static constexpr std::string_view typeName = "GUI_VECTOR_GRAPHIC_IMAGE";
		virtual std::string_view getTypeName() const override { return typeName; }
		/// Default constructor
		GuiVectorGraphicImage(Color4f color = Color4f{}, Texture texture = defaultTexture, GuiImageMode imageMode = GuiImageMode::FIT_IMAGE_TO_SIZE, GuiImageSizeHintMode sizeHintMode = GuiImageSizeHintMode::ARBITRARY);
		/// Constructor from JSON
		GuiVectorGraphicImage(const nlohmann::json& json, ParameterNameSet* parameterNameSet = nullptr);
		/// Destructor
		virtual ~GuiVectorGraphicImage() {};
		/// Copy constructor and assignment operator
		GuiVectorGraphicImage(const GuiVectorGraphicImage& other) noexcept = default;
		GuiVectorGraphicImage& operator=(const GuiVectorGraphicImage& other) noexcept = default;
		/// Move constructor and assignment operator
		GuiVectorGraphicImage(GuiVectorGraphicImage&& other) noexcept = default;
		GuiVectorGraphicImage& operator=(GuiVectorGraphicImage&& other) noexcept = default;
	protected:
		/// Draws this GuiElement object relative to its parent element. Will also recursively
		/// draw all children of this element.
		/// worldPosition:		position of the upper left corner of the guiElement in world space
		virtual void draw(const Vec2i& worldPosition) override;
	};

}