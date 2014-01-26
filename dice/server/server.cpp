#include <enet/enet.h>

#include <iostream>

using namespace std;

bool quit = false;
ENetHost *server;

bool setupNetwork()
{
	if (enet_initialize () != 0)
	{
		cerr << "error initializing enet" << endl;
		return false;
	}

	atexit (enet_deinitialize);

	ENetAddress address;

	address.host = ENET_HOST_ANY;
	address.port = 1921;

	cout << "creating server" << endl;

	// create server with max 32 clients, 2 channels, and no bandwidth limits
	server = enet_host_create (&address, 32, 2, 0,  0);

	if (!server)
	{
		cerr << "error creating enet server" << endl;
		return false;
	}
	else
	{
		cout << "server created on " << address.host << ":1921" << endl;
	}

	return true;
}

int main (int argc, char* argv[])
{
	if (!setupNetwork())
	{
		return 0;
	}

	while (!quit)
	{
		ENetEvent event;
		//Packet recvPacket;
		//Packet respPacket;

		if (enet_host_service (server, &event, 100) > 0)
		{
			cout << "event" << endl;
			switch (event.type)
			{
				case ENET_EVENT_TYPE_CONNECT:
					// new connection request
					cout << "new connection request from " << event.peer->address.host << ":" << event.peer->address.port << endl;

					/*
					event.peer->data = NULL;

					respPacket.clear();
					respPacket.type = CONNECTION_CLIENT_ID;
					respPacket.push (event.peer->address.host);

					respPacket.send (event.peer ENET_PACKET_FLAG_RELIABLE);
					*/
					break;
				
				case ENET_EVENT_TYPE_RECEIVE:
					cout << "recieved packet" << endl;
					cout << "packet length   : " << event.packet->dataLength << endl;
					cout << "packet contents : " << event.packet->data << endl;
					cout << "packet source   : " << event.peer->data << endl;
					cout << "packet chennel  : " << event.channelID << endl;
					cout << endl;

					enet_packet_destroy (event.packet);
					break;
				case ENET_EVENT_TYPE_DISCONNECT:
					// client disconnect
					cout << "client " << (char*)event.peer->data << " disconnect" << endl;

					break;
				default:
					break;
			}
		}
	}
}
