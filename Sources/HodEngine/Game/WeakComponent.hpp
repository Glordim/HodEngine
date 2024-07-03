#pragma once
#include <HodEngine/HodEngine.hpp>
#include <memory>

#include <HodEngine/Core/UID.hpp>
#include <HodEngine/Core/Reflection/ReflectionMacros.hpp>
#include <HodEngine/Core/Reflection/Traits/ReflectionTraitGetValueForSerialization.hpp>

namespace hod::game
{
    class Component;

    /// @brief 
    class HOD_API WeakComponentMapping
    {
    public:

        static void    Insert(const UID& uid, std::weak_ptr<Component> component)
        {
            _map[uid] = component;
        }

        static void    Clear()
        {
            _map.clear();
        };

        template<typename _Component_>
        static std::weak_ptr<_Component_> Resolve(const UID& uid)
        {
            auto it = _map.find(uid);
            if (it == _map.end())
            {
                return std::weak_ptr<_Component_>();
            }
            else
            {
                return std::weak_ptr<_Component_>(std::static_pointer_cast<_Component_>(it->second.lock()));//std::static_pointer_cast<_Component_>(it->second);
            }
        }

    private:

        static std::map<UID, std::weak_ptr<Component>> _map;
    };

    /// @brief 
    /// @tparam _Component_ 
    template<typename _Component_>
    class WeakComponent
    {
        REFLECTED_CLASS_NO_VIRTUAL(WeakComponent)

        //DESCRIBE_REFLECTED_CLASS_NO_PARENT(WeakComponent)

    public:

        /// @brief 
        /// @param pointer 
        WeakComponent()
        {
            _uid = UID::INVALID_UID;
        }

        /// @brief 
        /// @param pointer 
        WeakComponent(const std::weak_ptr<_Component_>& pointer)
        {
            _pointer = pointer;
        }

        /// @brief 
        /// @param pointer 
        WeakComponent(const std::shared_ptr<_Component_>& pointer)
        {
            _pointer = pointer;
        }

        /// @brief 
        ~WeakComponent()
        {
        }

        /// @brief 
        /// @param pointer 
        WeakComponent& operator = (const WeakComponent& copy)
        {
            _uid = copy._uid;
            _pointer = copy._pointer;
            return *this;
        }

        /// @brief 
        /// @param pointer 
        WeakComponent& operator = (const std::weak_ptr<_Component_>& pointer)
        {
            _pointer = pointer;
            return *this;
        }

        /// @brief 
        /// @return 
        std::shared_ptr<_Component_> Lock() const
        {
            std::shared_ptr<_Component_> lock = _pointer.lock();
            if (lock == nullptr)
            {
                _pointer = WeakComponentMapping::Resolve<_Component_>(_uid);
                lock = _pointer.lock();
            }
            return lock;
        }

        /// @brief 
        /// @return 
        const UID& GetUid() const
        {
            return _uid;
        }

        /// @brief 
        /// @return 
        const UID& GetForSerialization() const
        {
            std::shared_ptr<_Component_> lock = this->_pointer.lock();
            if (lock == nullptr)
            {
                return this->_uid;
            }
            return lock->GetUid();
        }

    private:
        //union 
        //{
            UID                         _uid;
            mutable std::weak_ptr<_Component_>  _pointer;
        //};
    };

    template<typename _Component_>
    WeakComponent<_Component_>::WeakComponentReflectionDescriptor::WeakComponentReflectionDescriptor()
    : hod::ReflectionDescriptor(hod::ReflectionDescriptor::GenerateReflectionData<WeakComponent, void>("WeakComponent"))
    {
        ReflectionProperty* property = ADD_PROPERTY(WeakComponent, _uid);
        property->AddTrait<ReflectionTraitGetValueForSerialization>([](const void* instance, void* value){ *static_cast<UID*>(value) = static_cast<const WeakComponent*>(instance)->GetForSerialization(); }, (uint32_t)sizeof(UID));
    }
}
