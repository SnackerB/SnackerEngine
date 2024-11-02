#include "VectorEventHandle.h"

namespace SnackerEngine
{

    VectorEventHandle::VectorEventHandle(std::size_t count)
        : active(false), childHandles{}
    {
        for (std::size_t i = 0; i < count; ++i) childHandles.push_back(ChildEventHandle(this));
    }

    VectorEventHandle::VectorEventHandle(const VectorEventHandle& other) noexcept
        : VectorEventHandle(other.size()) {}

    VectorEventHandle& VectorEventHandle::operator=(const VectorEventHandle& other) noexcept
    {
        active = false;
        childHandles.clear();
        for (std::size_t i = 0; i < other.size(); ++i) childHandles.push_back(ChildEventHandle(this));
        return *this;
    }

    VectorEventHandle::VectorEventHandle(VectorEventHandle&& other) noexcept
        : active(other.active), childHandles(std::move(other.childHandles))
    {
        for (auto& childHandle : childHandles) childHandle.parentHandle = this;
    }

    VectorEventHandle& VectorEventHandle::operator=(VectorEventHandle&& other) noexcept
    {
        active = other.active;
        childHandles = std::move(other.childHandles);
        for (auto& childHandle : childHandles) childHandle.parentHandle = this;
        return *this;
    }

    void VectorEventHandle::reset()
    {
        active = false;
        for (auto& childHandle : childHandles) childHandle.reset();
    }

    void VectorEventHandle::resize(std::size_t size)
    {
        std::size_t oldSize = childHandles.size();
        childHandles.resize(size);
        for (std::size_t i = oldSize; i < size; ++i) {
            childHandles[i].parentHandle = this;
        }
    }

}