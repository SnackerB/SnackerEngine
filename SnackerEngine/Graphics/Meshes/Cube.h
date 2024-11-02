#pragma once

#include "Graphics/Mesh.h"

namespace SnackerEngine
{
	//------------------------------------------------------------------------------------------------------
	/// Helper function that creates a cube mesh. Should be called only by the meshManager
	Mesh createMeshCube(const bool& TexCoords = false, const bool& Normals = false, const bool& Tangents = false);
	//------------------------------------------------------------------------------------------------------
}