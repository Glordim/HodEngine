#pragma once

#include <Core/src/Singleton.h>

#include <cstdint>

#include <winsock.h>

#include <rapidjson/stringbuffer.h>

namespace HOD
{
	namespace GAME
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		class EditorBridge : public CORE::Singleton<EditorBridge>
		{
			friend class CORE::Singleton<EditorBridge>;

		public:

			bool				Init();

			void				Update(float dt);

			void				SendSetWindowHandle();

		protected:

								EditorBridge();
								~EditorBridge() override;

		private:

			void				SendJson(const rapidjson::StringBuffer& json);

		private:

			SOCKET				_socket = 0;

		};
	}
}
