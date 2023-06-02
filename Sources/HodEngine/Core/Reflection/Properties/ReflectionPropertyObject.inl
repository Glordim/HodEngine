
namespace hod
{
	namespace core
	{
		namespace Reflection
		{
			namespace Property
			{
				template<typename _object_>
				Object<_object_>::Object(uint32_t offset, const char* name)
					: ReflectionProperty()
					, _offset(offset)
					, _name(name)
				{

				}
				
				template<typename _object_>
				void Object<_object_>::Serialize(const void* instance, Document::Node& node)
				{
					const uint8_t* instanceAddress = reinterpret_cast<const uint8_t*>(instance);
					const uint8_t* objectAddress = instanceAddress + _offset;

					const _object_* objectInstance = reinterpret_cast<const _object_*>(objectAddress);

					Document::Node& objectNode = node.GetOrAddChild(_name);
					
					_object_::GetReflectionDescriptor()->SerializeInDocument((const void*)objectInstance, objectNode);
				}

				template<typename _object_>
				void Object<_object_>::Deserialize(void* instance, const Document::Node& node)
				{
					uint8_t* instanceAddress = reinterpret_cast<uint8_t*>(instance);
					uint8_t* objectAddress = instanceAddress + _offset;

					_object_* objectInstance = reinterpret_cast<_object_*>(objectAddress);

					const Document::Node* objectNode = node.GetChild(_name);
					
					if (objectNode != nullptr)
					{
						_object_::GetReflectionDescriptor()->DeserializeFromDocument((void*)objectInstance, *objectNode);
					}
				}
			}
		}
	}
}
