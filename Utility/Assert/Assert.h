#pragma once

namespace SnackerEngine
{
	#ifndef NDEBUG
	#define ASSERT(x) do { if (!(x)) __debugbreak(); } while (0)
	#else
	#define ASSERT(x) do { (void)sizeof(x); } while (0)
	#endif
}