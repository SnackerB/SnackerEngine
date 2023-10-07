#include "AssetManager/LoaderOBJ.h"
#include "Core/Engine.h"
#include "Core/Log.h"
#include "Graphics/Model.h"
#include "AssetManager/MeshManager.h"

#include <unordered_map>
#include <fstream>
#include <optional>

struct two_indices
{
	two_indices(unsigned int index1, unsigned int index2) { this->index1 = index1; this->index2 = index2; };
	unsigned int index1;
	unsigned int index2;
	bool operator<(const two_indices& other) const { return (this->index1 == other.index1) ? (this->index2 < other.index2) : (this->index1 < other.index1); };
	bool operator>(const two_indices& other) const { return (this->index1 == other.index1) ? (this->index2 > other.index2) : (this->index1 > other.index1); };
	bool operator==(const two_indices& other) const { return ((this->index1 == other.index1) && (this->index2 == other.index2)); };
};

struct three_indices
{
	three_indices(unsigned int index1, unsigned int index2, unsigned int index3) { this->index1 = index1; this->index2 = index2; this->index3 = index3; };
	unsigned int index1;
	unsigned int index2;
	unsigned int index3;
	bool operator<(const three_indices& other) const { return (this->index1 == other.index1) ? ((this->index2 == other.index2) ? (this->index3 < other.index3) : (this->index2 < other.index2)) : (this->index1 < other.index1); };
	bool operator>(const three_indices& other) const { return (this->index1 == other.index1) ? ((this->index2 == other.index2) ? (this->index3 > other.index3) : (this->index2 > other.index2)) : (this->index1 > other.index1); };
	bool operator==(const three_indices& other) const { return ((this->index1 == other.index1) && (this->index2 == other.index2) && (this->index3 == other.index3)); };
};

// Hashes for the two custom classes
template<>
struct std::hash<two_indices> {
	std::size_t operator()(const two_indices& key) const
	{
		return (std::hash<unsigned int>()(key.index1) ^ (std::hash<unsigned int>()(key.index2)));
	}
};

template<>
struct std::hash<three_indices> {
	std::size_t operator()(const three_indices& key) const
	{
		return (std::hash<unsigned int>()(key.index1) ^ (std::hash<unsigned int>()(key.index2)) ^ (std::hash<unsigned int>()(key.index3)));
	}
};

namespace SnackerEngine
{

	enum class VERTEX_TYPE
	{
		NONE,
		POS,
		POS_TEX,
		POS_TEX_NORM,
		POS_NORM,
	};

	void addFace(std::istringstream& iss, VERTEX_TYPE& vertexType, std::vector<unsigned int>& indexBuffer,
		std::unordered_map<two_indices, unsigned int>& indexBufferDictionary_two, unsigned int& vertexBufferCounter, 
		std::vector<float>& vertexBuffer, std::vector<Vec3f>& vertices, std::vector<Vec2f>& textureCoordinates, std::vector<Vec3f>& normalCoordinates,
		std::unordered_map<three_indices, unsigned int>& indexBufferDictionary_three)
	{
		if (vertexType == VERTEX_TYPE::NONE)
		{
			// Store old line position
			std::streampos begin_line = iss.tellg();
			// Detect Vertex Type
			unsigned int temp;
			iss >> temp;
			if (iss.peek() == '/')
			{
				iss.get();
				if (iss.peek() == '/')
				{
					vertexType = VERTEX_TYPE::POS_NORM;
				}
				else
				{
					iss >> temp;
					if (iss.get() == '/')
					{
						vertexType = VERTEX_TYPE::POS_TEX_NORM;
					}
					else
					{
						vertexType = VERTEX_TYPE::POS_TEX;
					}
				}
			}
			else
			{
				vertexType = VERTEX_TYPE::POS;
			}
			iss.seekg(begin_line);
		}
		// Add face!
		switch (vertexType)
		{
		case VERTEX_TYPE::POS:
		{
			unsigned int index;
			iss >> index;
			indexBuffer.push_back(index - 1);
			iss >> index;
			indexBuffer.push_back(index - 1);
			iss >> index;
			indexBuffer.push_back(index - 1);

			break;
		}
		case VERTEX_TYPE::POS_NORM:
		{
			unsigned int indexPos;
			unsigned int indexNorm;
			for (unsigned int i = 0; i < 3; ++i)
			{
				iss >> indexPos; indexPos--;
				iss.get(); iss.get();
				iss >> indexNorm; indexNorm--;
				std::unordered_map<two_indices, unsigned int>::iterator it = indexBufferDictionary_two.find({ indexPos, indexNorm });
				if (it != indexBufferDictionary_two.end())
				{
					indexBuffer.push_back(it->second);
				}
				else
				{
					indexBufferDictionary_two.insert(std::make_pair(two_indices(indexPos, indexNorm), vertexBufferCounter));
					vertexBuffer.push_back(vertices[indexPos].x);
					vertexBuffer.push_back(vertices[indexPos].y);
					vertexBuffer.push_back(vertices[indexPos].z);
					vertexBuffer.push_back(normalCoordinates[indexNorm].x);
					vertexBuffer.push_back(normalCoordinates[indexNorm].y);
					vertexBuffer.push_back(normalCoordinates[indexNorm].z);
					indexBuffer.push_back(vertexBufferCounter);
					vertexBufferCounter++;
				}
			}
			break;
		}
		case VERTEX_TYPE::POS_TEX:
		{
			unsigned int indexPos;
			unsigned int indexTex;
			for (unsigned int i = 0; i < 3; i++)
			{
				iss >> indexPos; indexPos--;
				iss.get();
				iss >> indexTex; indexTex--;
				std::unordered_map<two_indices, unsigned int>::iterator it = indexBufferDictionary_two.find({ indexPos, indexTex });
				if (it != indexBufferDictionary_two.end())
				{
					indexBuffer.push_back(it->second);
				}
				else
				{
					indexBufferDictionary_two.insert(std::make_pair(two_indices(indexPos, indexTex), vertexBufferCounter));
					vertexBuffer.push_back(vertices[indexPos].x);
					vertexBuffer.push_back(vertices[indexPos].y);
					vertexBuffer.push_back(vertices[indexPos].z);
					vertexBuffer.push_back(textureCoordinates[indexTex].x);
					vertexBuffer.push_back(textureCoordinates[indexTex].y);
					indexBuffer.push_back(vertexBufferCounter);
					vertexBufferCounter++;
				}
			}
			break;
		}
		case VERTEX_TYPE::POS_TEX_NORM:
		{
			unsigned int indexPos;
			unsigned int indexTex;
			unsigned int indexNorm;
			for (unsigned int i = 0; i < 3; ++i)
			{
				iss >> indexPos; indexPos--;
				iss.get();
				iss >> indexTex; indexTex--;
				iss.get();
				iss >> indexNorm; indexNorm--;
				std::unordered_map<three_indices, unsigned int>::iterator it = indexBufferDictionary_three.find({ indexPos, indexTex, indexNorm });
				if (it != indexBufferDictionary_three.end())
				{
					indexBuffer.push_back(it->second);
				}
				else
				{
					indexBufferDictionary_three.insert(std::make_pair(three_indices(indexPos, indexTex, indexNorm), vertexBufferCounter));
					vertexBuffer.push_back(vertices[indexPos].x);
					vertexBuffer.push_back(vertices[indexPos].y);
					vertexBuffer.push_back(vertices[indexPos].z);
					vertexBuffer.push_back(textureCoordinates[indexTex].x);
					vertexBuffer.push_back(textureCoordinates[indexTex].y);
					vertexBuffer.push_back(normalCoordinates[indexNorm].x);
					vertexBuffer.push_back(normalCoordinates[indexNorm].y);
					vertexBuffer.push_back(normalCoordinates[indexNorm].z);
					indexBuffer.push_back(vertexBufferCounter);
					vertexBufferCounter++;
				}
			}
			break;
		}
		default:
			break;
		}
	}

	std::optional<Mesh> constructMeshOBJ(const VERTEX_TYPE& vertexType, std::vector<Vec3f>& vertices, std::vector<float>& vertexBuffer, std::vector<unsigned int>& indexBuffer)
	{
		if (vertexType == VERTEX_TYPE::NONE)
		{
			return {};
		}

		float* verticesPtr;
		unsigned int vertex_count; // Vertex count (in floats!)
		switch (vertexType)
		{
		case VERTEX_TYPE::POS:
			verticesPtr = &vertices[0].x;
			vertex_count = static_cast<unsigned int>(vertices.size()) * 3;
			break;
		case VERTEX_TYPE::POS_TEX:
			verticesPtr = &vertexBuffer[0];
			vertex_count = static_cast<unsigned int>(vertexBuffer.size());
			break;
		case VERTEX_TYPE::POS_NORM:
			verticesPtr = &vertexBuffer[0];
			vertex_count = static_cast<unsigned int>(vertexBuffer.size());
			break;
		case VERTEX_TYPE::POS_TEX_NORM:
			verticesPtr = &vertexBuffer[0];
			vertex_count = static_cast<unsigned int>(vertexBuffer.size());
			break;
		default:
			verticesPtr = nullptr;
			vertex_count = 0;
			break;
		}

		VertexBufferLayout layout;

		switch (vertexType)
		{
		case VERTEX_TYPE::POS:
			layout.push<float>(3);
			break;
		case VERTEX_TYPE::POS_TEX:
			layout.push<float>(3);
			layout.push<float>(2);
			break;
		case VERTEX_TYPE::POS_NORM:
			layout.push<float>(3);
			layout.push<float>(3);
			break;
		case VERTEX_TYPE::POS_TEX_NORM:
			layout.push<float>(3);
			layout.push<float>(2);
			layout.push<float>(3);
			break;
		default:
			break;
		}

		return std::make_optional(MeshManager::createMesh(layout, vertexBuffer, indexBuffer));
	}

	void resetVectors(std::string& mtlib, std::vector<float>& vertexBuffer, unsigned int& vertexBufferCounter, std::vector<unsigned int>& indexBuffer, 
		std::unordered_map<two_indices, unsigned int>& indexBufferDictionary_two, std::unordered_map<three_indices, unsigned int>& indexBufferDictionary_three,
		VERTEX_TYPE& vertexType)
	{
		mtlib = "";
		vertexBuffer.clear();
		vertexBufferCounter = 0;
		indexBuffer.clear();
		indexBufferDictionary_two.clear();
		indexBufferDictionary_three.clear();
		vertexType = VERTEX_TYPE::NONE;
	}

	Model loadModelOBJ(const std::string& path, const bool& printDebug)
	{
		// Get path
		std::optional<std::string> fullPath = Engine::getFullPath(path);
		if (!fullPath.has_value())
		{
			warningLogger << LOGGER::BEGIN << "Could not find object file at " << path << LOGGER::ENDL;
			return Model();
		}
		std::ifstream file(fullPath.value());
		if (!file.is_open())
		{
			warningLogger << LOGGER::BEGIN << "Could not load object file at " << fullPath.value()  << LOGGER::ENDL;
			return Model();
		}

		std::string line{}; // Current line in the obj file
		std::string mtlib{};
		std::string name{}; // Name of the current mesh
		std::vector<Mesh> meshes{};
		std::vector<Vec3f> vertices{};
		std::vector<Vec2f> textureCoordinates{};
		std::vector<Vec3f> normalCoordinates{};
		/// Various data structures necessary for the loading process
		std::vector<float> vertexBuffer{};
		unsigned int vertexBufferCounter = 0;
		std::vector<unsigned int> indexBuffer{};
		std::unordered_map<two_indices, unsigned int> indexBufferDictionary_two{};
		std::unordered_map<three_indices, unsigned int> indexBufferDictionary_three{};
		VERTEX_TYPE vertexType = VERTEX_TYPE::NONE;
		/// Line counter used for debugging
		unsigned int line_counter = 0;
		// Parse obj file line by line!
		while (std::getline(file, line))
		{
			if (printDebug) {
				line_counter++;
				if (line_counter % 100000 == 0)
				{
					infoLogger << LOGGER::BEGIN << line_counter << LOGGER::ENDL;
				}
			}
			std::istringstream iss(line);
			std::string word;
			if (iss >> word)
			{
				if (word == "v")
				{
					float x, y, z;
					iss >> x;
					iss >> y;
					iss >> z;
					vertices.push_back({ x, y, z });
				}
				else if (word == "vt")
				{
					float x, y;
					iss >> x;
					iss >> y;
					textureCoordinates.push_back({ x, y });
				}
				else if (word == "vn")
				{
					float x, y, z;
					iss >> x;
					iss >> y;
					iss >> z;
					normalCoordinates.push_back({ x, y, z });
				}
				else if (word == "f")
				{
					addFace(iss, vertexType, indexBuffer, indexBufferDictionary_two, vertexBufferCounter, vertexBuffer, vertices, textureCoordinates, normalCoordinates, indexBufferDictionary_three);
				}
				else if (word == "#")
				{
					if (printDebug) {
						infoLogger << LOGGER::BEGIN << "comment: ";
						while (iss >> word)
						{
							infoLogger << word << " ";
						}
						infoLogger << LOGGER::ENDL;
					}
				}
				else if (word == "mtllib")
				{
					while (iss >> word)
					{
						mtlib = word;
					}
				}
				else if (word == "o")
				{
					if (name != "")
					{
						// new mesh starts from here. Construct mesh, reset vectors!
						auto mesh = constructMeshOBJ(vertexType, vertices, vertexBuffer, indexBuffer);
						if (mesh) meshes.push_back(mesh.value());
						resetVectors(mtlib, vertexBuffer, vertexBufferCounter, indexBuffer, indexBufferDictionary_two, indexBufferDictionary_three, vertexType);
					}
					while (iss >> word)
					{
						name = word;
					}
				}
			}
		}
		// Insert the final (last) mesh
		auto mesh = constructMeshOBJ(vertexType, vertices, vertexBuffer, indexBuffer);
		if (mesh) meshes.push_back(mesh.value());
		return Model(meshes);
	}

}