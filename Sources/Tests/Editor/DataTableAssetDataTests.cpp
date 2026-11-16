#include <gtest/gtest.h>

#include <HodEngine/GameEditor/DataTableEditor/DataTableAssetData.hpp>

#include <HodEngine/Core/Document/Document.hpp>
#include <HodEngine/Core/Reflection/ReflectionDescriptor.hpp>
#include <HodEngine/Game/DataStruct/DataStruct.hpp>
#include <HodEngine/Game/DataStruct/DataStructFactory.hpp>

namespace hod
{
	class DataTableAssetDataTestRow : public DataStruct
	{
		REFLECTED_CLASS(DataTableAssetDataTestRow, DataStruct)

	public:
		int32_t _value = 0;
		String  _name;
	};

	DESCRIBE_REFLECTED_CLASS(DataTableAssetDataTestRow)
	{
		HOD_PROPERTY(_value);
		HOD_PROPERTY(_name);
	}

	class DataTableAssetDataTest : public ::testing::Test
	{
	protected:
		void SetUp() override
		{
			if (DataStructFactory::GetInstance() == nullptr)
			{
				DataStructFactory::CreateInstance();
				_factoryCreated = true;
			}
			DataStructFactory::GetInstance()->Register<DataTableAssetDataTestRow>();
		}

		void TearDown() override
		{
			DataStructFactory::GetInstance()->Unregister<DataTableAssetDataTestRow>();
			if (_factoryCreated)
			{
				DataStructFactory::DestroyInstance();
			}
		}

		/// @brief A row as the tab adds it
		static void AddRow(DataTableAssetData& data, const char* key, int32_t value)
		{
			DataTableAssetDataTestRow* row = DataTableAssetDataTestRow::GetReflectionDescriptor().CreateInstance<DataTableAssetDataTestRow>();
			row->_value = value;
			row->_name = key;
			data._rows.PushBack(DataTableAssetData::Row {String(key), row});
		}

	private:
		bool _factoryCreated = false;
	};

	TEST_F(DataTableAssetDataTest, RoundTripKeepsKeysAndOrder)
	{
		DataTableAssetData written;
		written._rowType = DataTableAssetDataTestRow::GetReflectionDescriptor().GetType();
		AddRow(written, "Zeta", 1);
		AddRow(written, "Alpha", 2);
		AddRow(written, "Mu", 3);

		Document document;
		ASSERT_TRUE(written.Write(document.GetRootNode()));

		DataTableAssetData read;
		ASSERT_TRUE(read.Read(document.GetRootNode()));
		EXPECT_EQ(read._rowType, written._rowType);
		ASSERT_EQ(read._rows.Size(), 3u);
		EXPECT_EQ(read._rows[0]._key, "Zeta");
		EXPECT_EQ(read._rows[1]._key, "Alpha");
		EXPECT_EQ(read._rows[2]._key, "Mu");
		EXPECT_EQ(static_cast<DataTableAssetDataTestRow*>(read._rows[1]._data)->_value, 2);
		EXPECT_EQ(static_cast<DataTableAssetDataTestRow*>(read._rows[2]._data)->_name, "Mu");
	}

	TEST_F(DataTableAssetDataTest, EmptyTableKeepsItsRowType)
	{
		DataTableAssetData written;
		written._rowType = DataTableAssetDataTestRow::GetReflectionDescriptor().GetType();

		Document document;
		ASSERT_TRUE(written.Write(document.GetRootNode()));

		DataTableAssetData read;
		ASSERT_TRUE(read.Read(document.GetRootNode()));
		EXPECT_EQ(read._rowType, written._rowType);
		EXPECT_TRUE(read._rows.Empty());
	}

	TEST_F(DataTableAssetDataTest, UnknownRowTypeRefused)
	{
		DataTableAssetData written;
		written._rowType = DataTableAssetDataTestRow::GetReflectionDescriptor().GetType();
		AddRow(written, "Alpha", 1);

		Document document;
		ASSERT_TRUE(written.Write(document.GetRootNode()));

		DataStructFactory::GetInstance()->Unregister<DataTableAssetDataTestRow>(); // Its module not loaded
		DataTableAssetData read;
		EXPECT_FALSE(read.Read(document.GetRootNode()));
		EXPECT_EQ(read._rowType, written._rowType); // Known, to tell which
		EXPECT_TRUE(read._rows.Empty());
		DataStructFactory::GetInstance()->Register<DataTableAssetDataTestRow>();
	}
}
