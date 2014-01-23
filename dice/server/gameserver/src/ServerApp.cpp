/*
 * File:	ServerApp.cpp
 * Author:	James Letendre
 *
 */

#include <vector>
#include <cassert>

#include "perlinNoise.h"

#include "ServerApp.h"
#include "PolyVoxCore/SimpleInterface.h"
#include "PolyVoxCore/CubicSurfaceExtractor.h"
#include "PolyVoxCore/SurfaceMesh.h"
#include "PolyVoxCore/Raycast.h"

#include <OgreWindowEventUtilities.h>

#define VOXEL_SCALE 1.0

using namespace std;

bool quit = false;

void kill_sig( int code )
{
	quit = true;
}

//-------------------------------------------------------------------------------------
ServerApp::ServerApp(void) : terrain(NULL)
{
}
//-------------------------------------------------------------------------------------
ServerApp::~ServerApp(void)
{
	if(terrain) delete terrain;
}
//-------------------------------------------------------------------------------------
bool ServerApp::setupNetwork(void)
{
	// initialize ENet
	if( enet_initialize() != 0 )
	{
		cerr << "Error initializing ENet" << endl;
		return false;
	}

	// add hook to run on exit
	atexit(enet_deinitialize);

	ENetAddress address;

	address.host = ENET_HOST_ANY;
	address.port = 4321;

	cout << "Creating server" << endl;
	// create server with max 32 clients, 2 channels and now bandwidth limits
	server = enet_host_create( &address, 32, 2, 0, 0 );

	if( !server )
	{
		cerr << "Error creating ENet server" << endl;
		return false;
	}

	return true;
}
//-------------------------------------------------------------------------------------
void ServerApp::syncToClient( ENetPeer *peer )
{
	// send play connection packets for each connected player, other than us
	for( auto player : players )
	{
		if( player.getId() != peer->address.host )
		{
			Packet *syncPacket = player.serialize();
			syncPacket->type = PLAYER_SYNC;
			syncPacket->send(peer, ENET_PACKET_FLAG_RELIABLE );
			delete syncPacket;
		}
	}
}
//-------------------------------------------------------------------------------------
void ServerApp::go(void)
{
	if (!setup())
		return;

	terrain = new TerrainPager(NULL, NULL, this);
	
	while( !quit )
	{
		//Pump messages in all registered RenderWindow windows
		Ogre::WindowEventUtilities::messagePump();

		// handle network
		ENetEvent event;
		std::string playerName;
		bool newPlayer;
		Packet recvPacket;
		Packet respPacket;

		int32_t vecX, vecY, vecZ;
		uint8_t mat;

		if( enet_host_service( server, &event, 100 ) > 0 )
		{
			newPlayer = false;

			switch( event.type )
			{
				case ENET_EVENT_TYPE_CONNECT:
					// new connection request
					cout << "New connection request from " << event.peer->address.host << ":" << event.peer->address.port << endl;

					event.peer->data = NULL;

					respPacket.clear();
					respPacket.type = CONNECTION_CLIENT_ID;
					respPacket.push( event.peer->address.host );

					respPacket.send( event.peer, ENET_PACKET_FLAG_RELIABLE );
					break;

				case ENET_EVENT_TYPE_RECEIVE:
					// new data from a existing client
					recvPacket.unserialize( event.packet->data, event.packet->dataLength );

					switch( recvPacket.type )
					{
						case CONNECTION_REQUEST_SYNC:
							syncToClient( event.peer );
							break;
						case CONNECTION_SYNC_FINISHED:
							// not used in server
							break;
						case PLAYER_SET_USERNAME:
							// free old name if present
							if( event.peer->data )
							{
								cout << "Client " << (char*)event.peer->data << " now known as ";
								free( event.peer->data );
								newPlayer = false;
							}
							else
							{
								cout << "Client " << event.peer->address.host << " known as ";
								newPlayer = true;

							}

							playerName.clear();
							while( recvPacket.getSize() )
							{
								playerName.push_back( recvPacket.pop<char>() );
							}
							event.peer->data = strdup( playerName.c_str() );

							cout << (char*)event.peer->data << endl;

							if( newPlayer )
							{
								playerName = std::string( (char*)event.peer->data );
								// add player to game
								players.push_back( PlayerEntity(event.peer->address.host, playerName) );

								// broadcast to all clients
								Packet *joinPacket = players[players.size()-1].serialize();
								joinPacket->type = PLAYER_SYNC;

								joinPacket->broadcast(server, ENET_PACKET_FLAG_RELIABLE);
							}

							break;
						case PLAYER_CONNECT:
							break;
						case PLAYER_DISCONNECT:
							break;
						case PLAYER_MOVE:
							break;

						case TERRAIN_REQUEST:
							terrain->request( recvPacket, event.peer );
							break;
						case TERRAIN_RESPONSE:
							// not used in server
							break;
						case TERRAIN_UPDATE:
							// update this pixel
							vecX = recvPacket.pop<int32_t>();
							vecY = recvPacket.pop<int32_t>();
							vecZ = recvPacket.pop<int32_t>();

							mat = recvPacket.pop<uint8_t>();

							// TODO verify change can occur
							if( true )
							{
								terrain->setVoxelAt(PolyVox::Vector3DInt32(vecX, vecY, vecZ), PolyVox::Material8(mat));

								// notify clients of the change
								respPacket.clear();
								respPacket.type = TERRAIN_UPDATE;

								respPacket.push(vecX);
								respPacket.push(vecY);
								respPacket.push(vecZ);
								respPacket.push(mat);

								respPacket.broadcast( server, ENET_PACKET_FLAG_RELIABLE );
							}
							else
							{
								// notify client that that change didn't in fact happen
								respPacket.clear();
								respPacket.type = TERRAIN_UPDATE;

								respPacket.push(vecX);
								respPacket.push(vecY);
								respPacket.push(vecZ);
								respPacket.push(terrain->getVoxelAt(PolyVox::Vector3DInt32(vecX, vecY, vecZ)).getMaterial());

								respPacket.send( event.peer, ENET_PACKET_FLAG_RELIABLE );
							}

							break;

						default:
							break;
					}
					break;

				case ENET_EVENT_TYPE_DISCONNECT:
					// client disconnect
					cout << "Client "  << (char*)event.peer->data << " disconnect" << endl;

					// delete the player
					playerName = (char*)event.peer->data;
					for( size_t i = 0; i < players.size(); i++ )
					{
						if( playerName == players[i].getName() )
						{
							Packet dropPacket;
							dropPacket.type = PLAYER_DISCONNECT;
							for( char c : playerName )
							{
								dropPacket.push(c);
							}
							
							dropPacket.broadcast(server, ENET_PACKET_FLAG_RELIABLE);

							players.erase( players.begin() + i );
							break;
						}
					}

					free(event.peer->data);
					event.peer->data = NULL;

					break;
				case ENET_EVENT_TYPE_NONE:
					// nothing happened
					break;
			}
		}
	}

}
//-------------------------------------------------------------------------------------
bool ServerApp::setup(void)
{
	if( !BaseApplication::setup() )
		return false;

	setupNetwork();

	return true;
}

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
	INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
#else
		int main(int argc, char *argv[])
#endif
		{
			signal( SIGKILL, kill_sig );
			signal( SIGTERM, kill_sig );
			signal( SIGINT,  kill_sig );

			// Create application object
			initPerlinNoise();

			ServerApp app;

			try {
				app.go();
			} catch( Ogre::Exception& e ) {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
				MessageBox( NULL, e.getFullDescription().c_str(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
				std::cerr << "An exception has occured: " <<
					e.getFullDescription().c_str() << std::endl;
#endif
			}

			return 0;
		}

#ifdef __cplusplus
}
#endif
