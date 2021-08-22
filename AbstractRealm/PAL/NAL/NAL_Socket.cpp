// Parent
#include "NAL_Socket.hpp"


namespace NAL
{
	using OSAL::EAddressInfoCode;

#pragma region SocketAddress
	SocketAddress::SocketAddress(IPV4_TRAddress _address, u16 _port)
	{
		auto& v4Address = GetAsIPV4();

		v4Address.Family           = EAddressFamily::IPV4;
		v4Address.TransportAddress = _address;
		v4Address.Port             = OSAL::HostToNetwork<u16, u16>(_port);
	}

	SocketAddress::SocketAddress(IPV6_TRAddress _address, u16 _port, ui32 _scopeID)
	{
		auto& v6Address = GetAsIPV6();

		v6Address.Family           = EAddressFamily::IPV6;
		v6Address.TransportAddress = _address;
		v6Address.Port             = OSAL::HostToNetwork<u16, u16>(_port);
		v6Address.ScopeID          = _scopeID;
	}

	SocketAddress::SocketAddress(const OSAL::SocketAddress& _info)
	{
		FormatWithData<OSAL::SocketAddress>(getPtr(address), getPtr(_info), 1);
	}

	SPtr<SocketAddress> SocketAddress::CreateIPV4(const String& _hostAndPort)
	{
		auto colon = _hostAndPort.find_last_of(':');

		String host, port;

		if (colon != String::npos)
		{
			host = _hostAndPort.substr(0, colon);
			port = _hostAndPort.substr(colon + 1);
		}
		else
		{
			host = _hostAndPort;
			port = "0";
		}

		AddressInfo hint {};
		
		hint.Family = EAddressFamily::IPV4;

		ptr<AddressInfo> result = nullptr;

		EAddressInfoCode resultCode = OSAL::GetAddressInfo(host.c_str(), port.c_str(), hint, dref(result));

		if (resultCode != EAddressInfoCode::Success)
		{
			OSAL::FreeAddressInfo(result);

			return nullptr;
		}

		while (!result->Address && result->Next)
		{
			result = result->Next;
		}

		if (result->Address == nullptr)
		{
			OSAL::FreeAddressInfo(result);

			return nullptr;
		}

		SPtr<SocketAddress> socketAddress = MakeShared<SocketAddress>(dref(result->Address));

		OSAL::FreeAddressInfo(result);

		return socketAddress;
	}
#pragma endregion SocketAddress

#pragma region Socket
	void Socket::Create(EAddressFamily _family, ESocketType _type, EProtocol _protocol)
	{
		handle = OSAL::CreateSocket(_family, _type, _protocol);
	}

	void Socket::CreateUDP(EAddressFamily _family, EProtocol _protocol)
	{
		handle = OSAL::CreateSocket(_family, ESocketType::DGram, _protocol);
	}

	void Socket::CreateTCP(EAddressFamily _family, EProtocol _protocol)
	{
		handle = OSAL::CreateSocket(_family, ESocketType::Stream, _protocol);
	}

	void Socket::Bind(const SocketAddress& _address)
	{
		OSAL::BindSocket(dref(this), _address, _address.GetSize());
	}

	ESocketOpResult Socket::Shutdown(ESocketChannel _what)
	{
		return OSAL::ShutdownSocket(handle, _what);
	}

	ESocketOpResult Socket::Close()
	{
		return OSAL::CloseSocket(handle);
	}
#pragma endregion Socket
}
