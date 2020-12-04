#include "Client.h"
#include "DebugUtils.h"
#include "Network.h"
#include "../Packet.h"

#include <functional>
#include <iostream>
#include <memory>

namespace project::network
{
    Client::Client(std::string serverIP)
        : m_server{ nullptr },
          m_networkPrefabManager{},
          m_coordinator{&m_networkPrefabManager},
          m_serverIP{std::move(serverIP)}
    {
        host = enet_host_create(nullptr, 1, nc::net::ChannelLimit, 0, 0);
        if(!host)
        {
            throw std::runtime_error("Failed to create client");
        }
        nc::net::Network::BindServerCommandCallback(std::bind(SendToServer, this, std::placeholders::_1));
    }

    Client::~Client()
    {
        enet_host_destroy(host);
    }

    void Client::Poll(uint32_t timeout)
    {
        ENetEvent event;
        while(enet_host_service(host, &event, timeout) > 0)
        {
            if(event.type == ENET_EVENT_TYPE_RECEIVE)
            {
                m_coordinator.Dispatch(&event);
                enet_packet_destroy(event.packet);
            }
        }
    }

    void Client::Send(nc::net::PacketBuffer data, nc::net::Channel channel, ENetPeer* peer)
    {
        ENetPacket* packet = enet_packet_create(data.data, data.size, ENET_PACKET_FLAG_RELIABLE);
        enet_peer_send(peer, (enet_uint8)channel, packet);
    }

    void Client::SendToServer(nc::net::PacketBuffer data)
    {
        IF_THROW(!m_server, "Client::SendToServer - m_server is not set");
        ENetPacket* packet = enet_packet_create(data.data, data.size, ENET_PACKET_FLAG_RELIABLE);
        enet_peer_send(m_server, (enet_uint8)nc::net::Channel::Reliable, packet);
    }

    bool Client::Connect(std::string& playerName)
    {
        ENetEvent event;
        ENetPeer* peer;
        enet_address_set_host(&address, m_serverIP.c_str());
        address.port = 1234;

        peer = enet_host_connect(host, &address, 2, 0);
        if(!peer)
        {
            std::cerr << "No available peers\n";
            return false;
        }
        if(enet_host_service(host, &event, 3000) > 0 &&
            event.type == ENET_EVENT_TYPE_CONNECT)
        {
            std::cout << "Connected to server\n";
            m_server = peer;
            Packet::ClientSendName packet{.name = playerName};
            Send(ToPacketBuffer(&packet), nc::net::Channel::Reliable, m_server);
            return true;
        }
        else
        {
            std::cout << "Failed to connect\n";
            return false;
        }
    }
}