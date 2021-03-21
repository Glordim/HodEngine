#include "Content.h"

namespace HOD
{
	namespace CORE
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		Content::Content()
			: _isDirty(false)
			, _isValid(false)
			, _uid(UID::GenerateUID())
			, _name("")
		{
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		bool Content::IsValid() const
		{
			size_t vectorSize = _dependencies.size();

			for (uint32_t i = 0; i < vectorSize; ++i)
			{
				if (_dependencies[i].second->IsValid() == false)
				{
					return false;
				}
			}

			return _assetPath.empty() == false && _name.empty() == false && _uid != UID::INVALID_UID;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void Content::AddDependency(Content& dependency)
		{
			_dependencies.resize(_dependencies.size() + 1);
			std::pair<UID, Content*>& pair = _dependencies.back();

			pair.first = dependency.GetUID();
			pair.second = &dependency;

			SetDirty();

			_addDependencySignal.Emit(&dependency);
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void Content::RemoveDependency(const UID& uid)
		{
			size_t vectorSize = _dependencies.size();

			for (uint32_t i = 0; i < vectorSize; ++i)
			{
				if (_dependencies[i].first == uid)
				{
					SetDirty();
					_removeDependencySignal.Emit(_dependencies[i].second);
					_dependencies.erase(_dependencies.begin() + i);
					return;
				}
			}
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		const std::vector<Content*> Content::GetAllDependencies() const
		{
			std::vector<Content*> vComponents;

			auto it = _dependencies.begin();
			auto itEnd = _dependencies.end();

			while (it != itEnd)
			{
				vComponents.push_back(it->second);

				++it;
			}

			return vComponents;
		}
	}
}
