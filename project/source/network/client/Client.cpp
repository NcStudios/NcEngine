#include "Client.h"
#include "../NetworkDetails.h"
#include "../Packet.h"

#include <iostream>
#include <memory>

namespace project::network
{
    Client::Client()
    {
        m_server = nullptr;
        host = enet_host_create(nullptr, 1, nc::net::CHANNEL_LIMIT, 0, 0);
        if(!host)
        {
            throw std::runtime_error("Failed to create client");
        }
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

    bool Client::Connect(const char* hostAddress, std::string& playerName)
    {
        ENetEvent event;
        ENetPeer* peer;
        enet_address_set_host(&address, hostAddress);
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
            PacketClientSendName packet(playerName);
            Send(packet.ToPacketBuffer(), nc::net::Channel::Reliable, m_server);
            return true;
        }
        else
        {
            std::cout << "Failed to connect\n";
            return false;
        }
    }
}