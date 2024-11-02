#pragma once

namespace SnackerEngine
{
	#ifdef _DEBUG
	#define ASSERT(x) if (!(x)) __debugbreak();
	#else
	#define ASSERT(x)
	#endif // _DEBUG
}