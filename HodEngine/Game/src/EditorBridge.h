#pragma once

#include <Core/Src/Singleton.h>

#include <cstdint>

#if defined(_WIN32)
	#include <winsock.h>

	using Socket = SOCKET;
#elif defined(__linux__)
	#include <sys/socket.h>

	using Socket = int;
#endif

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

			Socket				_socket = 0;
		};
	}
}
