#pragma once

#include "Gui\GuiElements\GuiPanel.h"
#include "Graphics/Texture.h"

namespace SnackerEngine
{

	class GuiImage : public GuiPanel
	{
	public:
		/// Enum that specifies wether the GuiElement is resized to the image size or
		/// the other way around!
		enum class GuiImageMode
		{
			RESIZE_TO_IMAGE_SIZE,
			FIT_IMAGE_TO_SIZE,
		};
	public:
		/// Static default Attributes
		static Shader defaultImageShader;
		static Texture defaultTexture;
	private:
		/// Model matrix for the image
		Mat4f modelMatrixImage{};
		/// Shader for the image
		Shader imageShader = defaultImageShader;
		/// The texture/image that is displayed
		Texture texture = defaultTexture;
		/// The image mode
		GuiImageMode guiImageMode = GuiImageMode::FIT_IMAGE_TO_SIZE;
	public:
		/// name of this GuiElementType for JSON parsing
		static constexpr std::string_view typeName = "GUI_IMAGE";
		virtual std::string_view getTypeName() const override { return typeName; }
		/// Default constructor
		GuiImage(Texture texture = defaultTexture, GuiImageMode imageMode = GuiImageMode::FIT_IMAGE_TO_SIZE);
		/// Constructor from JSON
		GuiImage(const nlohmann::json& json, const nlohmann::json* data = nullptr, std::set<std::string>* parameterNames = nullptr);
		/// Destructor
		virtual ~GuiImage() {};
		/// Copy constructor and assignment operator
		GuiImage(const GuiImage& other) noexcept = default;
		GuiImage& operator=(const GuiImage& other) noexcept = default;
		/// Move constructor and assignment operator
		GuiImage(GuiImage&& other) noexcept = default;
		GuiImage& operator=(GuiImage&& other) noexcept = default;
		/// Getters
		const Shader& getImageShader() const { return imageShader; }
		const Texture& getTexture() const { return texture; }
		GuiImageMode getGuiImageMode() const { return guiImageMode; }
		/// Setters
		void setImageShader(const Shader& imageShader) { this->imageShader = imageShader; }
		void setImageTexture(const Texture& texture) { this->texture = texture; onSizeChange(); }
		void setGuiImageMode(GuiImageMode imageMode) { this->guiImageMode = guiImageMode;  }
	protected:
		/// Draws this GuiElement object relative to its parent element. Will also recursively
		/// draw all children of this element.
		/// worldPosition:		position of the upper left corner of the guiElement in world space
		virtual void draw(const Vec2i& worldPosition) override;
		/// This function is called when the size changes. Can eg. be. be used to
		/// compute model matrices. Not called by the constructor. Do not enforce layouts
		/// in this function!
		virtual void onSizeChange() override;
	};

}