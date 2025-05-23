#pragma once
#include "HodEngine/Game/Export.hpp"
#include <memory>
#include <unordered_map>

#include <HodEngine/Core/UID.hpp>
#include <HodEngine/Core/Reflection/ReflectionMacros.hpp>
#include <HodEngine/Core/Reflection/Traits/ReflectionTraitGetValueForSerialization.hpp>

#include "HodEngine/Game/Component.hpp"

namespace hod::game
{
    class HOD_GAME_API WeakComponentBase
    {
        REFLECTED_CLASS_NO_VIRTUAL(WeakComponentBase)

        //DESCRIBE_REFLECTED_CLASS_NO_PARENT(WeakComponent)

    public:

        public:

                            WeakComponentBase(ReflectionDescriptor* componentDescriptor);
                            WeakComponentBase(ReflectionDescriptor* componentDescriptor, Component* pointer);
        virtual             ~WeakComponentBase();

        WeakComponentBase&   operator = (const WeakComponentBase& copy);
        WeakComponentBase&   operator = (Component* pointer);

        bool                 operator==(const WeakComponentBase& other) const;

    public:

        Component*                  Lock() const;
        uint64_t                    GetInstanceId() const;
        uint64_t                    GetForSerialization() const;

        ReflectionDescriptor*       GetComponentDescriptor() const;

        void                        SetInstanceId(uint64_t instanceId);
        void                        SetPointer(Component* pointer);

    private:

        ReflectionDescriptor*           _componentDescriptor = nullptr;

        uint64_t                        _instanceId = 0;
        mutable WeakPtr<Component>      _pointer;
    };

    /// @brief 
    /// @tparam _Component_ 
    template<typename _Component_>
    class WeakComponent : public WeakComponentBase
    {
    public:

        WeakComponent()
        : WeakComponentBase(&_Component_::GetReflectionDescriptor())
        {
        }

        /// @brief 
        /// @param pointer 
        WeakComponent(_Component_* pointer)
        : WeakComponentBase(&_Component_::GetReflectionDescriptor(), pointer)
        {
        }

        /// @brief 
        ~WeakComponent()
        {
        }

        /// @brief 
        /// @return 
        _Component_* Lock() const
        {
            return static_cast<_Component_*>(WeakComponentBase::Lock());
        }

        void operator = (std::shared_ptr<_Component_> pointer)
        {
            SetPointer(pointer);
        }
    };
}
