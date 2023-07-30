#define ENET_IMPLEMENTATION
#include "enet.h"

#include <iostream>

#define LOG(...) std::cout << __VA_ARGS__ << std::endl

int main() {
    int r = enet_initialize();

    ENetHost * client = enet_host_create( NULL, 1, 2, 0, 0 );
    if( client == nullptr ) {
        LOG( "Could not create client" );
        return 0;
    }

    ENetAddress address = {};
    enet_address_set_host( &address, "127.0.0.1" );
    address.port = 27164;

    ENetPeer * peer = enet_host_connect( client, &address, 2, 0 );
    if( peer == nullptr ) {
        LOG( "Could not create peer" );
        return 0;

    }

    bool isConnected = false;
    ENetEvent event = {};
    if( enet_host_service( client, &event, 5000 ) > 0 && event.type == ENET_EVENT_TYPE_CONNECT ) {
        isConnected = true;
    }

    if( isConnected ) {
        LOG( "Connected sluts" );
        
        while( enet_host_service( client, &event, 30 ) >= 0 ) {
            if( event.type == ENET_EVENT_TYPE_NONE ) {
                //LOG( "ENET_EVENT_TYPE_NONE" );
            }
            else if( event.type == ENET_EVENT_TYPE_CONNECT ) {
                LOG( "ENET_EVENT_TYPE_CONNECT " );
            }
            else if( event.type == ENET_EVENT_TYPE_DISCONNECT ) {
                LOG( "ENET_EVENT_TYPE_DISCONNECT" );
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
