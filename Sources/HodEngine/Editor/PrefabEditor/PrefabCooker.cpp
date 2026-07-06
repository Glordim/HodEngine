#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/PrefabEditor/PrefabCooker.hpp"
#include "HodEngine/Game/PrefabResource.hpp"

#include <HodEngine/Core/Document/DocumentReaderJson.hpp>
#include <HodEngine/Core/Document/DocumentWriterJson.hpp>

namespace hod::inline editor
{
	bool PrefabCooker::FillDataBlock(const Asset& asset, uint32_t /*platforms*/, uint8_t configs, uint32_t /*languages*/)
	{
		/*
		DocumentReaderJson documentReader;
		if (documentReader.Read(document, data) == false)
		{
			return false;
		}
		return true;
		*/
		(void)asset;
		(void)configs;
		return false;
	}
}
