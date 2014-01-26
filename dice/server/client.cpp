#include <enet/enet.h>

#include <iostream>

using namespace std;

bool quit = false;

ENetHost *client;
ENetPeer *server;

bool setupNetwork()
{
	// initialize enet
	if (enet_initialize () != 0)
	{
		cerr << "error initializing enet" << endl;
		return false;
	}

	atexit (enet_deinitialize);


	// create a client
	cout << "creating client" << endl;

	// create client with 2 channels, and no bandwidth limits
	client = enet_host_create (NULL, 1, 2, 0,  0);

	if (!client)
	{
		cerr << "error creating client" << endl;
		return false;
	}

	// connect to server
	ENetAddress address;

	string ip_addr;
	cout << "IP Address: ";
	cin >> ip_addr;

	enet_address_set_host (&address, ip_addr.c_str());
	address.port = 1921;

	server = enet_host_connect (client, &address, 2, 0);

	if (!server)
	{
		cerr << "no more available outgoing connections available" << endl;
		exit (EXIT_FAILURE);
	}

	// wait up to 5 seconds for the connection attempt to succeed
	ENetEvent event;
	if (enet_host_service (client, &event, 5000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT)
	{
		cout << "connected to server" << endl;
	}
	else
	{
		cerr << "failed to connect to server" << endl;
		enet_peer_reset (server);
		return false;
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

		string message;
		cin >> message;

		if (message.compare("quit") == 0)
		{
			quit = true;
		}
		else
		{
			ENetPacket *packet = enet_packet_create (&message,  message.length() +1, ENET_PACKET_FLAG_RELIABLE);

			enet_peer_send (server, 0, packet);
			enet_host_flush(client);
		}
	}
}
