#pragma once

#include "Graphics/VertexBuffer.h"

#include <vector>

namespace SnackerEngine
{
	//------------------------------------------------------------------------------------------------------
	class VertexBuffer {
	public:
		/// The storage type is a hint to GLFW how the storage will most likely be used
		enum class VertexBufferStorageType
		{
			STATIC,		/// The data store contents will be modified once and used many times.
			DYNAMIC		/// The data store contents will be modified repeatedly and used many times.
		};
	private:
		/// This is set to true if this VertexBuffer object is valid and corresponds to data on the GPU
		bool valid; 
		/// GPU ID of this VertexBuffer object
		unsigned int GPU_ID;
		/// Storage type
		VertexBufferStorageType storageType;
		// Size of the data in bytes
		unsigned int sizeInBytes; 
		/// Generates the buffer, Sets the data, and moves it to the GPU, finalizing this VertexBuffer object.
		/// data:	pointer to the data
		/// size:	size in bytes
		void setDataAndFinalize(void* data, const unsigned int& size);
		/// Changes a part of the data on the GPU. Can only be done when the VertexBuffer object is already valid.
		/// data:	pointer to the data
		/// size:	size in bytes
		/// offset:	offset to the start of the data in bytes
		void changeSubData(void* data, const unsigned int& size, const unsigned int& offset);
	public:
		/// Constructor with a given storage type
		explicit VertexBuffer(const VertexBufferStorageType storageType);
		/// Constructor using the default storage type: STATIC
		VertexBuffer();
		// Move constructor
		VertexBuffer(VertexBuffer&& vb) noexcept;
		// Deleted copy constructor
		VertexBuffer(VertexBuffer& vb) = delete;
		// Move assignment operator
		VertexBuffer& operator=(VertexBuffer&& vb) noexcept;
		// Deleted copy assignment operator
		VertexBuffer& operator=(VertexBuffer& vb) = delete;
		/// Destructor
		~VertexBuffer();
		/// Binds this VertexBuffer object
		void bind();
		/// Unbinds all VertexBuffer objects
		static void unBind();
		/// Changes the storage type of this VertexBuffer object. Can only be done before finalizing the VertexBuffer object
		void setStorageType(VertexBufferStorageType storageType);

		/// Generates the buffer, Sets the data, and moves it to the GPU, finalizing this VertexBuffer object.
		template<typename T>
		void setDataAndFinalize(const std::vector<T>& data) {
			// We need to do a const cast to get rid of const modifier for GLFW. 
			// However, the data is NOT modified!
			setDataAndFinalize(const_cast<void*>(static_cast<const void*>(data.data())), static_cast<unsigned int>(data.size()) * sizeof(T));
		};

		/// Changes a part of the data on the GPU. Can only be done when the VertexBuffer object is already valid.
		template<typename T>
		void changeSubData(const std::vector<T> data, const unsigned int offset) {
			// We need to do a const cast to get rid of const modifier for GLFW. 
			// However, the data is NOT modified!
			changeSubData(const_cast<void*>(static_cast<const void*>(data.data())), data.size() * sizeof(T), offset);
		};

		/// Checks if this IndexBuffer object is valid and corresponds to data on the GPU
		bool isValid() { return valid; };
		/// returns the used storage type
		VertexBufferStorageType getStorageType() { return storageType; };
		/// Returns the size of the stored data in bytes
		unsigned int getSizeInBytes() { return sizeInBytes; };
	};
	//------------------------------------------------------------------------------------------------------
}