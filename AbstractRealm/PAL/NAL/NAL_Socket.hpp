#pragma once


#include "NAL_Backend.hpp"

#include "OSAL_Networking.hpp"


namespace NAL
{
	using OSAL::EAddressFamily;
	using OSAL::EProtocol;
	using OSAL::ESocketType;
	using OSAL::ESocketOpResult;
	using OSAL::ESocketChannel;
	using OSAL::EIPV;
	using OSAL::AddressInfo;
	using OSAL::IPV4_TRAddress;
	using OSAL::IPV6_TRAddress;
	using OSAL::SocketAddress_IPV4;
	using OSAL::SocketAddress_IPV6;


	class SocketAddress
	{
	public:

		SocketAddress(IPV4_TRAddress _address, u16 _port);
		SocketAddress(IPV6_TRAddress _address, u16 _port, ui32 _scopeID);

		SocketAddress(const OSAL::SocketAddress& socketAddress);

		static SPtr<SocketAddress> CreateIPV4(const String& _hostAndPort);
		static SPtr<SocketAddress> CreateIPV6(u32 /*_address*/, u16 /*_port*/)
		{
			Exception::Fatal::NotImplemented("CreateIPV6");

			return nullptr;   // Line to remove warning.
		}

		ForceInline 
		ui32 GetSize() const
		{
			return sizeof(address);
		}

		operator const OSAL::SocketAddress() const
		{
			return address;
		}

	protected:

		ForceInline 
		SocketAddress_IPV4& GetAsIPV4()
		{
			return RCast<SocketAddress_IPV4>(address);
		}

		ForceInline 
		SocketAddress_IPV6& GetAsIPV6()
		{
			return RCast<SocketAddress_IPV6>(address);
		}

		EIPV version;

		OSAL::SocketAddress address;
	};


	class Socket
	{
	public:

		ForceInline
		void Create(EAddressFamily _family, ESocketType _type, EProtocol _protocol)
		{
			handle = OSAL::CreateSocket(_family, _type, _protocol);
		}

		ForceInline
		void CreateUDP(EAddressFamily _family, EProtocol _protocol = EProtocol::Default)
		{
			handle = OSAL::CreateSocket(_family, ESocketType::DGram, _protocol);
		}

		ForceInline
			void CreateTCP(EAddressFamily _family, EProtocol _protocol = EProtocol::Default)
		{
			handle = OSAL::CreateSocket(_family, ESocketType::Stream, _protocol);
		}

		ForceInline
		void Bind(const SocketAddress& _address)
		{
			OSAL::BindSocket(dref(this), _address, _address.GetSize());
		}

		ForceInline
		ESocketOpResult Shutdown(ESocketChannel _what)
		{
			return OSAL::ShutdownSocket(handle, _what);
		}

		ForceInline
		ESocketOpResult Close()
		{
			return OSAL::CloseSocket(handle);
		}

		operator OSAL::Socket()
		{
			return handle;
		}

	protected:

		OSAL::Socket handle;
	};
}
