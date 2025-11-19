#pragma once

#include "HodEngine/Editor/Trait/ReflectionTraitComponentCustomEditor.hpp"

#define ADD_CUSTOM_EDITOR(Class) AddTrait<ReflectionTraitComponentCustomEditor>(DefaultAllocator::GetInstance().New<Class>())
