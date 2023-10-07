#include "Assert.h"
#include "Core\Log.h"

#include <GLFW/glfw3.h>
#include <string>

namespace SnackerEngine
{

	void GLClearErrors()
	{
		while (GLenum error = glGetError() != GL_NO_ERROR)
		{
			warningLogger << LOGGER::BEGIN << "unresolved ERROR " << error << "!" << LOGGER::ENDL;
		}
	}

	bool GLLogCall(const char* function, const char* file, int line)
	{
		if (GLenum error = glGetError())
		{
			errorLogger << LOGGER::BEGIN << function << " in " << file << " at line " << line << ": ";
			// Print OpenGL error
			switch (error)
			{
			case GL_NO_ERROR:                      errorLogger << "GL_NO_ERROR"; break;
			case GL_INVALID_ENUM:                  errorLogger << "GL_INVALID_ENUM"; break;
			case GL_INVALID_VALUE:                 errorLogger << "GL_INVALID_VALUE"; break;
			case GL_INVALID_OPERATION:             errorLogger << "GL_INVALID_OPERATION"; break;
			case GL_STACK_OVERFLOW:                errorLogger << "GL_STACK_OVERFLOW"; break;
			case GL_STACK_UNDERFLOW:               errorLogger << "GL_STACK_UNDERFLOW"; break;
			case GL_OUT_OF_MEMORY:                 errorLogger << "GL_OUT_OF_MEMORY"; break;
			case 0x8031: /* not core */            errorLogger << "GL_TABLE_TOO_LARGE_EXT"; break;
			case 0x8065: /* not core */            errorLogger << "GL_TEXTURE_TOO_LARGE_EXT"; break;
			default:
				errorLogger << "unhandled error code!"; break;
			}
			errorLogger << LOGGER::ENDL;
			return false;
		}
		return true;
	}

}