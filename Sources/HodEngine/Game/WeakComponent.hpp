#pragma once
#include "HodEngine/Game/Export.hpp"
#include <memory>
#include <map>

#include <HodEngine/Core/UID.hpp>
#include <HodEngine/Core/Reflection/ReflectionMacros.hpp>
#include <HodEngine/Core/Reflection/Traits/ReflectionTraitGetValueForSerialization.hpp>

namespace hod::game
{
    class Component;

    /// @brief 
    class HOD_GAME_API WeakComponentMapping
    {
    public:

        static void Insert(const UID& uid, std::weak_ptr<Component> component);
        static void Clear();

        static std::shared_ptr<Component> Resolve(const UID& uid);

        template<typename _Component_>
        static std::shared_ptr<_Component_> Resolve(const UID& uid)
        {
            return std::static_pointer_cast<_Component_>(Resolve(uid));
        }

    private:

        static std::map<UID, std::weak_ptr<Component>> _map;
    };

    class HOD_GAME_API WeakComponentBase
    {
        REFLECTED_CLASS_NO_VIRTUAL(WeakComponentBase, HOD_GAME_API)

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
        const UID&                  GetUid() const;
        const UID&                  GetForSerialization() const;

        ReflectionDescriptor*       GetComponentDescriptor() const;

        void                        SetUid(const UID& uid);

    private:

        ReflectionDescriptor*           _componentDescriptor = nullptr;
        
        UID                                 _uid;
        mutable std::weak_ptr<Component>   _pointer;
    };

    /// @brief 
    /// @tparam _Component_ 
    template<typename _Component_>
    class WeakComponent : public WeakComponentBase
    {
    public:

        WeakComponent()
        : WeakComponentBase(_Component_::GetReflectionDescriptor())
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
    };
}
