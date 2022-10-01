#pragma once

#include "Graphics/Material.h"
#include "AssetManager/MaterialData.h"
#include "Gui/Text/Font.h"
#include "Graphics/Color.h"

namespace SnackerEngine
{
	//------------------------------------------------------------------------------------------------------
	struct SimpleTextMaterialData : public MaterialData
	{
		/// The font the text is in
		Font font;
		/// Text color
		Color4f textColor;
		/// Background color
		Color4f backgroundColor;
		/// Constructor
		SimpleTextMaterialData(const Shader& shader, const Font& font, const Color4f& textColor, const Color4f& backgroundColor);
		/// Binds this material
		void bind() override;
	};
	//------------------------------------------------------------------------------------------------------
}