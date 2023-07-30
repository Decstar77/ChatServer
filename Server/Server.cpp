#define ENET_IMPLEMENTATION
#include "enet.h"

#include <iostream>

#define LOG(...) std::cout << __VA_ARGS__ << std::endl

int main()
{
    enet_initialize();

    ENetAddress address = {};
    //enet_address_set_host( &address, "127.0.0.1" );
    address.host = ENET_HOST_ANY;
    address.port = 27164;

    ENetHost * server = enet_host_create( &address, 32, 2, 0, 0 );
    if( server == nullptr ) {
        LOG( "Failed to create ENet server host." );
        return 0;
    }

    LOG( "Server started " );
    char serverIp[ 256 ] = {};
    if( enet_address_get_host_ip( &server->address, serverIp, sizeof( serverIp ) ) == 0 ) {
        LOG( "IP " << serverIp );
    }

    ENetEvent event = {};
    while( enet_host_service( server, &event, 500 ) >= 0 ) {
        switch( event.type ) {
            case ENET_EVENT_TYPE_CONNECT:
            {
                char hostIp[ 256 ] = {};
                if( enet_address_get_host_ip( &event.peer->address, hostIp, sizeof( hostIp ) ) == 0 ) {
                    LOG( "Connection from " << hostIp );
                }
                else {
                    LOG( "Unable to retrieve IP address" );
                }

            } break;
            case ENET_EVENT_TYPE_RECEIVE:
            {
                char hostIp[ 256 ] = {};
                if( enet_address_get_host_ip( &event.peer->address, hostIp, sizeof( hostIp ) ) == 0 ) {
                    LOG( "Message from " << hostIp );
                    char textBuffer[ 256 ] = {};
                    if( event.packet->dataLength < 256 ) {
                        memcpy( textBuffer, event.packet->data, event.packet->dataLength );
                        LOG( textBuffer );
                    }
                    else {
                        LOG( "Msg to big, ignoring it" );
                    }
                }
                else {
                    LOG( "Unable to retrieve IP address" );
                }
            } break;
            case ENET_EVENT_TYPE_DISCONNECT:
            {
                LOG( "Disconnection" );
            } break;
            case ENET_EVENT_TYPE_DISCONNECT_TIMEOUT:
            {
                LOG( "Disconnection - Timeout" );
            } break;
            case ENET_EVENT_TYPE_NONE:
            {

            } break;
        }
    }

    enet_host_destroy( server );
    enet_deinitialize();

    return 0;
}
