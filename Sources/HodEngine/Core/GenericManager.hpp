#pragma once
#include <HodEngine/HodEngine.hpp>

#include "HodEngine/Core/UID.hpp"
#include "HodEngine/Core/Event.hpp"
#include <map>

namespace hod
{
	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	template<typename T>
	class HOD_API GenericManager
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

#include "GenericManager.inl"
