#include "EditorBridge.h"

#include "Scene.h"

#include <Application/src/Application.h>

#include <rapidjson/writer.h>

#if defined(_WIN32)
	// Need to link with Ws2_32.lib
	#pragma comment(lib, "ws2_32.lib")
#endif

namespace HOD
{
	template<>
	GAME::EditorBridge* CORE::Singleton<GAME::EditorBridge>::_instance = nullptr;

	namespace GAME
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		EditorBridge::EditorBridge() : Singleton()
		{

		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		EditorBridge::~EditorBridge()
		{
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		bool EditorBridge::Init()
		{
		#if defined(_WIN32)
			WSADATA wsaData;

			int iResult;

			// Initialize Winsock
			iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
			if (iResult != 0)
			{
				printf("WSAStartup failed: %d\n", iResult);
				return false;
			}

			_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

			if (_socket == INVALID_SOCKET)
			{
				printf("Error at socket(): %ld\n", WSAGetLastError());
				WSACleanup();
				return false;
			}

			sockaddr_in sockaddr;
			sockaddr.sin_family = AF_INET;
			sockaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
			sockaddr.sin_port = htons(APPLICATION::Application::GetInstance()->GetEditorPort());

			iResult = connect(_socket, (SOCKADDR*)&sockaddr, sizeof(sockaddr));
			if (iResult == SOCKET_ERROR)
			{
				closesocket(_socket);
				_socket = INVALID_SOCKET;
			}

			if (_socket == INVALID_SOCKET)
			{
				printf("Unable to connect to server!\n");
				WSACleanup();
				return false;
			}

			SendSetWindowHandle();

			return true;
		#elif (__linux__)
			return false; // TODO
		#endif
		};

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void EditorBridge::Update(float dt)
		{

		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void EditorBridge::SendJson(const rapidjson::StringBuffer& json)
		{
			size_t jsonSize = json.GetSize();

			char* buffer = new char[sizeof(short) + jsonSize];
			*((short*)buffer) = jsonSize;

			memcpy(buffer + sizeof(short), json.GetString(), jsonSize);

			send(_socket, buffer, sizeof(short) + jsonSize, 0);
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void EditorBridge::SendSetWindowHandle()
		{
			rapidjson::StringBuffer json;
			rapidjson::Writer<rapidjson::StringBuffer> writer(json);

			writer.StartObject();
			{
				writer.Key("Command");
				writer.String("SetWindowHandle");

				writer.Key("WindowHandle");
				writer.Uint64((uint64_t)APPLICATION::Application::GetInstance()->GetHwnd());
			}
			writer.EndObject();

			SendJson(json);
		}
	}
}
