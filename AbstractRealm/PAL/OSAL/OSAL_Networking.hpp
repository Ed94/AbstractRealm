#pragma once

#include "OSAL_Platform.hpp"


namespace OSAL
{
	namespace PlatformBackend
	{
		template<OSAL::EOS>
		struct NetworkingAPI_Maker;

		template<>
		struct NetworkingAPI_Maker<EOS::Windows>
		{
			enum class EAddressFamily : ui32
			{
				Unspecified  = AF_UNSPEC,
				Unix         = AF_UNIX,
				IPV4         = AF_INET,
				IMP_Link     = AF_IMPLINK,
				PUP          = AF_PUP,
				Chaos        = AF_CHAOS,
				NS           = AF_NS,
				IPX          = AF_IPX,
				ISO          = AF_ISO,
				OSI          = AF_OSI,
				ECMA         = AF_ECMA,
				Datakit      = AF_DATAKIT,
				CCITT        = AF_CCITT,
				SNA          = AF_SNA,
				DECnet       = AF_DECnet,
				DLI          = AF_DLI,
				LAT          = AF_LAT,
				HYPERchannel = AF_HYLINK,
				AppleTalk    = AF_APPLETALK,
				VoiceView    = AF_VOICEVIEW,
				FireFox      = AF_FIREFOX,
				Unknown1     = AF_UNKNOWN1,
				Banyan       = AF_BAN,
				NativeATM    = AF_ATM,
				IPV6         = AF_INET6,
				MSCS         = AF_CLUSTER,
				IEEE_1284    = AF_12844,
				NetBIOS      = AF_NETBIOS,
				IrDA         = AF_IRDA,
				NetDesOSI    = AF_NETDES,
			#if(_WIN32_WINNT >= 0x0501)
				TCNProcess   = AF_TCNPROCESS,
				TCNMessage   = AF_TCNMESSAGE,
				ICLFXBM      = AF_ICLFXBM,
			#if(_WIN32_WINNT < 0x0600)
				Bluetooth    = AF_BTH,
			#endif
			#if(_WIN32_WINNT >= 0x0601)
				Link         = AF_LINK,
			#endif
			#if(_WIN32_WINNT >= 0x0604)
				HyperVisor   = AF_HYPERV,
			#endif
			#endif
			};

		#pragma region Sockets
			enum class ESocketType : ui32
			{
				Unspecified = 0,
				Stream      = SOCK_STREAM,
				DGram       = SOCK_DGRAM,
				Raw         = SOCK_RAW,
				RDM         = SOCK_RDM,
				SeqPacket   = SOCK_SEQPACKET
			};

			enum class EProtocol : ui32
			{
				Default = 0,
			#if(_WIN32_WINNT >= 0x0501)
				HopByHopOpts = IPPROTO_HOPOPTS,  // IPv6 Hop-by-Hop options
			#endif//(_WIN32_WINNT >= 0x0501)
				ICMP = IPPROTO_ICMP          ,
				IGMP = IPPROTO_IGMP          ,
				GGP  = IPPROTO_GGP           ,
			#if(_WIN32_WINNT >= 0x0501)
				IPV4 = IPPROTO_IPV4          ,
			#endif//(_WIN32_WINNT >= 0x0501)
			#if(_WIN32_WINNT >= 0x0600)
				ST = IPPROTO_ST            ,
			#endif//(_WIN32_WINNT >= 0x0600)
				TCP = IPPROTO_TCP           ,
			#if(_WIN32_WINNT >= 0x0600)
				CBT = IPPROTO_CBT           ,
				EGP = IPPROTO_EGP           ,
				IGP = IPPROTO_IGP           ,
			#endif//(_WIN32_WINNT >= 0x0600)
				PUP = IPPROTO_PUP           ,
				UDP = IPPROTO_UDP           ,
				IDP = IPPROTO_IDP           ,
			#if(_WIN32_WINNT >= 0x0600)
				RDP = IPPROTO_RDP           ,
			#endif//(_WIN32_WINNT >= 0x0600)

			#if(_WIN32_WINNT >= 0x0501)
				IPV6        = IPPROTO_IPV6          , // IPv6 header
				Routing     = IPPROTO_ROUTING       , // IPv6 Routing header
				Fragment    = IPPROTO_FRAGMENT      , // IPv6 fragmentation header
				ESP         = IPPROTO_ESP           , // encapsulating security payload
				AuthHeader  = IPPROTO_AH            , // authentication header
				ICMPV6      = IPPROTO_ICMPV6        , // ICMPv6
				None        = IPPROTO_NONE          , // IPv6 no next header
				DestOptions = IPPROTO_DSTOPTS       , // IPv6 Destination options
			#endif//(_WIN32_WINNT >= 0x0501)
				ND,
			#if(_WIN32_WINNT >= 0x0501)
				ICLFXBM,
			#endif//(_WIN32_WINNT >= 0x0501)
			#if(_WIN32_WINNT >= 0x0600)
				PIM,
				PGM,
				L2TP,
				SCTP,
			#endif//(_WIN32_WINNT >= 0x0600)
				Raw,
			};

			enum class ESocketChannel : ui32
			{
				Recieve = SD_RECEIVE,
				Send    = SD_SEND,
				Both    = SD_BOTH
			};

			enum class ESocketOpResult : si32
			{
				Succes       = 0,
				SocketError  = SOCKET_ERROR
			};

			enum class ESocketStatus : ui32
			{
				Success            = 0,
				NotReady           = WSASYSNOTREADY,
				VerNotSupported    = WSAVERNOTSUPPORTED,
				BlockingInProgress = WSAEINPROGRESS,
				TaskLimitReached   = WSAEPROCLIM,
				Fault              = WSAEFAULT
			};

			using Socket = SOCKET;

			//using AddressFamily = ADDRESS_FAMILY;
			using AddressData = CHAR[14];

			struct SocketAddress : PlatformStruct_Base<sockaddr>
			{
				EAddressFamily  Family = EAddressFamily::Unspecified;
				AddressData     Address;
			};

			static Socket InvalidSocket() 
			{
				return INVALID_SOCKET; 
			};

			static Socket CreateSocket(EAddressFamily _famly, ESocketType _type, EProtocol _protocol);

			static ESocketOpResult BindSocket    (Socket _socket, const SocketAddress& _address, ui32 _length);
			static ESocketOpResult ShutdownSocket(Socket _socket, ESocketChannel _what);
			static ESocketOpResult CloseSocket   (Socket _socket);

			// Not exposed, this should be handled by loading OSAL, or by directly checking for windows
			// I'm not sure if any other platform uses this.
			using SocketSubsystemData = WSADATA;

			static SocketSubsystemData sockSubsysInfo;

			static ESocketStatus StartupSockSubsys();
			static ESocketStatus CleanupSockSubsys();
		#pragma endregion Sockets

			enum class EIPV
			{
				_4,
				_6
			};

			using IPV4_TRAddress = in_addr;
			using IPV4_PadType   = CHAR[8];

			struct SocketAddress_IPV4 : PlatformStruct_Base<sockaddr_in>
			{
				EAddressFamily  Family           = EAddressFamily::IPV4;
				u16             Port             = 0;
				IPV4_TRAddress  TransportAddress{};
				IPV4_PadType    Padding         {};
			};

			using IPV6_TRAddress = in6_addr;
			using IPV6_ScopeID   = SCOPE_ID;

			struct SocketAddress_IPV6 : PlatformStruct_Base<sockaddr_in6>
			{
				EAddressFamily Family           = EAddressFamily::IPV6;
				u16            Port             = 0;
				ui32           FlowInfo         = 0;
				IPV6_TRAddress TransportAddress{};
				union
				{
					ui32         ScopeID   = 0;
					IPV6_ScopeID ScopeInfo;
				};
			};

			enum class EIPPresToNetworkCode : si32
			{
				Success             = 1,
				InvalidPresentaiton = 0,
				Error               = -1
			};

			static EIPPresToNetworkCode IPV4AddressToNetwork   (RoCStr _presentation, IPV4_TRAddress& _ipv6Address_Out);
			static EIPPresToNetworkCode IPPresentationToNetwork(RoCStr _presentation, IPV6_TRAddress& _ipv6Address_Out);

			enum class ENamespace : ui32
			{
				All = NS_ALL,
			};

			enum class EAddressInfoCode : ui32
			{
				Success      = 0,
				TryAgain     = EAI_AGAIN,
				BadFlags     = EAI_BADFLAGS,
				Fail         = EAI_FAIL,
				Family       = EAI_FAMILY,
				Memory       = EAI_MEMORY,
				NoSecureName = EAI_NOSECURENAME,
				NoName       = EAI_NONAME,
				Service      = EAI_SERVICE,
				SocketType   = EAI_SOCKTYPE,
				IPSpecPolicy = EAI_IPSECPOLICY
			};

			struct AddressInfo : PlatformStruct_Base<ADDRINFOA>
			{
				ui32               Flags          = 0x0;
				EAddressFamily     Family         = EAddressFamily::Unspecified;
				ESocketType        SocketType     = ESocketType::Unspecified;
				ui32               Protocol       = 0;
				uDM                AddressLength  = 0;
				CStr               CannoncialName = nullptr;
				ptr<SocketAddress> Address        = nullptr;
				ptr<AddressInfo>   Next           = nullptr;
			};

			static EAddressInfoCode GetAddressInfo     (RoCStr _nodeName, RoCStr _serviceName, const AddressInfo& _hints, AddressInfo& _info_out);
			static EAddressInfoCode GetAddressInfoAsync(RoCStr _nodeName, RoCStr _serviceName, const AddressInfo& _hints, AddressInfo& _info_out);

			static void FreeAddressInfo(ptr<AddressInfo> _addressInfo_in);
		};

		using NetworkingAPI = NetworkingAPI_Maker<OSAL::OS>;
	}

	using PlatformBackend::NetworkingAPI;

	using EAddressFamily = NetworkingAPI::EAddressFamily;
	
#pragma region Sockets
	using ESocketType     = NetworkingAPI::ESocketType;
	using EProtocol       = NetworkingAPI::EProtocol;
	using ESocketOpResult = NetworkingAPI::ESocketOpResult;
	using ESocketChannel  = NetworkingAPI::ESocketChannel;
	using ESocketStatus   = NetworkingAPI::ESocketStatus;
	using Socket          = NetworkingAPI::Socket;

	constexpr auto InvalidSocket  = NetworkingAPI::InvalidSocket;
	constexpr auto CreateSocket   = NetworkingAPI::CreateSocket;
	constexpr auto BindSocket     = NetworkingAPI::BindSocket;
	constexpr auto ShutdownSocket = NetworkingAPI::ShutdownSocket;
	constexpr auto CloseSocket    = NetworkingAPI::CloseSocket;
#pragma endregion Sockets

	//using AddressFamily        = NetworkingAPI::AddressFamily;
	using AddressData          = NetworkingAPI::AddressData;
	using AddressInfo          = NetworkingAPI::AddressInfo;
	using EAddressInfoCode     = NetworkingAPI::EAddressInfoCode;
	using EIPV                 = NetworkingAPI::EIPV;
	using EIPPresToNetworkCode = NetworkingAPI::EIPPresToNetworkCode;
	using IPV4_TRAddress       = NetworkingAPI::IPV4_TRAddress;
	using IPV6_TRAddress       = NetworkingAPI::IPV6_TRAddress;
	using SocketAddress        = NetworkingAPI::SocketAddress;
	using SocketAddress_IPV4   = NetworkingAPI::SocketAddress_IPV4;
	using SocketAddress_IPV6   = NetworkingAPI::SocketAddress_IPV6;

	template<typename ReturnType, typename ArgumentType>
	static Where<IsArithmetic<ArgumentType>(),
	ReturnType> HostToNetwork(ArgumentType _hostData)
	{
		if constexpr (IsFloat<ArgumentType>())
		{
			return IsSameType<f32, ArgumentType>() ?
				htonf(_hostData) :
				htond(_hostData)
			;
		}
		else if constexpr (IsSameType<u16, ArgumentType>())
		{
			return htons(_hostData);
		}
		else if constexpr (IsSameType<u32, ArgumentType>())
		{
			return htonl(_hostData);
		}
		else if constexpr (IsSameType<u64, ArgumentType>())
		{
			return htonll(_hostData);
		}
		else
		{
			Exception::Fatal::NoEntry("HostToNetwork");

			return 0;
		}
	}

	template<typename ReturnType, typename ArgumentType>
	static  Where<IsArithmetic<ArgumentType>(),
	ReturnType> NetworkToHost(ArgumentType _hostData)
	{
		if constexpr (IsFloat<ArgumentType>())
		{
			return IsSameType<f32, ArgumentType>() ?
				ntohf(_hostData) :
				ntohd(_hostData)
			;
		}
		else if constexpr (IsSameType<u16, ArgumentType>())
		{
			return ntohs(_hostData);
		}
		else if constexpr (IsSameType<u32, ArgumentType>())
		{
			return ntohl(_hostData);
		}
		else if constexpr (IsSameType<u64, ArgumentType>())
		{
			return ntohll(_hostData);
		}
		else constexpr
		{
			Exception::Fatal::NoEntry("HostToNetwork");

			return 0;
		}
	}

	constexpr fnPtr<EIPPresToNetworkCode, RoCStr, IPV4_TRAddress&> IPV4AddressToNetwork    = NetworkingAPI::IPV4AddressToNetwork;
	constexpr fnPtr<EIPPresToNetworkCode, RoCStr, IPV6_TRAddress&> IPPresentationToNetwork = NetworkingAPI::IPPresentationToNetwork;

	constexpr auto GetAddressInfo  = NetworkingAPI::GetAddressInfo;
	constexpr auto FreeAddressInfo = NetworkingAPI::FreeAddressInfo;

	void Load_Networking();
	void Unload_Networking();
}
