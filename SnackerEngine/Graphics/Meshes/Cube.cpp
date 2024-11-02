#include "AssetManager/MeshManager.h"

namespace SnackerEngine
{
	//------------------------------------------------------------------------------------------------------
	// vertex data for only positions cube
	const static std::vector<float> positionsCubeSmall = {
		// front
		-1.0, -1.0,  1.0,
		 1.0, -1.0,  1.0,
		 1.0,  1.0,  1.0,
		-1.0,  1.0,  1.0,
		// back
		-1.0, -1.0, -1.0,
		 1.0, -1.0, -1.0,
		 1.0,  1.0, -1.0,
		-1.0,  1.0, -1.0,
	};
	//------------------------------------------------------------------------------------------------------
	// index data for only positions cube
	const static std::vector<unsigned int> indicesCubeSmall = {
		// front
		0, 1, 2,
		2, 3, 0,
		// right
		1, 5, 6,
		6, 2, 1,
		// back
		7, 6, 5,
		5, 4, 7,
		// left
		4, 0, 3,
		3, 7, 4,
		// bottom
		4, 5, 1,
		1, 0, 4,
		// top
		3, 2, 6,
		6, 7, 3
	};
	//------------------------------------------------------------------------------------------------------
	// positions for cube with more data than just positions
	const static std::vector<float> positionsCubeLarge = {
		// front
		-1.0, -1.0,  1.0,
		 1.0, -1.0,  1.0,
		 1.0,  1.0,  1.0,
		-1.0,  1.0,  1.0,
		// right
		 1.0, -1.0,  1.0,
		 1.0, -1.0, -1.0,
		 1.0,  1.0, -1.0,
		 1.0,  1.0,  1.0,
		 // back
		 1.0, -1.0, -1.0,
		-1.0, -1.0, -1.0,
		-1.0,  1.0, -1.0,
		 1.0,  1.0, -1.0,
		 // left
		-1.0, -1.0, -1.0,
		-1.0, -1.0,  1.0,
		-1.0,  1.0,  1.0,
		-1.0,  1.0, -1.0,
		// top
		-1.0,  1.0,  1.0,
		 1.0,  1.0,  1.0,
		 1.0,  1.0, -1.0,
		-1.0,  1.0, -1.0,
		// bottom
		-1.0, -1.0, -1.0,
		 1.0, -1.0, -1.0,
		 1.0, -1.0,  1.0,
		-1.0, -1.0,  1.0,
	};
	//------------------------------------------------------------------------------------------------------
	// indices for cube with more data than just positions
	const static std::vector<unsigned int> indicesCubeLarge = {
		// front
		0, 1, 2,
		2, 3, 0,
		// right
		4, 5, 6,
		6, 7, 4,
		// back
		8, 9, 10,
		10, 11, 8,
		// left
		12, 13, 14,
		14, 15, 12,
		// top
		16, 17, 18,
		18, 19, 16,
		// botom
		20, 21, 22,
		22, 23, 20,
	};
	//------------------------------------------------------------------------------------------------------
	// textCoordinates
	const static std::vector<float> texCoordsCubeLarge = {
		// front
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,
		// right
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,
		// back
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,
		// left
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,
		// top
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,
		// bottom
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,
	};
	//------------------------------------------------------------------------------------------------------
	// normal vectors
	const static std::vector<float> normalsCubeLarge = {
		// front
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		// right
		1.0, 0.0, 0.0,
		1.0, 0.0, 0.0,
		1.0, 0.0, 0.0,
		1.0, 0.0, 0.0,
		// back
		0.0, 0.0, -1.0,
		0.0, 0.0, -1.0,
		0.0, 0.0, -1.0,
		0.0, 0.0, -1.0,
		// left
		-1.0, 0.0, 0.0,
		-1.0, 0.0, 0.0,
		-1.0, 0.0, 0.0,
		-1.0, 0.0, 0.0,
		// top
		0.0, 1.0, 0.0,
		0.0, 1.0, 0.0,
		0.0, 1.0, 0.0,
		0.0, 1.0, 0.0,
		// bottom
		0.0, -1.0, 0.0,
		0.0, -1.0, 0.0,
		0.0, -1.0, 0.0,
		0.0, -1.0, 0.0,
	};
	//------------------------------------------------------------------------------------------------------
	// tangent vectors
	const static std::vector<float> tangentsCubeLarge = {
		// front
		1.0, 0.0, 0.0,
		1.0, 0.0, 0.0,
		1.0, 0.0, 0.0,
		1.0, 0.0, 0.0,
		// right
		0.0, 0.0, -1.0,
		0.0, 0.0, -1.0,
		0.0, 0.0, -1.0,
		0.0, 0.0, -1.0,
		// back
		-1.0, 0.0, 0.0,
		-1.0, 0.0, 0.0,
		-1.0, 0.0, 0.0,
		-1.0, 0.0, 0.0,
		// left
	   0.0, 0.0, 1.0,
	   0.0, 0.0, 1.0,
	   0.0, 0.0, 1.0,
	   0.0, 0.0, 1.0,
	   // top
	  1.0, 0.0, 0.0,
	  1.0, 0.0, 0.0,
	  1.0, 0.0, 0.0,
	  1.0, 0.0, 0.0,
	  // bottom
	  1.0, 0.0, 0.0,
	  1.0, 0.0, 0.0,
	  1.0, 0.0, 0.0,
	  1.0, 0.0, 0.0,
	};
	//------------------------------------------------------------------------------------------------------
	Mesh createMeshCube(const bool& TexCoords, const bool& Normals, const bool& Tangents)
	{
		if (!TexCoords && !Normals && !Tangents) {
			// only positions!
			VertexBufferLayout layout;
			layout.push<float>(3); // positions
			return MeshManager::createMesh(layout, positionsCubeSmall, indicesCubeSmall);
		}
		VertexBufferLayout layout;
		unsigned int totalVertexDataSize = static_cast<unsigned int>(positionsCubeLarge.size());
		layout.push<float>(3); // positions
		if (TexCoords) {
			totalVertexDataSize += static_cast<unsigned int>(texCoordsCubeLarge.size());
			layout.push<float>(2); // texCoords
		}
		if (Normals) {
			totalVertexDataSize += static_cast<unsigned int>(normalsCubeLarge.size());
			layout.push<float>(3); // normals
		}
		if (Tangents) {
			totalVertexDataSize += static_cast<unsigned int>(tangentsCubeLarge.size());
			layout.push<float>(3); // tangents
		}
		std::vector<float> vertices;
		vertices.reserve(totalVertexDataSize);
		unsigned int index = 0;
		for (unsigned int i = 0; i < 24; ++i) {
			vertices.push_back(positionsCubeLarge[i * 3]);
			vertices.push_back(positionsCubeLarge[i * 3 + 1]);
			vertices.push_back(positionsCubeLarge[i * 3 + 2]);
			if (TexCoords) {
				vertices.push_back(texCoordsCubeLarge[i * 2]);
				vertices.push_back(texCoordsCubeLarge[i * 2 + 1]);
			}
			if (Normals) {
				vertices.push_back(normalsCubeLarge[i * 3]);
				vertices.push_back(normalsCubeLarge[i * 3 + 1]);
				vertices.push_back(normalsCubeLarge[i * 3 + 2]);
			}
			if (Tangents) {
				vertices.push_back(tangentsCubeLarge[i * 3]);
				vertices.push_back(tangentsCubeLarge[i * 3 + 1]);
				vertices.push_back(tangentsCubeLarge[i * 3 + 2]);
			}
		}
		return MeshManager::createMesh(layout, vertices, indicesCubeLarge);
	}
	//------------------------------------------------------------------------------------------------------
}