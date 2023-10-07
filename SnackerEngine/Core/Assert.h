#pragma once

#include "Utility\Debug\Assert.h"

namespace SnackerEngine
{
#ifdef _DEBUG
#define GLCall(x) GLClearErrors(); x; ASSERT(GLLogCall(#x, __FILE__, __LINE__))
#define ALCall(x) ALClearErrors(); x; ASSERT(ALLogCall(#x, __FILE__, __LINE__))
#else
#define GLCall(x) x
#define ALCall(x) x
#endif // DEBUG
	//------------------------------------------------------------------------------------------------------
	/// Clears all previous GL errors
	void GLClearErrors();
	//------------------------------------------------------------------------------------------------------
	/// Prints errors if there were any
	bool GLLogCall(const char* function, const char* file, int line);
	//------------------------------------------------------------------------------------------------------
}