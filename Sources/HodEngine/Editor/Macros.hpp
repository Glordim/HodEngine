#pragma once

#include "HodEngine/Editor/Trait/ReflectionTraitComponentCustomEditor.hpp"

#define ADD_CUSTOM_EDITOR(Class) AddTrait<ReflectionTraitComponentCustomEditor>(new Class())
