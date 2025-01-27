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
                            WeakComponentBase(ReflectionDescriptor* componentDescriptor, const std::shared_ptr<Component>& pointer);
        virtual             ~WeakComponentBase();

        WeakComponentBase&   operator = (const WeakComponentBase& copy);
        WeakComponentBase&   operator = (const std::weak_ptr<Component>& pointer);

        bool                operator==(const WeakComponentBase& other) const;

    public:

        std::shared_ptr<Component>  Lock() const;
        uint64_t                    GetInstanceId() const;
        uint64_t                    GetForSerialization() const;

        ReflectionDescriptor*       GetComponentDescriptor() const;

        void                        SetInstanceId(uint64_t instanceId);
        void                        SetPointer(std::shared_ptr<Component> pointer);

    private:

        ReflectionDescriptor*           _componentDescriptor = nullptr;

        uint64_t                           _instanceId = 0;
        mutable std::weak_ptr<Component>   _pointer;
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
        WeakComponent(const std::shared_ptr<_Component_>& pointer)
        : WeakComponentBase(_Component_::GetReflectionDescriptor(), pointer)
        {
        }

        /// @brief 
        ~WeakComponent()
        {
        }

        /// @brief 
        /// @return 
        std::shared_ptr<_Component_> Lock() const
        {
            return std::static_pointer_cast<_Component_>(WeakComponentBase::Lock());
        }

        void operator = (std::shared_ptr<_Component_> pointer)
        {
            SetPointer(pointer);
        }
    };
}
