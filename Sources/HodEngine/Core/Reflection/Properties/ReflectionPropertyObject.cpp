#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyObject.h"
#include "HodEngine/Core/Object.h"

namespace hod
{
	namespace core
	{
		namespace Reflection
		{
			namespace Property
			{
				/// @brief 
				/// @param offset 
				/// @param name 
				Object::Object(uint32_t offset, const char* name)
					: ReflectionProperty()
					, _offset(offset)
					, _name(name)
				{

				}
				
				/// @brief 
				/// @param instance 
				/// @param node 
				void Object::Serialize(const void* instance, Document::Node& node)
				{
					const uint8_t* instanceAddress = reinterpret_cast<const uint8_t*>(instance);
					const uint8_t* objectAddress = instanceAddress + _offset;

					const hod::Object* objectInstance = reinterpret_cast<const hod::Object*>(objectAddress);

					Document::Node& objectNode = node.GetOrAddChild(_name);
					
					objectInstance->GetReflectionDescriptorV()->SerializeInDocument((const void*)objectInstance, objectNode);
				}

				/// @brief 
				/// @param instance 
				/// @param node 
				void Object::Deserialize(void* instance, const Document::Node& node)
				{
					uint8_t* instanceAddress = reinterpret_cast<uint8_t*>(instance);
					uint8_t* objectAddress = instanceAddress + _offset;

					hod::Object* objectInstance = reinterpret_cast<hod::Object*>(objectAddress);

					const Document::Node* objectNode = node.GetChild(_name);
					
					if (objectNode != nullptr)
					{
						objectInstance->GetReflectionDescriptorV()->DeserializeFromDocument((void*)objectInstance, *objectNode);
					}
				}

				/// @brief 
				/// @return 
				const char* Object::GetName() const
				{
					return _name;
				}

				/// @brief 
				/// @param instance 
				/// @return 
				hod::Object* Object::GetInstance(void* instance) // todo use it on deserialize
				{
					uint8_t* instanceAddress = reinterpret_cast<uint8_t*>(instance);
					uint8_t* objectAddress = instanceAddress + _offset;

					hod::Object* objectInstance = reinterpret_cast<hod::Object*>(objectAddress);

					return objectInstance;
				}
			}
		}
	}
}
