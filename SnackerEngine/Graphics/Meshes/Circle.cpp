#include "Circle.h"
#include "Math/Vec.h"
#include "AssetManager/MeshManager.h"
#include "core/Log.h" // DEBUG

#define _USE_MATH_DEFINES
#include <math.h>

namespace SnackerEngine
{
	Mesh SnackerEngine::createMeshCircle(unsigned int subdivisions)
	{
		if (subdivisions < 3) subdivisions = 3;
		std::vector<float> vertices;
		vertices.reserve((static_cast<std::size_t>(subdivisions) + 1) * 2);
		std::vector<unsigned int> indices;
		indices.reserve(subdivisions * 3);
		vertices.push_back(0.5f); vertices.push_back(0.5f);
		for (unsigned int i = 0; i < subdivisions; ++i) {
			float rad = static_cast<float>(i) / static_cast<float>(subdivisions) * 2.0f * static_cast<float>(M_PI);
			vertices.push_back(std::cos(rad)*0.5f + 0.5f);
			vertices.push_back(std::sin(rad)*0.5f + 0.5f);
			indices.push_back(0);
			indices.push_back(1 + i);
			indices.push_back(1 + (i + 1) % subdivisions);
		}
		VertexBufferLayout layout;
		layout.push<float>(2); // positions and texCoords
		return MeshManager::createMesh(layout, vertices, indices);
	}

}
