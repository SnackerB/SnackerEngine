#include "Graphics/Meshes/Plane.h"
#include "AssetManager/MeshManager.h"

namespace SnackerEngine
{
	//------------------------------------------------------------------------------------------------------
	// vertex data for only positions cube
	const static std::vector<float> positions = {
		0.0, 0.0, 0.0,
		0.0, 1.0, 0.0,
		1.0, 1.0, 0.0,
		1.0, 0.0, 0.0,
	};
	//------------------------------------------------------------------------------------------------------
	// index data for only positions cube
	const static std::vector<unsigned int> indices = {
		0, 1, 2,
		2, 3, 0,
	};
	//------------------------------------------------------------------------------------------------------
	// textCoordinates
	const static std::vector<float> texCoords = {
		0.0, 0.0,
		0.0, 1.0,
		1.0, 1.0,
		1.0, 0.0,
	};
	//------------------------------------------------------------------------------------------------------
	// normal vectors
	const static std::vector<float> normals = {
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
	};
	//------------------------------------------------------------------------------------------------------
	// tangent vectors
	const static std::vector<float> tangents = {
		1.0, 0.0, 0.0,
		1.0, 0.0, 0.0,
		1.0, 0.0, 0.0,
		1.0, 0.0, 0.0,
	};
	//------------------------------------------------------------------------------------------------------
	Mesh createMeshPlane(const bool& TexCoords, const bool& Normals, const bool& Tangents)
	{
		VertexBufferLayout layout;
		unsigned int totalVertexDataSize = static_cast<unsigned int>(positions.size());
		layout.push<float>(3); // positions
		if (TexCoords) {
			totalVertexDataSize += static_cast<unsigned int>(texCoords.size());
			layout.push<float>(2); // texCoords
		}
		if (Normals) {
			totalVertexDataSize += static_cast<unsigned int>(normals.size());
			layout.push<float>(3); // normals
		}
		if (Tangents) {
			totalVertexDataSize += static_cast<unsigned int>(tangents.size());
			layout.push<float>(3); // tangents
		}
		std::vector<float> vertices;
		vertices.reserve(totalVertexDataSize);
		unsigned int index = 0;
		for (unsigned int i = 0; i < 4; ++i) {
			vertices.push_back(positions[i * 3]);
			vertices.push_back(positions[i * 3 + 1]);
			vertices.push_back(positions[i * 3 + 2]);
			if (TexCoords) {
				vertices.push_back(texCoords[i * 2]);
				vertices.push_back(texCoords[i * 2 + 1]);
			}
			if (Normals) {
				vertices.push_back(normals[i * 3]);
				vertices.push_back(normals[i * 3 + 1]);
				vertices.push_back(normals[i * 3 + 2]);
			}
			if (Tangents) {
				vertices.push_back(tangents[i * 3]);
				vertices.push_back(tangents[i * 3 + 1]);
				vertices.push_back(tangents[i * 3 + 2]);
			}
		}
		return MeshManager::createMesh(layout, vertices, indices);
	}
	//------------------------------------------------------------------------------------------------------
}