#pragma once

#include <vector>

namespace SnackerEngine {
	//------------------------------------------------------------------------------------------------------
	class IndexBuffer {
		/// This is set to true if this IndexBuffer object is valid and corresponds to data on the GPU
		bool valid;
		/// GPU ID of this IndexBuffer object
		unsigned int GPU_ID;
		/// Counts how many indices are stored
		unsigned int indexCount;
	public:
		/// Standard constructor
		IndexBuffer();
		/// Constructor with defined indices
		IndexBuffer(const std::vector<unsigned int>& indices);
		// Move constructor
		IndexBuffer(IndexBuffer&& ib) noexcept;
		// Deleted copy constructor 
		IndexBuffer(IndexBuffer& ib) = delete; 
		// Move assignment operator
		IndexBuffer& operator=(IndexBuffer&& ib) noexcept;
		// Deleted copy assignment operator 
		IndexBuffer& operator=(IndexBuffer& ib) = delete;
		// Destructor
		~IndexBuffer();
		/// Binds this IndexBuffer object
		void bind() const;
		/// Unbinds all IndexBuffer objects
		static void unBind();
		/// Sets the indices of this buffer
		void setIndices(const std::vector<unsigned int>& indices);
		/// Checks if this IndexBuffer object is valid and corresponds to data on the GPU
		bool isValid() { return valid; };
		/// Returns the number of indices stored in this IndexBuffer object
		unsigned int getCount() const { return indexCount; };
	};
	//------------------------------------------------------------------------------------------------------
}