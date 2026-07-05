#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/FontEditor/FontImporter.hpp"

namespace hod::inline editor
{
	bool FontImporter::FillDataBlock(Stream& source, ImporterSettings* importSettings)
	{
		(void)source;
		(void)importSettings;
		/*
		Resource::Data ttfData;
		ttfData._size = data.GetSize();
		ttfData._buffer = DefaultAllocator::GetInstance().Allocate<uint8_t>(ttfData._size);
		if (data.Read(ttfData._buffer, ttfData._size) != ttfData._size)
		{
			OUTPUT_ERROR("FontImporter : Can't read Font data");
			return false;
		}
		datas.push_back(ttfData);
		return true;
		*/
		return false;
	}
}
