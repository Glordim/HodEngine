#include "HodEngine/GameEditor/Pch.hpp"
#include "HodEngine/GameEditor/EntityBasedTabEditor/ViewportDebugView.hpp"

#include <algorithm>

namespace hod::inline editor
{
	namespace
	{
		struct DebugViewInstance
		{
			const void*                  _viewport = nullptr;
			const ViewportDebugViewType* _type = nullptr;
			ViewportDebugView*           _view = nullptr;
		};

		Vector<const ViewportDebugViewType*>& DebugViewTypes()
		{
			static Vector<const ViewportDebugViewType*> types;
			return types;
		}

		Vector<DebugViewInstance>& DebugViewInstances()
		{
			static Vector<DebugViewInstance> instances;
			return instances;
		}

		DebugViewInstance* FindDebugViewInstance(const void* viewport)
		{
			for (DebugViewInstance& instance : DebugViewInstances())
			{
				if (instance._viewport == viewport)
				{
					return &instance;
				}
			}
			return nullptr;
		}
	}

	/// @brief
	/// @param type
	/// @return
	bool ViewportDebugViewRegistry::Register(const ViewportDebugViewType& type)
	{
		if (std::find(DebugViewTypes().Begin(), DebugViewTypes().End(), &type) != DebugViewTypes().End())
		{
			OUTPUT_ERROR("ViewportDebugViewRegistry: '{}' is registered already", type._itemName);
			return false;
		}
		DebugViewTypes().PushBack(&type);
		return true;
	}

	/// @brief
	/// @param type
	void ViewportDebugViewRegistry::Unregister(const ViewportDebugViewType& type)
	{
		for (auto it = DebugViewInstances().Begin(); it != DebugViewInstances().End();)
		{
			if (it->_type == &type)
			{
				DefaultAllocator::GetInstance().Delete(it->_view);
				it = DebugViewInstances().Erase(it);
			}
			else
			{
				++it;
			}
		}

		auto it = std::find(DebugViewTypes().Begin(), DebugViewTypes().End(), &type);
		if (it != DebugViewTypes().End())
		{
			DebugViewTypes().Erase(it);
		}
	}

	/// @brief
	/// @return
	const Vector<const ViewportDebugViewType*>& ViewportDebugViewRegistry::GetTypes()
	{
		return DebugViewTypes();
	}

	/// @brief
	/// @param viewport
	/// @return
	const ViewportDebugViewType* ViewportDebugViewRegistry::GetEnabledType(const void* viewport)
	{
		DebugViewInstance* instance = FindDebugViewInstance(viewport);
		return instance != nullptr ? instance->_type : nullptr;
	}

	/// @brief
	/// @param viewport
	/// @return
	ViewportDebugView* ViewportDebugViewRegistry::GetEnabled(const void* viewport)
	{
		DebugViewInstance* instance = FindDebugViewInstance(viewport);
		return instance != nullptr ? instance->_view : nullptr;
	}

	/// @brief
	/// @param viewport
	/// @param type
	void ViewportDebugViewRegistry::SetEnabled(const void* viewport, const ViewportDebugViewType* type)
	{
		DebugViewInstance* instance = FindDebugViewInstance(viewport);
		if (instance != nullptr)
		{
			if (instance->_type == type)
			{
				return;
			}
			DefaultAllocator::GetInstance().Delete(instance->_view);
			DebugViewInstances().Erase(static_cast<uint32_t>(instance - DebugViewInstances().Data()));
		}

		if (type != nullptr)
		{
			ViewportDebugView* view = type->_create();
			if (view != nullptr)
			{
				DebugViewInstances().PushBack({viewport, type, view});
			}
		}
	}
}
