#ifndef CLIENT_CONTEXT_H__
#define CLIENT_CONTEXT_H__

#include <map>
#include <mutex>
#include <memory>
#include <boost/asio.hpp>
#include "Destination.h"
#include "I2PService.h"
#include "HTTPProxy.h"
#include "SOCKS.h"
#include "I2PTunnel.h"
#include "SAM.h"
#include "BOB.h"
#include "I2CP.h"
#include "AddressBook.h"

namespace i2p
{
namespace client
{
	const char I2P_TUNNELS_SECTION_TYPE[] = "type";
	const char I2P_TUNNELS_SECTION_TYPE_CLIENT[] = "client";
	const char I2P_TUNNELS_SECTION_TYPE_SERVER[] = "server";
	const char I2P_TUNNELS_SECTION_TYPE_HTTP[] = "http";
	const char I2P_TUNNELS_SECTION_TYPE_IRC[] = "irc";
	const char I2P_TUNNELS_SECTION_TYPE_UDPCLIENT[] = "udpclient";
	const char I2P_TUNNELS_SECTION_TYPE_UDPSERVER[] = "udpserver";
	const char I2P_CLIENT_TUNNEL_PORT[] = "port";
	const char I2P_CLIENT_TUNNEL_ADDRESS[] = "address";
	const char I2P_CLIENT_TUNNEL_DESTINATION[] = "destination";
	const char I2P_CLIENT_TUNNEL_KEYS[] = "keys";
	const char I2P_CLIENT_TUNNEL_SIGNATURE_TYPE[] = "signaturetype";
	const char I2P_CLIENT_TUNNEL_DESTINATION_PORT[] = "destinationport";	
	const char I2P_SERVER_TUNNEL_HOST[] = "host";	
	const char I2P_SERVER_TUNNEL_HOST_OVERRIDE[] = "hostoverride";	
	const char I2P_SERVER_TUNNEL_PORT[] = "port";
	const char I2P_SERVER_TUNNEL_KEYS[] = "keys";
	const char I2P_SERVER_TUNNEL_SIGNATURE_TYPE[] = "signaturetype";
	const char I2P_SERVER_TUNNEL_INPORT[] = "inport";
	const char I2P_SERVER_TUNNEL_ACCESS_LIST[] = "accesslist";		
	const char I2P_SERVER_TUNNEL_GZIP[] = "gzip";
	const char I2P_SERVER_TUNNEL_WEBIRC_PASSWORD[] = "webircpassword";
	const char I2P_SERVER_TUNNEL_ADDRESS[] = "address";
	
	class ClientContext
	{
		public:

			ClientContext ();
			~ClientContext ();

			void Start ();
			void Stop ();

			void ReloadConfig ();

			std::shared_ptr<ClientDestination> GetSharedLocalDestination () const { return m_SharedLocalDestination; };
			std::shared_ptr<ClientDestination> CreateNewLocalDestination (bool isPublic = false, i2p::data::SigningKeyType sigType = i2p::data::SIGNING_KEY_TYPE_DSA_SHA1,
			    const std::map<std::string, std::string> * params = nullptr); // transient
			std::shared_ptr<ClientDestination> CreateNewLocalDestination (const i2p::data::PrivateKeys& keys, bool isPublic = true, 
				const std::map<std::string, std::string> * params = nullptr);
			void DeleteLocalDestination (std::shared_ptr<ClientDestination> destination);
			std::shared_ptr<ClientDestination> FindLocalDestination (const i2p::data::IdentHash& destination) const;		
			bool LoadPrivateKeys (i2p::data::PrivateKeys& keys, const std::string& filename, i2p::data::SigningKeyType sigType = i2p::data::SIGNING_KEY_TYPE_ECDSA_SHA256_P256);

			AddressBook& GetAddressBook () { return m_AddressBook; };
			const SAMBridge * GetSAMBridge () const { return m_SamBridge; };

			std::vector<std::shared_ptr<DatagramSessionInfo> > GetForwardInfosFor(const i2p::data::IdentHash & destination);
			
		private:

			void ReadTunnels ();
			template<typename Section, typename Type>
			std::string GetI2CPOption (const Section& section, const std::string& name, const Type& value) const;
			template<typename Section>
			void ReadI2CPOptions (const Section& section, std::map<std::string, std::string>& options) const;	

			void CleanupUDP(const boost::system::error_code & ecode);
			void ScheduleCleanupUDP();
			
		private:

			std::mutex m_DestinationsMutex;
			std::map<i2p::data::IdentHash, std::shared_ptr<ClientDestination> > m_Destinations;
			std::shared_ptr<ClientDestination>  m_SharedLocalDestination;	

			AddressBook m_AddressBook;

			i2p::proxy::HTTPProxy * m_HttpProxy;
			i2p::proxy::SOCKSProxy * m_SocksProxy;
			std::map<boost::asio::ip::tcp::endpoint, std::unique_ptr<I2PClientTunnel> > m_ClientTunnels; // local endpoint->tunnel
			std::map<std::pair<i2p::data::IdentHash, int>, std::unique_ptr<I2PServerTunnel> > m_ServerTunnels; // <destination,port>->tunnel

			std::mutex m_ForwardsMutex;			 
			std::map<boost::asio::ip::udp::endpoint, std::unique_ptr<I2PUDPClientTunnel> > m_ClientForwards; // local endpoint -> udp tunnel
			std::map<std::pair<i2p::data::IdentHash, int>, std::unique_ptr<I2PUDPServerTunnel> > m_ServerForwards; // <destination,port> -> udp tunnel
			
			SAMBridge * m_SamBridge;
			BOBCommandChannel * m_BOBCommandChannel;
			I2CPServer * m_I2CPServer;

			std::unique_ptr<boost::asio::deadline_timer> m_CleanupUDPTimer;
			
		public:
			// for HTTP
			const decltype(m_Destinations)& GetDestinations () const { return m_Destinations; };
			const decltype(m_ClientTunnels)& GetClientTunnels () const { return m_ClientTunnels; };
			const decltype(m_ServerTunnels)& GetServerTunnels () const { return m_ServerTunnels; };
			const decltype(m_ClientForwards)& GetClientForwards () const { return m_ClientForwards; }
			const decltype(m_ServerForwards)& GetServerForwards () const { return m_ServerForwards; }
	};
	
	extern ClientContext context;	
}		
}	

#endif
