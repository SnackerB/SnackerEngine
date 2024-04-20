#pragma once

#include <set>

namespace SnackerEngine
{

	/// Variable handles are two-way handles, if the value of one handle is changed, all
	/// variable handles subscribed to this handle are notified. Each variable handle
	/// posesses a local copy of the variable.
	template<typename T>
	class VariableHandle
	{
	protected:
		/// The value stored in this handle
		T value;
		/// variable handles that are connected to this handle
		std::set<VariableHandle<T>*> connectedHandles;
		/// this is set to true if an event occurred and can be reset by calling reset()
		bool active = false;
		/// Helper function that is called by connected variableHandles to notify a change in variable
		void notifyVariableChanged(const T& newValue, VariableHandle<T>* sourceHandle);
		/// Helper function that is called by connected variableHandles to announce that they have moved
		void notifyHandleMove(VariableHandle<T>* newPosition, VariableHandle<T>* oldPosition);
		/// Function that gets called when an event happens.
		/// Important: do not change the variable in this function again, as
		/// this can lead to infinite loops!
		virtual void onEvent() {}
	protected:
		/// Helper function that is called by connected variableHandles to notify a change in variable
		void notifyVariableChanged(const T& newValue, VariableHandle<T>* sourceHandle, VariableHandle<T>* destinationHandle);
		/// Helper function that can be called by derived classes to notify all connected variableHandles of a change
		void notifyAllConnectedHandles();
	public:
		/// Constructor
		VariableHandle()
			: value{}, connectedHandles{} {}
		VariableHandle(const T& value)
			: value{ value }, connectedHandles{} {}
		/// Disconnects this handle from the given handle (and vice versa)
		void disconnect(VariableHandle& other);
		/// Disconnect from all handles
		void disconnectFromAll();
		/// Copy constructor and assignment operator
		VariableHandle(const VariableHandle& other) noexcept;
		VariableHandle& operator=(const VariableHandle& other) noexcept;
		/// Move constructor and assignment operator
		VariableHandle(VariableHandle&& other) noexcept;
		VariableHandle& operator=(VariableHandle&& other) noexcept;
		/// Destructor
		~VariableHandle();
		/// Connects this handle with another handle. Returns true on success
		void connect(VariableHandle<T>& other);
		/// Returns true if this handle is currently connected to at least one other handle
		bool isValid() const { return !connectedHandles.empty(); }
		/// Getters
		bool isActive() const { return active; }
		/// Resets the handle
		void reset() { active = false; }
		/// Returns the current value
		const T& get() const { return value; }
		/// Sets the value and notifies all connected handles
		void set(const T& value);
		/// Overloading assignments, casts and arithmetic operations
		operator const T& () const { return get(); }
		VariableHandle& operator=(const T& value) { this->set(value); return *this; }
	};

	template<typename T>
	inline void VariableHandle<T>::notifyVariableChanged(const T& newValue, VariableHandle<T>* sourceHandle)
	{
		if (value != newValue) {
			value = newValue;
			active = true;
			onEvent();
			for (auto handle : connectedHandles) {
				if (handle != sourceHandle) handle->notifyVariableChanged(newValue, this);
			}
		}
	}

	template<typename T>
	inline void VariableHandle<T>::notifyVariableChanged(const T& newValue, VariableHandle<T>* sourceHandle, VariableHandle<T>* destinationHandle)
	{
		destinationHandle->notifyVariableChanged(newValue, sourceHandle);
	}

	template<typename T>
	inline void VariableHandle<T>::notifyAllConnectedHandles()
	{
		for (auto handle : connectedHandles) handle->notifyVariableChanged(value, this);
	}

	template<typename T>
	inline void VariableHandle<T>::notifyHandleMove(VariableHandle<T>* newPosition, VariableHandle<T>* oldPosition)
	{
		connectedHandles.erase(oldPosition);
		connectedHandles.insert(newPosition);
	}

	template<typename T>
	inline void VariableHandle<T>::disconnect(VariableHandle& other)
	{
		connectedHandles.erase(&other);
		other.connectedHandles.erase(this);
	}

	template<typename T>
	inline void VariableHandle<T>::disconnectFromAll()
	{
		for (auto& handle : connectedHandles) {
			handle->connectedHandles.erase(this);
		}
		connectedHandles.clear();
	}

	template<typename T>
	inline VariableHandle<T>::VariableHandle(const VariableHandle& other) noexcept
		: value(other.value), connectedHandles{}
	{
	}

	template<typename T>
	inline VariableHandle<T>& VariableHandle<T>::operator=(const VariableHandle& other) noexcept
	{
		disconnectFromAll();
		value = other.value;
		return *this;
	}

	template<typename T>
	inline VariableHandle<T>::VariableHandle(VariableHandle&& other) noexcept
		: value(std::move(other.value)), connectedHandles(std::move(other.connectedHandles))
	{
		for (auto& handle : connectedHandles) handle->notifyHandleMove(this, &other);
		other.connectedHandles.clear();
	}

	template<typename T>
	inline VariableHandle<T>& VariableHandle<T>::operator=(VariableHandle&& other) noexcept
	{
		disconnectFromAll();
		value = std::move(other.value);
		connectedHandles = std::move(other.connectedHandles);
		for (auto& handle : connectedHandles) handle->notifyHandleMove(this, &other);
		other.connectedHandles.clear();
		return *this;
	}

	template<typename T>
	inline VariableHandle<T>::~VariableHandle()
	{
		disconnectFromAll();
	}

	template<typename T>
	inline void VariableHandle<T>::connect(VariableHandle<T>& other)
	{
		if (this != &other) {
			connectedHandles.insert(&other);
			other.connectedHandles.insert(this);
			if (this->value != other.value) {
				notifyVariableChanged(other.value, &other);
			}
		}
	}

	template<typename T>
	inline void VariableHandle<T>::set(const T& value)
	{
		if (this->value != value) {
			this->value = value;
			notifyAllConnectedHandles();
		}
	}

}