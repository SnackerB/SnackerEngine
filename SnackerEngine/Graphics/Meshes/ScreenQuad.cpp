#include "Graphics/Meshes/ScreenQuad.h"
#include "AssetManager/MeshManager.h"

namespace SnackerEngine
{
	//------------------------------------------------------------------------------------------------------
	const static std::vector<float> vertices = {
		// positions   // texCoords
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f,
	};
	//------------------------------------------------------------------------------------------------------
	const static std::vector<unsigned int> indices = {
		0, 1, 2, // triangle 0
		0, 2, 3  // triangle 1
	};
	//------------------------------------------------------------------------------------------------------
	Mesh createMeshScreenQuad()
	{
		VertexBufferLayout layout;
		layout.push<float>(2); // positions
		layout.push<float>(2); // texCoords
		return MeshManager::createMesh(layout, vertices, indices);
	}
	//------------------------------------------------------------------------------------------------------
}