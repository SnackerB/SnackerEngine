#include "Graphics/Meshes/Sphere.h"
#include "Math/Vec.h"
#include "AssetManager/MeshManager.h"
#include "Math/VectorAlgorithms.h"

#define _USE_MATH_DEFINES
#include <math.h>

namespace SnackerEngine
{
	//------------------------------------------------------------------------------------------------------
	Mesh createMeshUVSphere(const unsigned int& numLatitudeLines, const unsigned int& numLongitudeLines)
	{
		// Snacked from https://gamedev.stackexchange.com/questions/150191/opengl-calculate-uv-sphere-vertices

		// One vertex at every latitude-longitude intersection,
		// plus one for the north pole and one for the south.
		// One meridian serves as a UV seam, so we double the vertices there.
		unsigned int numVertices = numLatitudeLines * (numLongitudeLines + 1) + 2;
		float radius = 1.0f;

		std::vector<Vec3f> positions(numVertices);
		std::vector<Vec3f> normals(numVertices);
		std::vector<Vec2f> texCoords(numVertices);

		// North pole.
		positions[0] = Vec3f(0, radius, 0);
		normals[0] = positions[0];
		normals[0].normalize();
		texCoords[0] = Vec2f(0, 1);

		// South pole.
		positions[numVertices - 1] = Vec3f(0, -radius, 0);
		normals[numVertices - 1] = positions[numVertices - 1];
		normals[numVertices - 1].normalize();
		texCoords[numVertices - 1] = Vec2f(0, 0);

		// +1.0f because there's a gap between the poles and the first parallel.
		float latitudeSpacing = 1.0f / (numLatitudeLines + 1.0f);
		float longitudeSpacing = 1.0f / (numLongitudeLines);

		// start writing new vertices at position 1
		unsigned int v = 1;
		for (unsigned int latitude = 0; latitude < numLatitudeLines; ++latitude)
		{
			for (unsigned int longitude = 0; longitude <= numLongitudeLines; ++longitude)
			{
				// Scale coordinates into the 0...1 texture coordinate range,
				// with north at the top (y = 1).
				texCoords[v] = Vec2f(
					longitude * longitudeSpacing,
					1.0f - (latitude + 1) * latitudeSpacing
				);

				// Convert to spherical coordinates:
				// theta is a longitude angle (around the equator) in radians.
				// phi is a latitude angle (north or south of the equator).
				float theta = texCoords[v].x * 2.0f * static_cast<float>(M_PI);
				float phi = (texCoords[v].y - 0.5f) * static_cast<float>(M_PI);

				// This determines the radius of the ring of this line of latitude.
				// It's widest at the equator, and narrows as phi increases/decreases.
				float c = cos(phi);

				// Usual formula for a vector in spherical coordinates.
				// You can exchange x & z to wind the opposite way around the sphere.
				positions[v] = Vec3f(
					c * cos(theta),
					sin(phi),
					c * sin(theta)
				) * radius;

				normals[v] = positions[v];
				normals[v].normalize();

				// Proceed to the next vertex.
				v++;
			}
		}

		// Construct indices
		std::vector<unsigned int> indices;
		v = 1;
		for (unsigned int longitude = 0; longitude < numLongitudeLines; ++longitude)
		{
			indices.push_back(v); indices.push_back(v + 1); indices.push_back(0);
			v++;
		}
		v = 1;
		for (unsigned int i = 1; i < numVertices - numLongitudeLines - 2; ++i)
		{
			indices.push_back(v); indices.push_back(v + numLongitudeLines + 1); indices.push_back(v + 1);
			indices.push_back(v + 1); indices.push_back(v + numLongitudeLines + 1); indices.push_back(v + numLongitudeLines + 2);
			v++;
		}
		for (unsigned int longitude = 0; longitude < numLongitudeLines; ++longitude)
		{
			indices.push_back(v); indices.push_back(v + 1); indices.push_back(numVertices - 1);
			v++;
		}

		// Compute tangents and bitangents
		auto tangentBitangent = computeTangentBiTangent(positions, texCoords, indices, false);

		std::vector<float> verticesFinal(numVertices * (3 + 3 + 2 + 3));
		for (unsigned int i = 0; i < numVertices; i++)
		{
			verticesFinal[i * (3 + 3 + 2 + 3) + 0] = positions[i].x; // position
			verticesFinal[i * (3 + 3 + 2 + 3) + 1] = positions[i].y;
			verticesFinal[i * (3 + 3 + 2 + 3) + 2] = positions[i].z;
			verticesFinal[i * (3 + 3 + 2 + 3) + 3] = texCoords[i].x; // texCoords
			verticesFinal[i * (3 + 3 + 2 + 3) + 4] = texCoords[i].y;
			verticesFinal[i * (3 + 3 + 2 + 3) + 5] = normals[i].x; // normals
			verticesFinal[i * (3 + 3 + 2 + 3) + 6] = normals[i].y;
			verticesFinal[i * (3 + 3 + 2 + 3) + 7] = normals[i].z;
			verticesFinal[i * (3 + 3 + 2 + 3) + 8] = tangentBitangent.first[i].x; // tangent
			verticesFinal[i * (3 + 3 + 2 + 3) + 9] = tangentBitangent.first[i].y;
			verticesFinal[i * (3 + 3 + 2 + 3) + 10] =tangentBitangent.first[i].z;
		}

		// Create model
		VertexBufferLayout layout;
		layout.push<float>(3); // position
		layout.push<float>(2); // texCoords
		layout.push<float>(3); // normals
		layout.push<float>(3); // tangent
		return MeshManager::createMesh(layout, verticesFinal, indices);
	}
	//------------------------------------------------------------------------------------------------------
}
