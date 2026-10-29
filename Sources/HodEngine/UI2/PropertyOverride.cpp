#include "HodEngine/UI2/Pch.hpp"
#include "HodEngine/UI2/PropertyOverride.hpp"
#include "HodEngine/UI2/Node.hpp"

#include <HodEngine/Core/Math.hpp>
#include <HodEngine/Core/Reflection/EnumTrait.hpp>
#include <HodEngine/Core/Reflection/Properties/ReflectionPropertyObject.hpp>
#include <HodEngine/Core/Reflection/Properties/ReflectionPropertyVariable.hpp>
#include <HodEngine/Core/Reflection/ReflectionDescriptor.hpp>
#include <HodEngine/Core/Reflection/Traits/ReflectionTraitHide.hpp>

namespace hod::inline ui2
{
	DESCRIBE_REFLECTED_ENUM(PropertyValue::Type, reflectionDescriptor)
	{
		reflectionDescriptor.AddEnumValue(PropertyValue::Type::Float, "Float");
		reflectionDescriptor.AddEnumValue(PropertyValue::Type::Vector2, "Vector2");
		reflectionDescriptor.AddEnumValue(PropertyValue::Type::Color, "Color");
	}

	DESCRIBE_REFLECTED_CLASS(PropertyValue, reflectionDescriptor)
	{
		AddPropertyT(reflectionDescriptor, &PropertyValue::_type, "Type");
		AddPropertyT(reflectionDescriptor, &PropertyValue::_float, "Float");
		AddPropertyT(reflectionDescriptor, &PropertyValue::_vector2, "Vector2");
		AddPropertyT(reflectionDescriptor, &PropertyValue::_color, "Color");
	}

	DESCRIBE_REFLECTED_CLASS(PropertyOverride, reflectionDescriptor)
	{
		AddPropertyT(reflectionDescriptor, &PropertyOverride::_target, "Target");
		AddPropertyT(reflectionDescriptor, &PropertyOverride::_property, "Property");
		AddPropertyT(reflectionDescriptor, &PropertyOverride::_value, "Value");
	}

	/// @brief
	/// @param property
	/// @param outType
	/// @return
	bool PropertyValue::GetType(const ReflectionProperty& property, Type& outType)
	{
		if (property.GetRttiType() == ReflectionPropertyVariable::GetRttiTypeStatic())
		{
			const ReflectionPropertyVariable& variable = static_cast<const ReflectionPropertyVariable&>(property);
			if (variable.GetType() == ReflectionPropertyVariable::Type::Float32 && variable.GetEnumDescriptor() == nullptr)
			{
				outType = Type::Float;
				return true;
			}
			return false;
		}

		if (property.GetRttiType() == ReflectionPropertyObject::GetRttiTypeStatic())
		{
			// Compared by type, not by address: each module (DLL) registering a property has its own descriptor instance of its type
			const ReflectionDescriptor* descriptor = static_cast<const ReflectionPropertyObject&>(property).GetReflectionDescriptor();
			if (descriptor == nullptr)
			{
				return false;
			}
			if (descriptor->GetType() == Vector2::GetReflectionDescriptor().GetType())
			{
				outType = Type::Vector2;
				return true;
			}
			if (descriptor->GetType() == Color::GetReflectionDescriptor().GetType())
			{
				outType = Type::Color;
				return true;
			}
		}
		return false;
	}

	/// @brief
	/// @param node
	/// @return
	Vector<const ReflectionProperty*> PropertyValue::GetOverridableProperties(const Node& node)
	{
		Vector<const ReflectionDescriptor*> descriptors;
		for (const ReflectionDescriptor* descriptor = &node.GetReflectionDescriptorV(); descriptor != nullptr; descriptor = descriptor->GetParent())
		{
			descriptors.Insert(descriptors.Begin(), descriptor);
		}

		Vector<const ReflectionProperty*> properties;
		for (const ReflectionDescriptor* descriptor : descriptors)
		{
			for (const ReflectionProperty* property : descriptor->GetProperties())
			{
				Type type;
				if (property->FindTrait<ReflectionTraitHide>() == nullptr && GetType(*property, type))
				{
					properties.PushBack(property);
				}
			}
		}
		return properties;
	}

	/// @brief
	/// @param other
	/// @return
	bool PropertyValue::Equals(const PropertyValue& other) const
	{
		if (_type != other._type)
		{
			return false;
		}
		switch (_type)
		{
			case Type::Float: return _float == other._float;
			case Type::Vector2: return _vector2 == other._vector2;
			case Type::Color: return _color == other._color;
		}
		return false;
	}

	/// @brief
	/// @param node
	/// @param propertyName
	/// @param outValue
	/// @return
	bool PropertyValue::Read(const Node& node, std::string_view propertyName, PropertyValue& outValue)
	{
		const ReflectionProperty* property = node.GetReflectionDescriptorV().FindProperty(propertyName);
		if (property == nullptr || GetType(*property, outValue._type) == false)
		{
			return false;
		}

		switch (outValue._type)
		{
			case Type::Float: outValue._float = static_cast<const ReflectionPropertyVariable*>(property)->GetValue<float>(&node); break;
			case Type::Vector2: outValue._vector2 = *static_cast<const Vector2*>(static_cast<const ReflectionPropertyObject*>(property)->GetValue(&node)); break;
			case Type::Color: outValue._color = *static_cast<const Color*>(static_cast<const ReflectionPropertyObject*>(property)->GetValue(&node)); break;
		}
		return true;
	}

	/// @brief
	/// @param node
	/// @param propertyName
	/// @return
	bool PropertyValue::Write(Node& node, std::string_view propertyName) const
	{
		const ReflectionProperty* property = node.GetReflectionDescriptorV().FindProperty(propertyName);
		Type                      propertyType;
		if (property == nullptr || GetType(*property, propertyType) == false || propertyType != _type)
		{
			return false;
		}

		switch (_type)
		{
			case Type::Float: static_cast<const ReflectionPropertyVariable*>(property)->SetValue<float>(&node, _float); break;
			case Type::Vector2: static_cast<const ReflectionPropertyObject*>(property)->SetValue(&node, &_vector2); break;
			case Type::Color: static_cast<const ReflectionPropertyObject*>(property)->SetValue(&node, &_color); break;
		}
		return true;
	}

	/// @brief
	/// @param from
	/// @param to
	/// @param t
	/// @return to's type; from's value is ignored if it's of another type
	PropertyValue PropertyValue::Lerp(const PropertyValue& from, const PropertyValue& to, float t)
	{
		if (from._type != to._type)
		{
			return to;
		}

		PropertyValue value = to;
		switch (to._type)
		{
			case Type::Float: value._float = from._float + (to._float - from._float) * t; break;
			case Type::Vector2: value._vector2 = Vector2::Lerp(from._vector2, to._vector2, t); break;
			case Type::Color:
			{
				value._color.r = from._color.r + (to._color.r - from._color.r) * t;
				value._color.g = from._color.g + (to._color.g - from._color.g) * t;
				value._color.b = from._color.b + (to._color.b - from._color.b) * t;
				value._color.a = from._color.a + (to._color.a - from._color.a) * t;
			}
			break;
		}
		return value;
	}

	namespace
	{
		/// @brief The node of owner's tree having that local id, null if none.
		Node* FindTarget(Node& owner, uint64_t target)
		{
			Node* root = &owner;
			while (root->GetParent() != nullptr)
			{
				root = root->GetParent();
			}
			return root->FindByLocalId(target);
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
				if (target == nullptr || PropertyValue::Read(*target, propertyOverride._property, base) == false)
				{
					continue;
				}

				Track& track = _tracks.EmplaceBack();
				track._target = propertyOverride._target;
				track._property = propertyOverride._property;
				track._base = base;
			}
		}

		// From wherever each property is now (possibly halfway through the previous transition)
		for (Track& track : _tracks)
		{
			track._to = track._base;
			if (active != nullptr)
			{
				for (const PropertyOverride& propertyOverride : *active)
				{
					if (propertyOverride._target == track._target && propertyOverride._property == track._property)
					{
						track._to = propertyOverride._value;
						break;
					}
				}
			}

			Node* target = FindTarget(owner, track._target);
			if (target == nullptr || PropertyValue::Read(*target, track._property, track._from) == false)
			{
				track._from = track._to;
			}
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

		for (const Track& track : _tracks)
		{
			Node* target = FindTarget(owner, track._target);
			if (target != nullptr)
			{
				PropertyValue::Lerp(track._from, track._to, t).Write(*target, track._property);
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
