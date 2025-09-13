#pragma once

#include "Gui/GuiElements/GuiPanel.h"
#include "Gui/Text/Text.h"
#include "Graphics/Material.h"
#include "Graphics/Color.h"
#include "Utility\Alignment.h"

namespace SnackerEngine
{

	class GuiTextBox : public GuiPanel
	{
	public:
		/// Controls how the textbox scales the text based on the size of the box
		enum class TextScaleMode
		{
			DONT_SCALE,			/// Text is not scaled
			SCALE_UP,			/// Text is scaled up to try to fit the box better.
			/// Works best in SINGLE_LINE parse mode
			SCALE_DOWN,			/// Text is scaled down to try to fit the box better.
			/// Works best in SINGLE_LINE parse mode
			SCALE_UP_DOWN,		/// Text is scaled up or down to try to fit the box better.
			/// Works best in SINGLE_LINE parse mode
			RECOMPUTE_DOWN,		/// If the text is too large it is recomputed with a lower
			/// font size. Also works well in other modes than SINGLE_LINE.
			/// Can be computationally expensive
			RECOMPUTE_UP_DOWN,	/// If the text is too smal/large it is recomputed with a larger/lower
			/// font size. Also works well in other modes than SINGLE_LINE.
			/// Can be computationally expensive
		};
		/// Controls how the size hints (minSize, maxSize, preferredSize) are set based on the text
		enum class SizeHintMode
		{
			ARBITRARY,					/// does not change the size hint
			SET_TO_TEXT_SIZE,			/// sets the size hint to the textSize
			SET_TO_TEXT_WIDTH,			/// sets the width of the size hint to the textWidth
			SET_TO_TEXT_HEIGHT,			/// sets the height of the size hint to the textHeight
		};
		/// struct that encapsulates three instances of SizeHintMode to fully specify the size hint
		/// behaviour of the text box
		struct SizeHintModes
		{
			SizeHintMode sizeHintModeMinSize;
			SizeHintMode sizeHintModeMaxSize;
			SizeHintMode sizeHintModePreferredSize;
			SizeHintModes() = default;
			SizeHintModes(SizeHintMode sizeHintModeMinSize, SizeHintMode sizeHintModeMaxSize, SizeHintMode sizeHintModePreferredSize)
				: sizeHintModeMinSize(sizeHintModeMinSize), sizeHintModeMaxSize(sizeHintModeMaxSize), sizeHintModePreferredSize(sizeHintModePreferredSize) {}
			SizeHintModes(SizeHintMode sizeHintMode)
				: sizeHintModeMinSize(sizeHintMode), sizeHintModeMaxSize(sizeHintMode), sizeHintModePreferredSize(sizeHintMode) {}
		};
	public:
		/// Static default Attributes
		static Color4f defaultTextColor;
		static Color4f defaultBackgroundColor;
		static unsigned defaultRecomputeTries;
		static SizeHintModes defaultSizeHintModes;
	protected:
		/// The text object. This needs to be a pointer such that the Edit text class can save an 
		/// EditableText object in this variable instead (thus avoiding code duplication)
		std::unique_ptr<DynamicText> dynamicText = nullptr;
		/// The scale factor of the text, used internally for some textBoxModes
		double scaleFactor = 1.0;
		/// Helper function that computes the the text position
		Vec2f computeTextPosition();
		/// Helper function that draws the text
		void drawText(const Vec2i& worldPosition);
		/// Helper function that draws the text, transforming it by the given transform matrix
		void drawText(const Vec2i& worldPosition, const Mat4f& transformMatrix);
		/// Helper function that recomputes the text and corresponding model matrices.
		/// This function should be called when the text needs to be recomputed, eg.
		/// when the font or fontSize changes.
		/// May also change the size of the textBox, depending on the textBoxMode
		void recomputeText();
		/// Helper function that should get called when the element is registered
		void onRegister(std::unique_ptr<DynamicText>&& dynamicText);
	private:
		/// Material, containing font, textColor and backgroundColor.
		/// MaterialData is of type SimpleTextMaterialData
		Material material = {};
		/// Text color
		Color4f textColor = defaultTextColor;
		/// Model matrices of the text
		Mat4f modelMatrixText = {};
		/// Text scale mode
		TextScaleMode textScaleMode = TextScaleMode::DONT_SCALE;
		/// Size hint modes
		SizeHintModes sizeHintModes = defaultSizeHintModes;
		/// border between the box and the text in pixels
		int border = GuiElement::defaultBorderSmall;
		/// The normal font size of the text (without any rescaling), in pt.
		double fontSize = GuiElement::defaultFontSizeNormal;
		/// The number of tries that are being made to resize the text when the textBoxMode is set to 
		/// FORCE_SIZE_RECOMPUTE_SCALE_DOWN or FORCE_SIZE_RECOMPUTE_SCALE
		unsigned int recomputeTries = defaultRecomputeTries;
		/// If this is set to false, the text will not be recomputed on size changes, which can be useful
		/// to save resources. The text will still be rescaled according to the textScaleMode.
		/// Default value: True
		bool doRecomputeOnSizeChange = true;
		/// Saving the last size the text was computed with. Useful for not doing unnecessary resizes.
		Vec2i lastSizeOnRecomputeText = Vec2i(-1, -1);
		/// The displayed text
		std::string text = "";
		/// The parse mode
		StaticText::ParseMode parseMode = StaticText::ParseMode::SINGLE_LINE;
		/// The text alignment
		AlignmentHorizontal alignmentHorizontal = AlignmentHorizontal::LEFT;
		AlignmentVertical alignmentVertical = AlignmentVertical::CENTER;
		/// The font of the text
		Font font = GuiElement::defaultFont;
		/// Helper function that Computes the modelMatrix of the text and the background box.
		/// Depending on the textBoxMode the text is scaled appropriately.
		void computeModelMatrices();
		/// Helper function that should be called on size change. Only recomputes the text
		/// if it is absolutely necessary, saving computation time!
		void recomputeTextOnSizeChange();
		/// Helper functions computing the correct values for the size hint height variables, ie.
		/// minheight, maxheight and preferredHeight.
		void comouteWidthHints();
		/// Helper function computing the correct values for the size hint width variables, ie.
		/// minWidth, maxWidth and preferredWidth. This function should only be called if the text,
		/// font, fontsize, ord border changes. The width hint is computed as if the parse mode was set
		/// to single_line.
		void computeHeightHints();
		/// Helper function that constructs the text material
		static Material constructTextMaterial(const Font& font, const Color4f& textColor, const Color4f& backgroundColor);
	protected:
		/// Sets the internal text variable. Can be used by derived GuiElements
		void setTextInternal(const std::string& text) { this->text = text; };
	public:
		/// name of this GuiElementType for JSON parsing
		static constexpr std::string_view typeName = "GUI_TEXT_BOX";
		virtual std::string_view getTypeName() const override { return typeName; }
		/// Default constructor
		GuiTextBox(const Vec2i& position = Vec2i(), const Vec2i& size = Vec2i(), const std::string& text = "", const Font& font = defaultFont, const double& fontSize = defaultFontSizeNormal, const Color4f& backgroundColor = defaultBackgroundColor);
		GuiTextBox(const std::string& text);
		/// Constructor from JSON
		GuiTextBox(const nlohmann::json& json, const nlohmann::json* data = nullptr, std::set<std::string>* parameterNames = nullptr);
		/// Destructor
		virtual ~GuiTextBox() {}
		/// Copy constructor and assignment operator
		GuiTextBox(const GuiTextBox& other) noexcept;
		GuiTextBox& operator=(const GuiTextBox& other) noexcept;
		/// Move constructor and assignment operator
		GuiTextBox(GuiTextBox&& other) noexcept = default;
		GuiTextBox& operator=(GuiTextBox&& other) noexcept = default;
		/// Getters
		Vec2i getTextSize() const;
		const Color4f& getTextColor() const { return textColor; }
		TextScaleMode getTextScaleMode() const { return textScaleMode; }
		const SizeHintModes& getSizeHintModes() const { return sizeHintModes; }
		const int getBorder() const { return border; }
		double getFontSize() const { return fontSize; }
		int getRecomputeTries() const { return recomputeTries; }
		bool isDoRecomputeOnSizeChange() const { return doRecomputeOnSizeChange; }
		const std::string& getText() const { return text; }
		StaticText::ParseMode getParseMode() const { return parseMode; }
		AlignmentHorizontal getAlignmentHorizontal() const { return alignmentHorizontal; }
		AlignmentVertical getAlignmentVertical() const { return alignmentVertical; }
		const Font& getFont() const { return font; }
		/// Setters
		void setTextColor(const Color4f& textColor);
		void setTextScaleMode(TextScaleMode textScaleMode);
		void setSizeHintModeMinSize(const SizeHintMode& sizeHintModeMinSize);
		void setSizeHintModeMaxSize(const SizeHintMode& sizeHintModeMaxSize);
		void setSizeHintModePreferredSize(const SizeHintMode& sizeHintModePreferredSize);
		void setSizeHintModes(const SizeHintModes& sizeHintModes);
		void setBorder(const int& border);
		void setFontSize(const double& fontSize);
		void setRecomputeTries(int recomputeTries);
		void setDoRecomputeOnSizeChange(const bool& doRecomputeOnSizeChange);
		void setText(const std::string& text);
		void setParseMode(const StaticText::ParseMode& parseMode);
		void setAlignmentHorizontal(AlignmentHorizontal alignmentHorizontal);
		void setAlignmentVertical(AlignmentVertical alignmentVertical);
		void setFont(const Font& font);
		void setBackgroundColor(const Color4f& backgroundColor) override;

		//==============================================================================================
		// Animatables
		//==============================================================================================

		std::unique_ptr<GuiElementAnimatable> animateTextColor(const Color4f& startVal, const Color4f& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear);
		std::unique_ptr<GuiElementAnimatable> animateBorder(const int& startVal, const int& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear);
		std::unique_ptr<GuiElementAnimatable> animateFontSize(const double& startVal, const double& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear);
		std::unique_ptr<GuiElementAnimatable> animateBackgroundColor(const Color4f& startVal, const Color4f& stopVal, double duration, std::function<double(double)> animationFunction = AnimationFunction::linear) override;

	protected:
		/// Function that draws the panel relative to its parent element, but also transforms according
		/// to the given transform matrix. This function can be called by child elements.
		/// Does not draw children.
		void draw(const Vec2i& worldPosition, const Mat4f& transformMatrix);
		/// Function that draws the panel relative to its parent element, but also transforms according
		/// to the given transform matrix. This function can be called by child elements.
		/// Does not draw children. Clipping box for drawing text is modified by the offset and the
		/// clipping box is of the given size.
		void draw(const Vec2i& worldPosition, const Mat4f& transformMatrix, const Vec2i& clippingBoxOffset, const Vec2i& clippingBoxSize);
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
	};
}