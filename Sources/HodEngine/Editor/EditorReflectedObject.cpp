#include "HodEngine/Editor/EditorReflectedObject.hpp"

namespace hod::editor
{
    EditorReflectedObject::EditorReflectedObject(void* instance, ReflectionDescriptor* reflectionDescriptor)
    : _instances({ instance })
    , _reflectionProperty(reflectionProperty)
    {

    }

    EditorReflectedObject::EditorReflectedObject(const std::vector<void*>& instances, ReflectionDescriptor* reflectionDescriptor)
    : _instances(instances)
    , _reflectionProperty(reflectionProperty)
    {

    }
}