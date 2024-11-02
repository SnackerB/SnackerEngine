#pragma once

namespace SnackerEngine
{

	class AssetManager
	{
	protected:
		friend class Engine;
		static void initialize();
		static void terminate();
	};

}