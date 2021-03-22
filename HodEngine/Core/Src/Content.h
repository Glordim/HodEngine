#pragma once

#include <string>
#include <vector>

#include "UID.h"
#include "Signal.h"

namespace HOD
{
	namespace CORE
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		class Content
		{
		public:

			using AddDependencySignal = Signal<Content*>;
			using RemoveDependencySignal = Signal<Content*>;

		public:
			
													Content();
													Content(const Content& copy) = delete;
													Content(Content&&) = delete;
			virtual									~Content() = default;
			
			void									operator=(const Content& right) = delete;
			void									operator=(Content&& right) = delete;

		public:

			const UID&								GetUID() const;
			const std::string&						GetName() const;
			const std::string&						GetAssetPath() const;
			bool									IsDirty() const;
			bool									IsValid() const;

			void									AddDependency(Content& dependency);
			void									RemoveDependency(const UID& uid);
			const std::vector<Content*>				GetAllDependencies() const;

			void									RegisterAddDependency(typename AddDependencySignal::Slot& slot);
			void									UnRegisterAddDependency(typename AddDependencySignal::Slot& slot);
			void									RegisterRemoveDependency(typename RemoveDependencySignal::Slot& slot);
			void									UnRegisterRemoveDependency(typename RemoveDependencySignal::Slot& slot);

			void									SetName(const std::string& name);
			void									SetAssetPath(const std::string& assetPath);

		public:

			virtual void							Load() = 0;
			virtual void							Store() = 0;

		protected:

			void									SetDirty();// Bool ?

		private:

			UID										_uid;
			std::string								_name;
			std::string								_assetPath;
			bool									_isDirty;
			bool									_isValid;
			std::vector<std::pair<UID, Content*>>	_dependencies;

			AddDependencySignal						_addDependencySignal;
			RemoveDependencySignal					_removeDependencySignal;
		};
	}
}

#include "Content.inl"