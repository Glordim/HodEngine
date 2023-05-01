#pragma once

#include "HodEngine/Core/UID.h"
#include "HodEngine/Core/Event.h"
#include <map>

namespace hod
{
	namespace CORE
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		template<typename T>
		class GenericManager
		{
		public:

			Event<T*>							OnDataAdded;
			Event<T*>							OnDataRemoved;

		public:

												GenericManager() = default;
												GenericManager(const GenericManager& copy) = delete;
			virtual								~GenericManager() = default;

			void								operator=(const GenericManager& right) = delete;

		public:

			uint32_t							Count() const;
			bool								IsEmpty() const;
			T*									GetData(const UID& uid) const;
			void								RemoveData(const UID& uid);

		public:

			void								AddData(const UID& uid, const T* data);

		private:

			std::vector<std::pair<UID, T*>>		_uidToDataMap;
		};
	}
}

#include "GenericManager.inl"
