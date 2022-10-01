#include "Gui/Text/TextMaterial.h"

namespace SnackerEngine
{
	//------------------------------------------------------------------------------------------------------
	SimpleTextMaterialData::SimpleTextMaterialData(const Shader& shader, const Font& font, const Color4f& textColor, const Color4f& backgroundColor)
		: MaterialData(shader), font(font), textColor(textColor), backgroundColor(backgroundColor) {}
	//------------------------------------------------------------------------------------------------------
	void SimpleTextMaterialData::bind()
	{
		MaterialData::bind();
		shader.setUniform<SnackerEngine::Color4f>("u_bgColor", backgroundColor);
		shader.setUniform<SnackerEngine::Color4f>("u_fgColor", textColor);
		shader.setUniform<SnackerEngine::Vec2i>("u_msdfDims", font.getMsdfDims());
		font.bind();
	}
	//------------------------------------------------------------------------------------------------------
}