#include "Gui/Text/TextMaterial.h"

namespace SnackerEngine
{
	//------------------------------------------------------------------------------------------------------
	SimpleTextMaterialData::SimpleTextMaterialData(const Shader& shader, const Font& font, const Color4f& color)
		: MaterialData(shader), font(font), color(color) {}
	//------------------------------------------------------------------------------------------------------
	void SimpleTextMaterialData::bind()
	{
		MaterialData::bind();
		shader.setUniform<SnackerEngine::Color4f>("u_color", color);
		shader.setUniform<SnackerEngine::Vec2i>("u_msdfDims", font.getMsdfDims());
		font.bind();
	}
	//------------------------------------------------------------------------------------------------------
}