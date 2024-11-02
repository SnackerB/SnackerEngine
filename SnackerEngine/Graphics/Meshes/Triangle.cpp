#include "Graphics/Meshes/Square.h"
#include "AssetManager/MeshManager.h"

namespace SnackerEngine
{
	//------------------------------------------------------------------------------------------------------
	const static std::vector<float> vertices = {
		// positions and texCoords at the same time
		0.0, 0.0,
		1.0, 0.0,
		1.0, 1.0,
	};
	//------------------------------------------------------------------------------------------------------
	const static std::vector<unsigned int> indices = {
		0, 1, 2, // triangle 0
	};
	//------------------------------------------------------------------------------------------------------
	Mesh createMeshTriangle()
	{
		VertexBufferLayout layout;
		layout.push<float>(2); // positions and texCoords
		return MeshManager::createMesh(layout, vertices, indices);
	}
	//------------------------------------------------------------------------------------------------------
}