#include "Content.h"

namespace HOD
{
	namespace CORE
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		inline const UID& Content::GetUID() const
		{
			return _uid;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		inline const std::string& Content::GetName() const
		{
			return _name;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		inline const std::string& Content::GetAssetPath() const
		{
			return _assetPath;
		}

		inline bool Content::IsDirty() const
		{
			return _isDirty;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		inline void Content::RegisterAddDependency(typename AddDependencySignal::Slot& slot)
		{
			_addDependencySignal.Connect(slot);
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		inline void Content::UnRegisterAddDependency(typename AddDependencySignal::Slot& slot)
		{
			_addDependencySignal.Disconnect(slot);
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		inline void Content::RegisterRemoveDependency(typename RemoveDependencySignal::Slot& slot)
		{
			_removeDependencySignal.Connect(slot);
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		inline void Content::UnRegisterRemoveDependency(typename RemoveDependencySignal::Slot& slot)
		{
			_removeDependencySignal.Disconnect(slot);
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		inline void Content::SetName(const std::string& name)
		{
			_name = std::string(name);
			SetDirty();
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		inline void Content::SetAssetPath(const std::string& assetPath)
		{
			_assetPath = std::string(assetPath);
			SetDirty();
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		inline void Content::SetDirty()
		{
			_isDirty = true;
		}
	}
}
