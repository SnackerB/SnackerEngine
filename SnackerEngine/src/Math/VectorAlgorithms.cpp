#include "Math/VectorAlgorithms.h"

namespace SnackerEngine
{

	std::pair<std::vector<Vec3f>, std::vector<Vec3f>> computeTangentBiTangent(const std::vector<Vec3f>& positions, const std::vector<Vec2f>& texCoords, const std::vector<unsigned int>& indices, const bool& computeBitangent)
	{
		std::vector<Vec3f> tangent(positions.size());
		std::vector<Vec3f> bitangent(positions.size());
		bool average = true;

		std::vector<unsigned int> averaging(positions.size());
		for (unsigned int i = 0; i < averaging.size(); ++i)
			averaging[i] = 0;

		for (unsigned int i = 0; i < indices.size(); i += 3)
		{
			Vec3f x1 = positions[indices[i + 0]];
			Vec3f x2 = positions[indices[i + 1]];
			Vec3f x3 = positions[indices[i + 2]];
			Vec2f t1 = texCoords[indices[i + 0]];
			Vec2f t2 = texCoords[indices[i + 1]];
			Vec2f t3 = texCoords[indices[i + 2]];

			Vec3f xA = x2 - x1;
			Vec3f xB = x3 - x1;
			Vec2f tA = t2 - t1;
			Vec2f tB = t3 - t1;

			float mul = 1 / (tA.x * tB.y - tA.y * tB.x);

			Vec3f T(
				mul * (tB.y * xA.x - tA.y * xB.x),
				mul * (tB.y * xA.y - tA.y * xB.y),
				mul * (tB.y * xA.z - tA.y * xB.z)
			);

			Vec3f B(
				mul * (-tB.x * xA.x + tA.x * xB.x),
				mul * (-tB.x * xA.y + tA.x * xB.y),
				mul * (-tB.x * xA.z + tA.x * xB.z)
			);

			// Average
			if (average)
			{
				for (unsigned int index = i; index < i + 3; ++index)
				{
					if (unsigned int n = averaging[indices[index]] == 0)
					{
						tangent[indices[index]] = T;
						if (computeBitangent)
							bitangent[indices[index]] = B;
					}
					else
					{
						Vec3f T_old = tangent[indices[index]];
						Vec3f B_old = bitangent[indices[index]];
						tangent[indices[index]] = T_old * static_cast<float>(n) / static_cast<float>(n + 1) + T / static_cast<float>(n + 1);
						if (computeBitangent)
							bitangent[indices[index]] = B_old * static_cast<float>(n) / static_cast<float>(n + 1) + B / static_cast<float>(n + 1);
					}
					averaging[indices[index]]++;
				}
			}
			else
			{
				for (unsigned int index = i; index < i + 3; ++index)
				{
					tangent[indices[index]] = T;
					if (computeBitangent)
						bitangent[indices[index]] = B;
				}
			}
		}
		return std::make_pair(tangent, bitangent);
	}

}
