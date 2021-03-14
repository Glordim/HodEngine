#pragma once

#include "Singleton.h"
#include "UID.h"
#include "Signal.h"
#include <map>

namespace HOD
{
    namespace CORE
    {
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		template<typename T>
		class GenericManager : public Singleton<GenericManager<T>>
        {
		public:

			using AddDataSignal = Signal<T*>;
			using RemoveDataSignal = Signal<T*>;

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

			void								RegisterAddData(typename AddDataSignal::Slot& slot);
			void								UnRegisterAddData(typename AddDataSignal::Slot& slot);
			void								RegisterRemoveData(typename RemoveDataSignal::Slot& slot);
			void								UnRegisterRemoveData(typename RemoveDataSignal::Slot& slot);

		public:

			void								AddData(const UID& uid, const T* data);

		private:
			std::vector<std::pair<UID, T*>>		_uidToDataMap;
			AddDataSignal						_addDataSignal;
			RemoveDataSignal					_removeDataSignal;
        };
	}
}

#include "GenericManager.inl"
