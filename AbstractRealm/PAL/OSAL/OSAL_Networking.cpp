// Parent
#include "OSAL_Networking.hpp"


#include "OSAL_Backend.hpp"


namespace OSAL
{
	namespace PlatformBackend
	{
		using NetAPI_Win = NetworkingAPI_Maker<EOS::Windows>;

		NetAPI_Win::SocketSubsystemData NetAPI_Win::sockSubsysInfo;

		Socket NetAPI_Win::CreateSocket(EAddressFamily _family, ESocketType _type, EProtocol _protocol)
		{
			return socket(SCast<int>(_family), SCast<int>(_type), SCast<int>(_protocol));
		}

		ESocketOpResult NetAPI_Win::BindSocket(Socket _socket, const SocketAddress& _address, ui32 length)
		{
			return SCast<ESocketOpResult>(bind(_socket, _address.operator const sockaddr*(), length));
		}
		
		ESocketOpResult NetAPI_Win::ShutdownSocket(Socket _socket, ESocketChannel _what)
		{
			return SCast<ESocketOpResult>(shutdown(_socket, SCast<int>(_what)));
		}

		ESocketOpResult NetAPI_Win::CloseSocket(Socket _socket)
		{
			return SCast<ESocketOpResult>(closesocket(_socket));
		}

		ESocketStatus NetAPI_Win::StartupSockSubsys()
		{
			WORD version = MAKEWORD(2, 2);

			return SCast<ESocketStatus>(WSAStartup(version, getPtr(sockSubsysInfo)));
		}

		ESocketStatus NetAPI_Win::CleanupSockSubsys()
		{
			return SCast<ESocketStatus>(WSACleanup());
		}

		EIPPresToNetworkCode NetAPI_Win::IPV4AddressToNetwork(RoCStr _presentation, IPV4_TRAddress& _ipv4Address_Out)
		{
			return SCast<EIPPresToNetworkCode>(inet_pton(AF_INET, _presentation, getPtr(_ipv4Address_Out)));
		}

		EIPPresToNetworkCode NetAPI_Win::IPPresentationToNetwork(RoCStr _presentation, IPV6_TRAddress& _ipv6Address_Out)
		{
			return SCast<EIPPresToNetworkCode>(inet_pton(AF_INET6, _presentation, getPtr(_ipv6Address_Out)));
		}

		EAddressInfoCode NetAPI_Win::GetAddressInfo(RoCStr _nodeName, RoCStr _serviceName, const AddressInfo& _hints, AddressInfo& _info_out)
		{
			return SCast<EAddressInfoCode>(getaddrinfo(_nodeName, _serviceName, _hints.operator const ADDRINFOA*(), getPtr(_info_out.operator ADDRINFOA*())));
		}

		EAddressInfoCode NetAPI_Win::GetAddressInfoAsync(RoCStr _nodeName, RoCStr _serviceName, const AddressInfo& _hints, AddressInfo& _info_out)
		{
			Exception::Fatal::NotImplemented("GetAddressInfo");

			return EAddressInfoCode::Fail;

			/*return GetAddrInfoExA
			(
				_nodeName, 
				_serviceName, 
				ENamespace::All,

			);*/
		}

		void NetAPI_Win::FreeAddressInfo(ptr<AddressInfo> _addressInfo_in)
		{
			freeaddrinfo(RCast<ADDRINFOA>(_addressInfo_in));
		}
	}

	void Load_Networking()
	{
		Log("Loading Networking.");

		if constexpr (OS == EOS::Windows)
		{
			NetworkingAPI::StartupSockSubsys();
		}
	}

	void Unload_Networking()
	{
		Log("Unloading networking.");

		if constexpr (OS == EOS::Windows)
		{
			NetworkingAPI::CleanupSockSubsys();
		}
	}
}



