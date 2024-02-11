#pragma once
#include <thread>
#include <iostream>
#include <ws2tcpip.h>
#include "Helper.h"
#include "Cryptor.h"
#include "RouterData.h"

constexpr uint16_t RANDOM_PORT = 0;
constexpr uint8_t IFACE = 0;

class Communicator {
private:
	SOCKET m_serverSocket;
	SOCKET m_supernodeSocket;
	uint16_t m_listeningPort;

	//General:
	static void ConnectToServer(SOCKET& socket, const std::string& ip, const uint16_t port); //Connect to the server.

	//Supernode methods:
	void joinNetwork(void); //Join as a router.
	//void leaveNetwork(void); //Stop being a router.
	const bool checkStatus(void); //Checks the status of the message from the supernode.
	

	//Server methods:
	void bindAndListen(void);
	void acceptClient(void);
	static void handleNewClient(const SOCKET& clientSocket);

	//Onion Routing Methods:
	static void createAndSendDhPublicKey(const SOCKET& clientSocket, Cryptor& cryptor); //Sends the Diffie-Hellman public key.
	static const RouterData readOtherDhPublicKeyAndAgree(const SOCKET& clientSocket, Cryptor& cryptor); //Reads the Diffie-Hellman public key of the second user and create a Diffie-Hellman shared key.
	static void passHandshake(const SOCKET& clientSocket, const SOCKET& nextRouterSocket, Cryptor& cryptor, const uint8_t routerIndex); //Passes the handshake to the next routers.
	static const RouterData dhHandshake(const SOCKET& clientSocket, Cryptor& cryptor); //Performs a handshake with Diffie-Hellman encryption to get an AES key.
	static const SOCKET fullHandshake(const SOCKET& clientSocket, Cryptor& cryptor); //Performs a full handshake.
	static const CryptoPP::SecByteBlock deconstructLayer(Cryptor& cryptor, const CryptoPP::SecByteBlock& onionPacket);
	static const CryptoPP::SecByteBlock addEncryptionLayer(Cryptor& cryptor, const CryptoPP::SecByteBlock& onionPacket);

	//Simple sends/reads:
	static void sendInteger(const SOCKET& clientSocket, const Integer& integer); //Sends an integer and its size with the socket.
	static void sendSecByteBlock(const SOCKET& clientSocket, const CryptoPP::SecByteBlock& secByteBlock); //Sends a SecByteBlock and its size with the socket.
	static void sendByteArray(const SOCKET& clientSocket, const byte* array, const uint32_t size); //Sends a byte array and its size with the socket.
	static const std::vector<byte> readByteVector(const SOCKET& clientSocket); //Reads a byte array from the socket and outputs as a byte vector.
	static const CryptoPP::SecByteBlock readSecByteBlock(const SOCKET& clientSocket); //Reads a byte array from the socket and outputs as a SecByteBlock.
	static const Integer readInteger(const SOCKET& clientSocket); //Reads an Integer from the socket.

public:
	Communicator(const std::string& ip, const uint16_t port);
	~Communicator(void);

	void startHandleRequests(void);
};