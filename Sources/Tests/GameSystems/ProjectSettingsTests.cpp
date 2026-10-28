#include <gtest/gtest.h>

#include <HodEngine/Core/FileSystem/FileSystem.hpp>
#include <HodEngine/GameSystems/ProjectSettings/ProjectSettingsManager.hpp>
#include <HodEngine/Core/Reflection/ReflectionDescriptor.hpp>

#include <filesystem>

namespace hod
{
	class TestSettings : public ProjectSettings
	{
		REFLECTED_CLASS(TestSettings, ProjectSettings)

	public:
		int32_t _value = 1;
	};

	DESCRIBE_REFLECTED_CLASS(TestSettings, reflectionDescriptor)
	{
		AddPropertyT(reflectionDescriptor, &TestSettings::_value, "Value");
	}

	class ProjectSettingsTest : public ::testing::Test
	{
	protected:
		void SetUp() override
		{
			_ownsFileSystem = FileSystem::GetInstance() == nullptr;
			if (_ownsFileSystem)
			{
				ASSERT_TRUE(FileSystem::CreateInstance()->Init());
			}
			_directory = FileSystem::GetTemporaryPath() / "HodProjectSettingsTests";
			std::filesystem::remove_all(_directory.GetString().CStr());

			ProjectSettingsManager::CreateInstance();
		}

		void TearDown() override
		{
			ProjectSettingsManager::DestroyInstance();
			std::filesystem::remove_all(_directory.GetString().CStr());
			if (_ownsFileSystem)
			{
				FileSystem::DestroyInstance();
			}
		}

		Path _directory;
		bool _ownsFileSystem = false;
	};

	TEST_F(ProjectSettingsTest, DefaultsWithoutFile)
	{
		ProjectSettingsManager* manager = ProjectSettingsManager::GetInstance();
		EXPECT_EQ(manager->Get<TestSettings>(), nullptr);

		TestSettings* settings = manager->Register<TestSettings>();
		ASSERT_NE(settings, nullptr);
		EXPECT_EQ(manager->Register<TestSettings>(), settings);
		EXPECT_EQ(manager->Get<TestSettings>(), settings);

		EXPECT_TRUE(manager->Load(_directory));
		EXPECT_EQ(settings->_value, 1);

		manager->Unregister<TestSettings>();
		EXPECT_EQ(manager->Get<TestSettings>(), nullptr);
		EXPECT_TRUE(manager->GetAll().Empty());
	}

	TEST_F(ProjectSettingsTest, SaveThenLoad)
	{
		ProjectSettingsManager* manager = ProjectSettingsManager::GetInstance();
		ASSERT_TRUE(manager->Load(_directory));

		TestSettings* settings = manager->Register<TestSettings>();
		settings->_value = 42;
		ASSERT_TRUE(manager->Save(*settings));
		EXPECT_TRUE(FileSystem::GetInstance()->Exists(ProjectSettingsManager::GetFilePath(_directory, TestSettings::GetReflectionDescriptor())));

		// Registered after the load (game module reloaded)
		manager->Unregister<TestSettings>();
		settings = manager->Register<TestSettings>();
		EXPECT_EQ(settings->_value, 42);

		// Registered before the load (builtin settings at startup)
		ProjectSettingsManager::DestroyInstance();
		manager = ProjectSettingsManager::CreateInstance();
		settings = manager->Register<TestSettings>();
		EXPECT_EQ(settings->_value, 1);
		ASSERT_TRUE(manager->Load(_directory));
		EXPECT_EQ(settings->_value, 42);
	}
}
