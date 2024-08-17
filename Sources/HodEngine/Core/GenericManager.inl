
namespace hod
{
	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	template<typename T>
	inline uint32_t GenericManager<T>::Count() const
	{
		return (uint32_t)_uidToDataMap.size();
	}

	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	template<typename T>
	inline bool GenericManager<T>::IsEmpty() const
	{
		return Count() == 0;
	}

	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	template<typename T>
	T* GenericManager<T>::GetData(const UID& uid) const
	{
		for (const std::pair<UID, T*>& pair : _uidToDataMap)
		{
			if (pair.first == uid)
			{
				return pair.second;
			}
		}

		return nullptr;
	}

	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	template<typename T>
	void GenericManager<T>::RemoveData(const UID& uid)
	{
		uint32_t vectorSize = (uint32_t)_uidToDataMap.size();

		for (uint32_t i = 0; i < vectorSize; ++i)
		{
			if (_uidToDataMap[i].first == uid)
			{
				OnDataRemoved.Emit(_uidToDataMap[i].second);
				_uidToDataMap.erase(_uidToDataMap.begin() + i);
				return;
			}
		}
	}

	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	template<typename T>
	inline void GenericManager<T>::AddData(const UID& uid, const T* data)
	{
		_uidToDataMap.resize(_uidToDataMap.size() + 1);
		std::pair<UID, T*>& pair = _uidToDataMap.back();

		pair.first = uid;
		pair.second = (T*)data;

		OnDataAdded.Emit(pair.second);
	}
}
