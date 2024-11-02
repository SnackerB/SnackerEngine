#pragma once

#include <set>

namespace SnackerEngine
{

	class EventHandle
	{
	public:
		/// Class representing an object that can be "observed" by handles
		class Observable
		{
		private:
			/// Set of pointers to all event handles that subscribed to this observable
			std::set<EventHandle*> eventHandles;
		protected:
			friend class EventHandle;
			/// Notifies the observable that the given eventHandle has moved
			void notifyHandleMove(EventHandle* old, EventHandle& eventHandle);
		public:
			/// Constructor
			Observable()
				: eventHandles{} {}
			/// subscribes the given eventHandle to this observable
			void subscribe(EventHandle& eventHandle);
			/// unsubscribes the given eventHandle from this observable
			void unsubscribe(EventHandle& eventHandle);
			/// Triggers an event and notifies all subscribed eventHandles
			void trigger();
			/// deleted copy constructor and assignment operator
			Observable(const Observable& other) = delete;
			Observable& operator=(const Observable& other) = delete;
			/// Move constructor and assignment operator
			Observable(Observable&& other) noexcept;
			Observable& operator=(Observable&& other) noexcept;
			/// Destructor
			~Observable();
		};
	private:
		/// Set of observables that this handle is subscribed to
		std::set<Observable*> observables{};
		/// this is set to true if an event occurred and can be reset by calling reset()
		bool active = false;
		/// Helper function that is called by observables to notify the handle an
		/// event has occured
		void notifyEvent();
	protected:
		/// Function that gets called when an event happens.
		virtual void onEvent() {}
	public:
		/// Constructor
		EventHandle()
			: observables{}, active{ false } {}
		/// Unsubscribes from all observables
		void unsubscribeFromAll();
		/// Copy constructor and assignment operator
		EventHandle(const EventHandle& other) noexcept;
		EventHandle& operator=(const EventHandle& other) noexcept;
		/// Move constructor and assignment operator
		EventHandle(EventHandle&& other) noexcept;
		EventHandle& operator=(EventHandle&& other) noexcept;
		/// Destructor
		~EventHandle();
		/// Getters
		bool isActive() const { return active; }
		/// Resets the handle
		void reset() { active = false; }
	};

}