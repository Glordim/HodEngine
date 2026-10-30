#include "HodEngine/UI2/Pch.hpp"
#include "HodEngine/UI2/PropertyOverride.hpp"
#include "HodEngine/UI2/Interaction.hpp"
#include "HodEngine/UI2/Layout.hpp"
#include "HodEngine/UI2/LayoutParams.hpp"
#include "HodEngine/UI2/Node.hpp"

#include <HodEngine/Core/Math.hpp>
#include <HodEngine/Core/Memory/DefaultAllocator.hpp>
#include <HodEngine/Core/Reflection/Properties/ReflectionPropertyObject.hpp>
#include <HodEngine/Core/Reflection/Properties/ReflectionPropertyVariable.hpp>
#include <HodEngine/Core/Reflection/ReflectionDescriptor.hpp>
#include <HodEngine/Core/Reflection/Traits/ReflectionTraitCustomSerialization.hpp>
#include <HodEngine/Core/Reflection/Traits/ReflectionTraitHide.hpp>
#include <HodEngine/Core/Reflection/Traits/ReflectionTraitNoSerialization.hpp>
#include <HodEngine/Core/Serialization/Serializer.hpp>

#include <type_traits>

namespace hod::inline ui2
{
	DESCRIBE_REFLECTED_CLASS(PropertyOverride, reflectionDescriptor)
	{
		AddPropertyT(reflectionDescriptor, &PropertyOverride::_target, "Target");
		AddPropertyT(reflectionDescriptor, &PropertyOverride::_property, "Property");
		AddPropertyT(reflectionDescriptor, &PropertyOverride::_value, "Value");
	}

	namespace
	{
		constexpr std::string_view LayoutParamsPrefix = "LayoutParams/";
		constexpr std::string_view LayoutPrefix = "Layout/";
		constexpr std::string_view InteractionPrefix = "Interaction/";

		/// @brief Calls function.template operator()<T>() with T the C++ type of a variable of that type.
		template<typename _Function_>
		void VisitVariableType(ReflectionPropertyVariable::Type type, _Function_&& function)
		{
			using Type = ReflectionPropertyVariable::Type;
			switch (type)
			{
				case Type::Bool: function.template operator()<bool>(); break;
				case Type::Int8: function.template operator()<int8_t>(); break;
				case Type::Int16: function.template operator()<int16_t>(); break;
				case Type::Int32: function.template operator()<int32_t>(); break;
				case Type::Int64: function.template operator()<int64_t>(); break;
				case Type::UInt8: function.template operator()<uint8_t>(); break;
				case Type::UInt16: function.template operator()<uint16_t>(); break;
				case Type::UInt32: function.template operator()<uint32_t>(); break;
				case Type::UInt64: function.template operator()<uint64_t>(); break;
				case Type::Float32: function.template operator()<float>(); break;
				case Type::Float64: function.template operator()<double>(); break;
				case Type::String: function.template operator()<String>(); break;
				default: break;
			}
		}

		bool IsSupportedObject(const ReflectionDescriptor& descriptor);

		/// @brief Whether a value of property's type can be held, eased and compared by PropertyValue (whatever owns it).
		bool IsSupported(const ReflectionProperty& property)
		{
			if (property.GetRttiType() == ReflectionPropertyVariable::GetRttiTypeStatic())
			{
				ReflectionPropertyVariable::Type type = static_cast<const ReflectionPropertyVariable&>(property).GetType();
				return type != ReflectionPropertyVariable::Type::Object && type < ReflectionPropertyVariable::Type::Count;
			}
			if (property.GetRttiType() == ReflectionPropertyObject::GetRttiTypeStatic())
			{
				const ReflectionDescriptor* descriptor = static_cast<const ReflectionPropertyObject&>(property).GetReflectionDescriptor();
				return descriptor != nullptr && IsSupportedObject(*descriptor);
			}
			return false; // arrays
		}

		/// @brief An object made of supported properties only, that can be instantiated, and serialized as its properties
		/// (a custom serialization may rely on members reflection doesn't copy).
		bool IsSupportedObject(const ReflectionDescriptor& descriptor)
		{
			if (descriptor.CanCreateInstance() == false || descriptor.FindTrait<ReflectionTraitCustomSerialization>() != nullptr)
			{
				return false;
			}
			for (const ReflectionDescriptor* level = &descriptor; level != nullptr; level = level->GetParent())
			{
				for (const ReflectionProperty* property : level->GetProperties())
				{
					if (IsSupported(*property) == false)
					{
						return false;
					}
				}
			}
			return true;
		}

		/// @brief Whether an override can drive property (of a node, LayoutParams or Layout).
		bool IsOverridable(const ReflectionProperty& property)
		{
			return property.FindTrait<ReflectionTraitHide>() == nullptr && property.FindTrait<ReflectionTraitNoSerialization>() == nullptr && IsSupported(property);
		}

		/// @brief The instance owning the property at propertyPath (node itself, its LayoutParams or its Layout) and that property.
		/// @return false if there is none, or it isn't overridable
		bool Resolve(const Node& node, std::string_view propertyPath, void*& outOwner, const ReflectionProperty*& outProperty)
		{
			const ReflectionDescriptor* descriptor = nullptr;
			if (propertyPath.starts_with(LayoutParamsPrefix))
			{
				LayoutParams* layoutParams = node.GetLayoutParams();
				if (layoutParams == nullptr)
				{
					return false;
				}
				outOwner = layoutParams;
				descriptor = &layoutParams->GetReflectionDescriptorV();
				propertyPath.remove_prefix(LayoutParamsPrefix.size());
			}
			else if (propertyPath.starts_with(LayoutPrefix))
			{
				Layout* layout = node.GetLayout();
				if (layout == nullptr)
				{
					return false;
				}
				outOwner = layout;
				descriptor = &layout->GetReflectionDescriptorV();
				propertyPath.remove_prefix(LayoutPrefix.size());
			}
			else if (propertyPath.starts_with(InteractionPrefix))
			{
				Interaction* interaction = node.GetInteraction();
				if (interaction == nullptr)
				{
					return false;
				}
				outOwner = interaction;
				descriptor = &interaction->GetReflectionDescriptorV();
				propertyPath.remove_prefix(InteractionPrefix.size());
			}
			else
			{
				outOwner = const_cast<Node*>(&node);
				descriptor = &node.GetReflectionDescriptorV();
			}

			outProperty = descriptor->FindProperty(propertyPath);
			return outProperty != nullptr && IsOverridable(*outProperty);
		}

		/// @brief Appends to paths the overridable properties of descriptor (its parents' first), prefixed by prefix.
		void CollectOverridableProperties(const ReflectionDescriptor& descriptor, std::string_view prefix, Vector<String>& paths)
		{
			if (descriptor.GetParent() != nullptr)
			{
				CollectOverridableProperties(*descriptor.GetParent(), prefix, paths);
			}
			for (const ReflectionProperty* property : descriptor.GetProperties())
			{
				if (IsOverridable(*property))
				{
					paths.PushBack(String(prefix.data(), (uint32_t)prefix.size()) + property->GetName());
				}
			}
		}

		/// @brief Whether values of left and right properties have the same type (so the same memory layout).
		bool IsSamePropertyType(const ReflectionProperty& left, const ReflectionProperty& right)
		{
			if (&left == &right)
			{
				return true;
			}
			if (left.GetRttiType() != right.GetRttiType())
			{
				return false;
			}
			if (left.GetRttiType() == ReflectionPropertyVariable::GetRttiTypeStatic())
			{
				return static_cast<const ReflectionPropertyVariable&>(left).GetType() == static_cast<const ReflectionPropertyVariable&>(right).GetType();
			}
			// Compared by type, not by address: each module (DLL) registering a property has its own descriptor instance of its type
			return static_cast<const ReflectionPropertyObject&>(left).GetReflectionDescriptor()->GetType() ==
			       static_cast<const ReflectionPropertyObject&>(right).GetReflectionDescriptor()->GetType();
		}

		/// @brief Eases variable from from to to into out (instances owning it): floats only, the others switch halfway.
		void LerpVariable(const ReflectionPropertyVariable& variable, const void* from, const void* to, void* out, float t)
		{
			if (variable.GetEnumDescriptor() == nullptr)
			{
				if (variable.GetType() == ReflectionPropertyVariable::Type::Float32)
				{
					float fromValue = variable.GetValue<float>(from);
					variable.SetValue<float>(out, fromValue + (variable.GetValue<float>(to) - fromValue) * t);
					return;
				}
				if (variable.GetType() == ReflectionPropertyVariable::Type::Float64)
				{
					double fromValue = variable.GetValue<double>(from);
					variable.SetValue<double>(out, fromValue + (variable.GetValue<double>(to) - fromValue) * (double)t);
					return;
				}
			}
			variable.Copy(t < 0.5f ? from : to, out);
		}

		/// @brief Eases every property of instances of descriptor's type (supported object, see IsSupportedObject).
		void LerpObject(const ReflectionDescriptor& descriptor, const void* from, const void* to, void* out, float t)
		{
			if (descriptor.GetParent() != nullptr)
			{
				LerpObject(*descriptor.GetParent(), from, to, out, t);
			}
			for (const ReflectionProperty* property : descriptor.GetProperties())
			{
				if (property->GetRttiType() == ReflectionPropertyVariable::GetRttiTypeStatic())
				{
					LerpVariable(*static_cast<const ReflectionPropertyVariable*>(property), from, to, out, t);
				}
				else if (property->GetRttiType() == ReflectionPropertyObject::GetRttiTypeStatic())
				{
					const ReflectionPropertyObject* object = static_cast<const ReflectionPropertyObject*>(property);
					LerpObject(*object->GetReflectionDescriptor(), object->GetInstance(from), object->GetInstance(to), object->GetInstance(out), t);
				}
			}
		}

		/// @brief Compares every property of instances of descriptor's type (supported object, see IsSupportedObject).
		bool EqualsObject(const ReflectionDescriptor& descriptor, const void* left, const void* right)
		{
			if (descriptor.GetParent() != nullptr && EqualsObject(*descriptor.GetParent(), left, right) == false)
			{
				return false;
			}
			for (const ReflectionProperty* property : descriptor.GetProperties())
			{
				if (property->GetRttiType() == ReflectionPropertyVariable::GetRttiTypeStatic())
				{
					if (static_cast<const ReflectionPropertyVariable*>(property)->CompareInstance(left, right) == false)
					{
						return false;
					}
				}
				else if (property->GetRttiType() == ReflectionPropertyObject::GetRttiTypeStatic())
				{
					const ReflectionPropertyObject* object = static_cast<const ReflectionPropertyObject*>(property);
					if (EqualsObject(*object->GetReflectionDescriptor(), object->GetInstance(left), object->GetInstance(right)) == false)
					{
						return false;
					}
				}
			}
			return true;
		}
	}

	/// @brief
	/// @param copy
	PropertyValue::PropertyValue(const PropertyValue& copy)
	{
		*this = copy;
	}

	/// @brief
	/// @param move
	PropertyValue::PropertyValue(PropertyValue&& move)
	{
		*this = std::move(move);
	}

	/// @brief
	PropertyValue::~PropertyValue()
	{
		Clear();
	}

	/// @brief
	/// @param copy
	/// @return
	PropertyValue& PropertyValue::operator=(const PropertyValue& copy)
	{
		if (this == &copy)
		{
			return *this;
		}
		if (copy.IsValid() == false)
		{
			Clear();
			return *this;
		}

		SetType(*copy._property);
		if (_variable != nullptr)
		{
			_variable->Copy(copy._storage, _storage);
		}
		else
		{
			static_cast<const ReflectionPropertyObject*>(_property)->GetReflectionDescriptor()->Copy(copy._storage, _storage);
		}
		return *this;
	}

	/// @brief
	/// @param move
	/// @return
	PropertyValue& PropertyValue::operator=(PropertyValue&& move)
	{
		if (this != &move)
		{
			Clear();
			_property = move._property;
			_variable = move._variable;
			_storage = move._storage;
			move._property = nullptr;
			move._variable = nullptr;
			move._storage = nullptr;
		}
		return *this;
	}

	/// @brief
	/// @param property
	void PropertyValue::SetType(const ReflectionProperty& property)
	{
		if (IsValid() && IsSamePropertyType(*_property, property))
		{
			return;
		}
		Clear();

		_property = &property;
		if (property.GetRttiType() == ReflectionPropertyVariable::GetRttiTypeStatic())
		{
			const ReflectionPropertyVariable& variable = static_cast<const ReflectionPropertyVariable&>(property);
			_variable = DefaultAllocator::GetInstance().New<ReflectionPropertyVariable>(variable.GetType(), 0u, variable.GetName(), nullptr, nullptr);
			_variable->SetEnumDescriptor(variable.GetEnumDescriptor());
			if (variable.GetType() == ReflectionPropertyVariable::Type::String)
			{
				_storage = DefaultAllocator::GetInstance().New<String>();
			}
			else
			{
				_storage = DefaultAllocator::GetInstance().New<uint64_t>(0u); // large and aligned enough for any other variable
			}
		}
		else
		{
			_storage = static_cast<const ReflectionPropertyObject&>(property).GetReflectionDescriptor()->CreateInstance();
		}
	}

	/// @brief
	void PropertyValue::Clear()
	{
		if (_storage != nullptr)
		{
			if (_variable != nullptr)
			{
				if (_variable->GetType() == ReflectionPropertyVariable::Type::String)
				{
					DefaultAllocator::GetInstance().Delete(static_cast<String*>(_storage));
				}
				else
				{
					DefaultAllocator::GetInstance().Delete(static_cast<uint64_t*>(_storage));
				}
			}
			else
			{
				static_cast<const ReflectionPropertyObject*>(_property)->GetReflectionDescriptor()->DeleteInstance(_storage);
			}
		}
		if (_variable != nullptr)
		{
			DefaultAllocator::GetInstance().Delete(_variable);
		}
		_property = nullptr;
		_variable = nullptr;
		_storage = nullptr;
	}

	/// @brief
	/// @return
	bool PropertyValue::IsValid() const
	{
		return _storage != nullptr;
	}

	/// @brief
	/// @param other
	/// @return
	bool PropertyValue::IsSameType(const PropertyValue& other) const
	{
		return IsValid() && other.IsValid() && IsSamePropertyType(*_property, *other._property);
	}

	/// @brief
	/// @param node
	/// @return
	Vector<String> PropertyValue::GetOverridableProperties(const Node& node, bool withInteraction)
	{
		Vector<String> paths;
		CollectOverridableProperties(node.GetReflectionDescriptorV(), "", paths);
		if (node.GetLayoutParams() != nullptr)
		{
			CollectOverridableProperties(node.GetLayoutParams()->GetReflectionDescriptorV(), LayoutParamsPrefix, paths);
		}
		if (node.GetLayout() != nullptr)
		{
			CollectOverridableProperties(node.GetLayout()->GetReflectionDescriptorV(), LayoutPrefix, paths);
		}
		if (withInteraction && node.GetInteraction() != nullptr)
		{
			CollectOverridableProperties(node.GetInteraction()->GetReflectionDescriptorV(), InteractionPrefix, paths);
		}
		return paths;
	}

	/// @brief
	/// @param propertyPath
	/// @return
	bool PropertyValue::IsLayoutParamsProperty(std::string_view propertyPath)
	{
		return propertyPath.starts_with(LayoutParamsPrefix);
	}

	/// @brief
	/// @param node
	/// @param propertyPath
	/// @return
	bool PropertyValue::Read(const Node& node, std::string_view propertyPath)
	{
		void*                     owner = nullptr;
		const ReflectionProperty* property = nullptr;
		if (Resolve(node, propertyPath, owner, property) == false)
		{
			return false;
		}

		SetType(*property);
		if (_variable != nullptr)
		{
			const ReflectionPropertyVariable* variable = static_cast<const ReflectionPropertyVariable*>(property);
			VisitVariableType(variable->GetType(), [&]<typename _Type_>() { _variable->SetValue<_Type_>(_storage, variable->GetValue<_Type_>(owner)); });
		}
		else
		{
			const ReflectionPropertyObject* object = static_cast<const ReflectionPropertyObject*>(property);
			object->GetReflectionDescriptor()->Copy(object->GetValue(owner), _storage);
		}
		return true;
	}

	/// @brief
	/// @param node
	/// @param propertyPath
	/// @return
	bool PropertyValue::Write(Node& node, std::string_view propertyPath) const
	{
		void*                     owner = nullptr;
		const ReflectionProperty* property = nullptr;
		if (IsValid() == false || Resolve(node, propertyPath, owner, property) == false || IsSamePropertyType(*property, *_property) == false)
		{
			return false;
		}

		if (_variable != nullptr)
		{
			const ReflectionPropertyVariable* variable = static_cast<const ReflectionPropertyVariable*>(property);
			VisitVariableType(variable->GetType(), [&]<typename _Type_>() { variable->SetValue<_Type_>(owner, _variable->GetValue<_Type_>(_storage)); });
		}
		else
		{
			static_cast<const ReflectionPropertyObject*>(property)->SetValue(owner, _storage);
		}
		return true;
	}

	/// @brief
	/// @param node
	/// @param propertyPath
	/// @param documentNode
	/// @return
	bool PropertyValue::Load(const Node& node, std::string_view propertyPath, const DocumentNode& documentNode)
	{
		void*                     owner = nullptr;
		const ReflectionProperty* property = nullptr;
		if (Resolve(node, propertyPath, owner, property) == false)
		{
			return false;
		}

		SetType(*property);
		if (_variable != nullptr)
		{
			// The serializer reads a variable from a child of the node it is given
			Document      document;
			DocumentNode& valueNode = document.GetRootNode().AddChild("Value");
			valueNode.Copy(documentNode);
			valueNode.SetName("Value");
			return Serializer::DeserializeVariable(_variable, _storage, document.GetRootNode(), "Value");
		}
		return Serializer::Deserialize(*static_cast<const ReflectionPropertyObject*>(property)->GetReflectionDescriptor(), _storage, documentNode);
	}

	/// @brief
	/// @param documentNode
	void PropertyValue::Save(DocumentNode& documentNode) const
	{
		String name = documentNode.GetName();
		if (IsValid() == false)
		{
			documentNode.Clear();
			return;
		}

		if (_variable != nullptr)
		{
			// The serializer writes a variable in a child of the node it is given
			Document document;
			Serializer::SerializeVariable(_variable, _storage, document.GetRootNode(), "Value");
			documentNode.Copy(*document.GetRootNode().GetChild("Value"));
		}
		else
		{
			documentNode.Clear();
			documentNode.SetType(DocumentNode::Type::Object);
			Serializer::Serialize(*static_cast<const ReflectionPropertyObject*>(_property)->GetReflectionDescriptor(), _storage, documentNode);
		}
		documentNode.SetName(name);
	}

	/// @brief
	/// @param other
	/// @return
	bool PropertyValue::Equals(const PropertyValue& other) const
	{
		if (IsSameType(other) == false)
		{
			return false;
		}
		if (_variable != nullptr)
		{
			return _variable->CompareInstance(_storage, other._storage);
		}
		return EqualsObject(*static_cast<const ReflectionPropertyObject*>(_property)->GetReflectionDescriptor(), _storage, other._storage);
	}

	/// @brief
	/// @param from
	/// @param to
	/// @param t
	/// @param out
	void PropertyValue::Lerp(const PropertyValue& from, const PropertyValue& to, float t, PropertyValue& out)
	{
		if (out.IsSameType(to) == false)
		{
			out = to;
		}
		if (from.IsSameType(to) == false)
		{
			out = to;
			return;
		}

		if (out._variable != nullptr)
		{
			LerpVariable(*out._variable, from._storage, to._storage, out._storage, t);
		}
		else
		{
			LerpObject(*static_cast<const ReflectionPropertyObject*>(out._property)->GetReflectionDescriptor(), from._storage, to._storage, out._storage, t);
		}
	}

	/// @brief
	/// @param copy
	PropertyOverride::PropertyOverride(const PropertyOverride& copy)
	{
		*this = copy;
	}

	/// @brief
	/// @param move
	PropertyOverride::PropertyOverride(PropertyOverride&& move)
	{
		*this = move;
	}

	/// @brief
	/// @param copy
	/// @return
	PropertyOverride& PropertyOverride::operator=(const PropertyOverride& copy)
	{
		if (this != &copy)
		{
			_target = copy._target;
			_property = copy._property;
			_value = copy._value;
		}
		return *this;
	}

	/// @brief
	/// @param move
	/// @return
	PropertyOverride& PropertyOverride::operator=(PropertyOverride&& move)
	{
		if (this != &move)
		{
			_target = move._target;
			_property = std::move(move._property);
			_value = move._value;
		}
		return *this;
	}

	/// @brief
	/// @param target
	/// @param outValue
	/// @return
	bool PropertyOverride::GetValue(const Node& target, PropertyValue& outValue) const
	{
		return outValue.Load(target, _property, _value.GetRootNode());
	}

	/// @brief
	/// @param value
	void PropertyOverride::SetValue(const PropertyValue& value)
	{
		value.Save(_value.GetRootNode());
	}

	namespace
	{
		/// @brief The node of owner's prefab having that local id, null if none.
		Node* FindTarget(Node& owner, uint64_t target)
		{
			return owner.GetScopeRoot()->FindByLocalId(target);
		}
	}

	/// @brief
	/// @param target
	/// @param property
	/// @return
	PropertyOverrideTransition::Track* PropertyOverrideTransition::FindTrack(uint64_t target, const String& property)
	{
		for (Track& track : _tracks)
		{
			if (track._target == target && track._property == property)
			{
				return &track;
			}
		}
		return nullptr;
	}

	/// @brief
	/// @param owner
	/// @param active
	/// @param allSets
	/// @param duration
	void PropertyOverrideTransition::Start(Node& owner, const Vector<PropertyOverride>* active, const Vector<const Vector<PropertyOverride>*>& allSets, float duration)
	{
		// Every property any state overrides gets a track, its base value read before any override touched it.
		for (const Vector<PropertyOverride>* overrides : allSets)
		{
			if (overrides == nullptr)
			{
				continue;
			}
			for (const PropertyOverride& propertyOverride : *overrides)
			{
				if (FindTrack(propertyOverride._target, propertyOverride._property) != nullptr)
				{
					continue;
				}

				Node*         target = FindTarget(owner, propertyOverride._target);
				PropertyValue base;
				if (target == nullptr || base.Read(*target, propertyOverride._property) == false)
				{
					continue;
				}

				Track& track = _tracks.EmplaceBack();
				track._target = propertyOverride._target;
				track._property = propertyOverride._property;
				track._base = std::move(base);
			}
		}

		// Resolved once here, from wherever each property is now (possibly halfway through the previous transition)
		for (Track& track : _tracks)
		{
			Node* target = FindTarget(owner, track._target);

			track._to = track._base;
			if (active != nullptr && target != nullptr)
			{
				for (const PropertyOverride& propertyOverride : *active)
				{
					if (propertyOverride._target == track._target && propertyOverride._property == track._property)
					{
						PropertyValue value;
						if (propertyOverride.GetValue(*target, value) && value.IsSameType(track._base))
						{
							track._to = std::move(value);
						}
						break;
					}
				}
			}

			if (target == nullptr || track._from.Read(*target, track._property) == false || track._from.IsSameType(track._to) == false)
			{
				track._from = track._to;
			}
			track._current = track._to;
		}

		_elapsed = 0.0f;
		_duration = duration;
		_running = (_tracks.Empty() == false);
	}

	/// @brief
	/// @param owner
	/// @param deltaTime
	void PropertyOverrideTransition::Update(Node& owner, float deltaTime)
	{
		if (_running == false)
		{
			return;
		}

		_elapsed += deltaTime;
		float t = _duration > 0.0f ? Clamp(_elapsed / _duration, 0.0f, 1.0f) : 1.0f;

		for (Track& track : _tracks)
		{
			Node* target = FindTarget(owner, track._target);
			if (target != nullptr)
			{
				PropertyValue::Lerp(track._from, track._to, t, track._current);
				track._current.Write(*target, track._property);
			}
		}

		if (t >= 1.0f)
		{
			_running = false;
		}
	}

	/// @brief
	void PropertyOverrideTransition::Reset()
	{
		_tracks.Clear();
		_running = false;
	}
}
