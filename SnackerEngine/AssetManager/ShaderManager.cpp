#include "ShaderManager.h"
#include "Core\Assert.h"
#include "Core/Log.h"
#include "Core/Engine.h"
#include "Graphics/Color.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <fstream>
#include <optional>
#include <sstream>

namespace SnackerEngine
{
	//======================================================================================================
	// ShaderData implementation
	//======================================================================================================
	ShaderManager::ShaderData::~ShaderData()
	{
		if (valid) {
			GLCall(glDeleteProgram(GPU_ID));
		}
	}
	//------------------------------------------------------------------------------------------------------
	void ShaderManager::ShaderData::bind() const
	{
		if (valid) {
			GLCall(glUseProgram(GPU_ID));
		}
	}
	//------------------------------------------------------------------------------------------------------
	void ShaderManager::unbindAll()
	{
		GLCall(glUseProgram(0));
	}
	//======================================================================================================
	// Helper function to load and compile shaders
	//======================================================================================================
	// Compiles the given shader. returns id of the compiled shader. Returns an empty optional if something went wrong.
	// In this case, a debug message will be printed to the console.
	// type: can be either GL_VERTEX_SHADER or GL_FRAGMENT_SHADER
	std::optional<unsigned int> compileShader(const unsigned int& type, const std::string& source)
	{
		// If the source is empty, don't bother compiling
		if (source.empty()) {
			// Print error message to console!
			switch (type)
			{
			case GL_VERTEX_SHADER:
				errorLogger << LOGGER::BEGIN << "Failed to compile vertex shader!" << LOGGER::ENDL;
				break;
			case GL_FRAGMENT_SHADER:
				errorLogger << LOGGER::BEGIN << "Failed to compile fragment shader!" << LOGGER::ENDL;
				break;
			default:
				errorLogger << LOGGER::BEGIN << "Failed to compile shader of unknown type!" << LOGGER::ENDL;
				break;
			}
			errorLogger << LOGGER::BEGIN << "Shader source was empty." << LOGGER::ENDL;
			// delete shader in case there is an error!
			glDeleteShader(0);
			return {};
		}
		// Creates a shader of the given type and saves the id in "id"
		GLCall(unsigned int id = glCreateShader(type));
		// Turns source into a const char* 
		const char* src = source.c_str();

		// Sets the source code in this shader object to src, it can then be compiled!
		GLCall(glShaderSource(id, 1, &src, nullptr));
		// Compiles the source code strings that have been stored in the shader object specified by id
		GLCall(glCompileShader(id));

		// Error handling
		int result;
		// Generally: returns a parameter of a shader object.
		// paramater name from [GL_SHADER_TYPE, GL_DELETE_STATUS, GL_COMPILE_STATUS, GL_INFO_LOG_LENGTH, GL_SHADER_SOURCE_LENGTH.]
		GLCall(glGetShaderiv(id, GL_COMPILE_STATUS, &result));
		if (result == GL_FALSE)
		{
			int length;
			// returns the length of the error message
			GLCall(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));
			// Hack to allocate memory on the stack
			char* message = (char*)_malloca(length * sizeof(char));
			// returns the error message
			GLCall(glGetShaderInfoLog(id, length, &length, message));
			// Print error message to console!
			switch (type)
			{
			case GL_VERTEX_SHADER:
				errorLogger << LOGGER::BEGIN << "Failed to compile vertex shader!" << LOGGER::ENDL;
				break;
			case GL_FRAGMENT_SHADER:
				errorLogger << LOGGER::BEGIN << "Failed to compile fragment shader!" << LOGGER::ENDL;
				break;
			default:
				errorLogger << LOGGER::BEGIN << "Failed to compile shader of unknown type!" << LOGGER::ENDL;
				break;
			}
			errorLogger << LOGGER::BEGIN << message << LOGGER::ENDL;
			// delete shader in case there is an error!
			glDeleteShader(0);
			return {};
		}
		// return the id of the compiled shader!
		return id;
	}
	//------------------------------------------------------------------------------------------------------
	/// Compiles a shader consisting of vertex and fragment shader and returns the program ID (=GPU_ID)
	/// of the new shader. Returns an empty optional if something went wrong
	std::optional<unsigned int> createShader(const std::string& vertexShader, const std::string& fragmentShader)
	{
		// Create a shader, save the ID in program.
		GLCall(unsigned int program = glCreateProgram());
		// Compile both the fragment and the vertex shader!
		auto vs = compileShader(GL_VERTEX_SHADER, vertexShader);
		if (!vs) {
			warningLogger << LOGGER::BEGIN << "Failed to compile vertex shader!" << LOGGER::ENDL;
			return {};
		}
		auto fs = compileShader(GL_FRAGMENT_SHADER, fragmentShader);
		if (!fs) {
			warningLogger << LOGGER::BEGIN << "Failed to compile fragment shader!" << LOGGER::ENDL;
			return {};
		}

		// attach shader objects to program object
		GLCall(glAttachShader(program, vs.value()));
		GLCall(glAttachShader(program, fs.value()));
		// links the program object
		GLCall(glLinkProgram(program));
		// checks to see whether the executables contained in program can execute given the current OpenGL state
		GLCall(glValidateProgram(program));

		// now, the shaders can be deleted. They are just an "intermediate" product of the shader program creation!
		GLCall(glDeleteShader(vs.value()));
		GLCall(glDeleteShader(fs.value()));

		return program;
	}
	//------------------------------------------------------------------------------------------------------
	/// Struct storing the source code for the vertex and fragment shader
	struct ShaderProgramSource
	{
		std::string vertexSource;
		std::string fragmentSource;
	};
	//------------------------------------------------------------------------------------------------------
	/// Parses the shader from the given filepath and returns a ShaderProgramSource struct
	std::optional<ShaderProgramSource> parseShader(const std::string& filepath)
	{
		// load the file
		std::ifstream stream(filepath);

		if (!stream.is_open())
		{
			warningLogger << LOGGER::BEGIN << "Couldn't find file \"" << filepath << "\"" << LOGGER::ENDL;
			return {};
		}

		// enum to use type in array (more clever than switch statement)
		enum class ShaderType
		{
			NONE = -1, VERTEX = 0, FRAGMENT = 1
		};

		std::string line;
		std::stringstream ss[2];
		ShaderType type = ShaderType::NONE;
		while (std::getline(stream, line))
		{
			// switch shader type if the lane #shader [shaderType] is found!
			if (line.find("#shader") != std::string::npos)
			{
				if (line.find("vertex") != std::string::npos)
				{
					type = ShaderType::VERTEX;
				}
				else if (line.find("fragment") != std::string::npos)
				{
					type = ShaderType::FRAGMENT;
				}
			}
			// else, add the line to the source code of the current shader type
			else
			{
				if (type == ShaderType::NONE) {
					warningLogger << LOGGER::BEGIN << "Shader type not specified!" << LOGGER::ENDL;
					return {};
				}
				ss[(int)type] << line << "\n";
			}
		}
		// return a ShaderProgramSource struct.
		return ShaderProgramSource{ ss[0].str(), ss[1].str() };
	}
	//------------------------------------------------------------------------------------------------------
	// Wraps the above functions into a single function that loads and compiles a shader.
	// Returns an empty optional if something goes wrong
	std::optional<unsigned int> loadAndCompileShader(const std::string& path)
	{
		std::optional<std::string> fullPath = Engine::getFullPath(path);
		if (!fullPath.has_value())
			return {};
		auto source = parseShader(fullPath.value());
		if (!source)
			return {};

		return createShader(source.value().vertexSource, source.value().fragmentSource);
	}
	//------------------------------------------------------------------------------------------------------
	int ShaderManager::ShaderData::getUniformLocation(const std::string& name) const
	{
		GLCall(int location = glGetUniformLocation(GPU_ID, name.c_str()));
#ifdef _DEBUG
		if (location == -1)
		{
			warningLogger << LOGGER::BEGIN << "Uniform \"" << name << "\" doesn't exist!" << LOGGER::ENDL;
		}
#endif // _DEBUG
		return location;
	}
	//======================================================================================================
	// ShaderManager implementation
	//======================================================================================================
	void ShaderManager::deleteShader(const ShaderID& shaderID)
	{
#ifdef _DEBUG
		if (shaderID > maxShaders) {
			warningLogger << LOGGER::BEGIN << "tried to delete shader that does not exist ..." << LOGGER::ENDL;
			return;
		}
		else if (!shaderDataArray[shaderID].valid) {
			warningLogger << LOGGER::BEGIN << "tried to delete shader that was not valid ..." << LOGGER::ENDL;
			return;
		}
#endif // _DEBUG

		// Delete entries in path map
		stringToShaderID.erase(shaderDataArray[shaderID].path);
		// Reset and unload Shader
		shaderDataArray[shaderID] = ShaderData();
		loadedShadersCount--;
		availableShaderIDs.push(shaderID);
	}
	//------------------------------------------------------------------------------------------------------
	ShaderManager::ShaderID ShaderManager::getNewShaderID()
	{
		if (loadedShadersCount >= maxShaders)
		{
			// Resize vector and add new available shaderID slots accordingly. For now: double size everytime this happens and send warning!
			shaderDataArray.resize(maxShaders * 2 + 1);
			for (ShaderID id = maxShaders + 1; id <= 2 * maxShaders; ++id)
			{
				availableShaderIDs.push(id);
			}
			warningLogger << LOGGER::BEGIN << "maximum amount of shaders exceeded. Resizing shaderManager to be able to store "
				<< 2 * maxShaders << " shaders!" << LOGGER::ENDL;
			maxShaders *= 2;
		}
		// Take ID from the front of the queue
		ShaderID id = availableShaderIDs.front();
		availableShaderIDs.pop();
		++loadedShadersCount;
		return id;
	}
	//------------------------------------------------------------------------------------------------------
	void ShaderManager::initialize(const unsigned int& startingSize)
	{
		// Initializes queue with all possible shaderIDs. shaderID = 0 is reserved for invalid shaders.
		for (ShaderID id = 1; id <= startingSize; ++id)
		{
			availableShaderIDs.push(id);
		}
		// shaderDataArray[0] stores the default shader
		shaderDataArray.resize(startingSize + 1);
		maxShaders = startingSize;
		// Load default shader!
		auto GPU_ID = loadAndCompileShader(std::string(defaultShaderPath));
		if (!GPU_ID) {
			errorLogger << LOGGER::BEGIN << "Could not load default shader" << LOGGER::ENDL;
		}
		else {
			ShaderData& shaderData = shaderDataArray[0];
			shaderData.GPU_ID = GPU_ID.value();
			shaderData.referenceCount = 0;
			shaderData.persistent = true;
			shaderData.valid = true;
			shaderData.path = "";
		}
	}
	//------------------------------------------------------------------------------------------------------
	void ShaderManager::increaseReferenceCount(const Shader& shader)
	{
		if (shader.shaderID == 0)
			return;
#ifdef _DEBUG
		if (shader.shaderID > maxShaders) {
			warningLogger << LOGGER::BEGIN << "tried to increase reference count of shader that does not exist ..." << LOGGER::ENDL;
			return;
		}
		else if (!shaderDataArray[shader.shaderID].valid) {
			warningLogger << LOGGER::BEGIN << "tried to increase reference count of shader that was not valid ..." << LOGGER::ENDL;
			return;
		}
#endif // _DEBUG
		shaderDataArray[shader.shaderID].referenceCount++;
	}
	//------------------------------------------------------------------------------------------------------
	void ShaderManager::decreaseReferenceCount(const Shader& shader)
	{
		if (shader.shaderID == 0)
			return;
#ifdef _DEBUG
		if (shader.shaderID > maxShaders) {
			warningLogger << LOGGER::BEGIN << "tried to decrease reference count of shader that does not exist ..." << LOGGER::ENDL;
			return;
		}
		else if (!shaderDataArray[shader.shaderID].valid) {
			warningLogger << LOGGER::BEGIN << "tried to decrease reference count of shader that was not valid ..." << LOGGER::ENDL;
			return;
		}
#endif // _DEBUG
		shaderDataArray[shader.shaderID].referenceCount--;
		if (shaderDataArray[shader.shaderID].referenceCount == 0 && !shaderDataArray[shader.shaderID].persistent) {
			deleteShader(shader.shaderID);
		}
	}
	//------------------------------------------------------------------------------------------------------
	void ShaderManager::bind(const Shader& shader)
	{
#ifdef _DEBUG
		if (shader.shaderID > maxShaders) {
			warningLogger << LOGGER::BEGIN << "tried to bind shader that does not exist ..." << LOGGER::ENDL;
			return;
		}
		else if (!shaderDataArray[shader.shaderID].valid) {
			warningLogger << LOGGER::BEGIN << "tried to bind shader that was not valid ..." << LOGGER::ENDL;
			return;
		}
#endif // _DEBUG
		shaderDataArray[shader.shaderID].bind();
	}
	//------------------------------------------------------------------------------------------------------
	ShaderManager::ShaderID ShaderManager::loadShader(const std::string& path)
	{
		// First look if shader was already valid
		auto it = stringToShaderID.find(path);
		if (it != stringToShaderID.end()) {
			return it->second;
		}
		// load and compile new shader
		auto GPU_ID = loadAndCompileShader(path);
		if (!GPU_ID)
			return defaultShader;
		ShaderID shaderID = getNewShaderID();
		ShaderData& shaderData = shaderDataArray[shaderID];
		shaderData.GPU_ID = GPU_ID.value();
		shaderData.referenceCount = 0;
		shaderData.persistent = false;
		shaderData.valid = true;
		shaderData.path = path;
		stringToShaderID.insert({ path, shaderID });
		return shaderID;
	}
	//------------------------------------------------------------------------------------------------------
	void ShaderManager::terminate()
	{
		/// Clears the shaderDataArray, deleting all shaders from the GPU!
		shaderDataArray.clear();
	}
	//------------------------------------------------------------------------------------------------------
	template<>
	void SnackerEngine::ShaderManager::setUniform<float>(const Shader& shader, const std::string& name, const float& data)
	{
		GLCall(glUniform1f(shaderDataArray[shader.shaderID].getUniformLocation(name), data));
	}
	//------------------------------------------------------------------------------------------------------
	template<>
	void SnackerEngine::ShaderManager::setUniform<Vec2f>(const Shader& shader, const std::string& name, const Vec2f& data)
	{
		GLCall(glUniform2f(shaderDataArray[shader.shaderID].getUniformLocation(name), data.x, data.y));
	}
	//------------------------------------------------------------------------------------------------------
	template<>
	void SnackerEngine::ShaderManager::setUniform<Vec3f>(const Shader& shader, const std::string& name, const Vec3f& data)
	{
		GLCall(glUniform3f(shaderDataArray[shader.shaderID].getUniformLocation(name), data.x, data.y, data.z));
	}
	//------------------------------------------------------------------------------------------------------
	template<>
	void SnackerEngine::ShaderManager::setUniform<Vec4f>(const Shader& shader, const std::string& name, const Vec4f& data)
	{
		GLCall(glUniform4f(shaderDataArray[shader.shaderID].getUniformLocation(name), data.x, data.y, data.z, data.w));
	}
	//------------------------------------------------------------------------------------------------------
	template<>
	void SnackerEngine::ShaderManager::setUniform<int>(const Shader& shader, const std::string& name, const int& data)
	{
		GLCall(glUniform1i(shaderDataArray[shader.shaderID].getUniformLocation(name), data));
	}
	//------------------------------------------------------------------------------------------------------
	template<>
	void SnackerEngine::ShaderManager::setUniform<Mat3f>(const Shader& shader, const std::string& name, const Mat3f& data)
	{
		GLCall(glUniformMatrix3fv(shaderDataArray[shader.shaderID].getUniformLocation(name), 1, GL_TRUE, &(data.data[0])));
	}
	//------------------------------------------------------------------------------------------------------
	template<>
	void SnackerEngine::ShaderManager::setUniform<Mat4f>(const Shader& shader, const std::string& name, const Mat4f& data)
	{
		GLCall(glUniformMatrix4fv(shaderDataArray[shader.shaderID].getUniformLocation(name), 1, GL_TRUE, &(data.data[0])));
	}
	//------------------------------------------------------------------------------------------------------
	template<>
	void SnackerEngine::ShaderManager::setUniform<Color3f>(const Shader& shader, const std::string& name, const Color3f& data)
	{
		GLCall(glUniform3f(shaderDataArray[shader.shaderID].getUniformLocation(name), data.r, data.g, data.b));
	}
	//------------------------------------------------------------------------------------------------------
	template<>
	void SnackerEngine::ShaderManager::setUniform<Color4f>(const Shader& shader, const std::string& name, const Color4f& data)
	{
		GLCall(glUniform4f(shaderDataArray[shader.shaderID].getUniformLocation(name), data.r, data.g, data.b, data.alpha));
	}
	//------------------------------------------------------------------------------------------------------
	template<>
	void SnackerEngine::ShaderManager::setUniform<Vec2i>(const Shader& shader, const std::string& name, const Vec2i& data)
	{
		GLCall(glUniform2i(shaderDataArray[shader.shaderID].getUniformLocation(name), data.x, data.y));
	}
	//------------------------------------------------------------------------------------------------------
	template<>
	void SnackerEngine::ShaderManager::setUniform<Vec3i>(const Shader& shader, const std::string& name, const Vec3i& data)
	{
		GLCall(glUniform3i(shaderDataArray[shader.shaderID].getUniformLocation(name), data.x, data.y, data.z));
	}
	//------------------------------------------------------------------------------------------------------
	template<>
	void SnackerEngine::ShaderManager::setUniform<Vec4i>(const Shader& shader, const std::string& name, const Vec4i& data)
	{
		GLCall(glUniform4i(shaderDataArray[shader.shaderID].getUniformLocation(name), data.x, data.y, data.z, data.w));
	}
	//------------------------------------------------------------------------------------------------------
	void ShaderManager::setModelViewProjection(const Shader& shader, const Mat4f& model, const Mat4f& view, const Mat4f& projection)
	{
		setUniform<Mat4f>(shader, "u_model", model);
		setUniform<Mat4f>(shader, "u_view", view);
		setUniform<Mat4f>(shader, "u_projection", projection);
	}
	//------------------------------------------------------------------------------------------------------
	const ShaderManager::ShaderData& ShaderManager::getShaderData(const Shader& shader)
	{
		return shaderDataArray[shader.shaderID];
	}
	//------------------------------------------------------------------------------------------------------
}