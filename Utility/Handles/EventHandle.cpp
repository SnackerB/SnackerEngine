#include "Handles/EventHandle.h"

namespace SnackerEngine
{

	void EventHandle::Observable::notifyHandleMove(EventHandle* old, EventHandle& eventHandle)
	{
		if (eventHandles.erase(old) > 0) {
			eventHandles.insert(&eventHandle);
		}
	}

	void EventHandle::Observable::subscribe(EventHandle& eventHandle)
	{
		eventHandles.insert(&eventHandle);
		eventHandle.observables.insert(this);
	}

	void EventHandle::Observable::unsubscribe(EventHandle& eventHandle)
	{
		eventHandles.erase(&eventHandle);
		eventHandle.observables.erase(this);
	}

	void EventHandle::Observable::trigger()
	{
		for (auto& eventHandle : eventHandles) {
			eventHandle->notifyEvent();
		}
	}

	EventHandle::Observable::Observable(Observable&& other) noexcept
		: eventHandles(std::move(other.eventHandles))
	{
		for (auto& eventHandle : eventHandles) {
			eventHandle->observables.erase(&other);
			eventHandle->observables.insert(this);
		}
		other.eventHandles.clear();
	}

	EventHandle::Observable& EventHandle::Observable::operator=(Observable&& other) noexcept
	{
		for (auto& eventHandle : eventHandles) {
			eventHandle->observables.erase(this);
		}
		eventHandles = std::move(other.eventHandles);
		for (auto& eventHandle : eventHandles) {
			eventHandle->observables.erase(&other);
			eventHandle->observables.insert(this);
		}
		other.eventHandles.clear();
		return *this;
	}

	EventHandle::Observable::~Observable()
	{
		for (auto& eventHandle : eventHandles) {
			eventHandle->observables.erase(this);
		}
		eventHandles.clear();
	}

	void EventHandle::notifyEvent()
	{
		active = true;
		onEvent();
	}

	void EventHandle::unsubscribeFromAll()
	{
		for (auto& observable : observables) {
			observable->eventHandles.erase(this);
		}
		observables.clear();
	}

	EventHandle::EventHandle(const EventHandle& other) noexcept
		: observables{}, active{false}
	{
		for (auto& observable : other.observables) {
			observable->subscribe(*this);
		}
	}
	
	EventHandle& EventHandle::operator=(const EventHandle& other) noexcept
	{
		unsubscribeFromAll();
		active = false;
		for (auto& observable : other.observables) {
			observable->subscribe(*this);
		}
		return *this;
	}
	
	EventHandle::EventHandle(EventHandle&& other) noexcept
		: observables(std::move(other.observables)), active(other.active)
	{
		for (auto& observable : observables) {
			observable->notifyHandleMove(&other, *this);
		}
		other.observables.clear();
	}
	
	EventHandle& EventHandle::operator=(EventHandle&& other) noexcept
	{
		unsubscribeFromAll();
		observables = std::move(other.observables);
		active = other.active;
		for (auto& observable : observables) {
			observable->notifyHandleMove(&other, *this);
		}
		other.observables.clear();
		return *this;
	}

	EventHandle::~EventHandle()
	{
		unsubscribeFromAll();
	}
	
}