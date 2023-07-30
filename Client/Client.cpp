#define ENET_IMPLEMENTATION
#include "enet.h"

#include <iostream>
#include <vector>
#include <atomic>
#include <mutex>
#include <thread>

#define LOG(...) std::cout << __VA_ARGS__ << std::endl

std::mutex sendLock;
std::vector<std::string> sendQueue;
std::atomic_bool closeConnection = false;

static void NetworkThreadLogic() {
    ENetHost * client = enet_host_create( NULL, 1, 2, 0, 0 );
    if( client == nullptr ) {
        LOG( "Could not create client" );
        return;
    }

    ENetAddress address = {};
    //enet_address_set_host( &address, "62.72.16.38" );
    enet_address_set_host( &address, "127.0.0.1" );
    address.port = 27164;

    ENetPeer * peer = enet_host_connect( client, &address, 2, 0 );
    if( peer == nullptr ) {
        LOG( "Could not create peer" );
        return;
    }

    bool isConnected = false;
    ENetEvent event = {};
    if( enet_host_service( client, &event, 5000 ) > 0 && event.type == ENET_EVENT_TYPE_CONNECT ) {
        isConnected = true;
    }

    if( isConnected ) {
        LOG( "Connected sluts" );

        while( enet_host_service( client, &event, 30 ) >= 0 && isConnected ) {
            if( closeConnection ) {
                enet_peer_disconnect( peer, 0 );
            }

            sendLock.lock();
            for (int i = 0; i < sendQueue.size(); i++ ) {
                std::string & msg = sendQueue[ i ];
                ENetPacket * packet = enet_packet_create( msg.c_str(), msg.length() + 1, ENET_PACKET_FLAG_RELIABLE );
                enet_peer_send( peer, 0, packet );
            }
            sendQueue.clear();
            sendLock.unlock();

            if( event.type == ENET_EVENT_TYPE_NONE ) {
                //LOG( "ENET_EVENT_TYPE_NONE" );
            }
            else if( event.type == ENET_EVENT_TYPE_CONNECT ) {
                LOG( "ENET_EVENT_TYPE_CONNECT " );
            }
            else if( event.type == ENET_EVENT_TYPE_DISCONNECT ) {
                LOG( "ENET_EVENT_TYPE_DISCONNECT" );
                isConnected = false;
            }
            else if( event.type == ENET_EVENT_TYPE_RECEIVE ) {
                LOG( "ENET_EVENT_TYPE_RECEIVE" );
            }
            else {
                LOG( "WHAT" );
            }
        }
    }

    enet_peer_reset( peer );
    enet_host_destroy( client );
    enet_deinitialize();
}

int main() {
    if( enet_initialize() != 0 ) {
        LOG( "Could not init ENet" );
        return 0;
    }

    std::thread networkThread( NetworkThreadLogic );

    while( true ) {
        char buf[ 256 ] = {};
        std::cin >> buf;
        
        if( strcmp( buf, "quit" ) == 0 ) {
            closeConnection = true;
            break;
        }

        if( strcmp( buf, "send" ) == 0 ) {
            std::cin >> buf;
            sendLock.lock();
            sendQueue.push_back( buf );
            sendLock.unlock();
        }
    }

    networkThread.join();
}
