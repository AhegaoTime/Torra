#pragma once
#pragma warning(disable:4996)
#include <iostream>
#include <ws2tcpip.h>
#include "Helper.h"
#include "Router.h"
#include "json.hpp"

using Json = nlohmann::json;

constexpr uint8_t ROUTERS_AMOUNT = 3;

class Communicator {
private:
	SOCKET m_onionSocket; //The Socket for the onion route.
	SOCKET m_supernodeSocket; //The Socket for communicating with the Supernode.
	std::vector<Router> m_routers;
	std::string m_entryGuardIp;
	uint16_t m_entryGuardPort;

	//General:
	static void ConnectToServer(SOCKET& socket, const std::string& ip, const uint16_t port); //Connect to the server.

	//Communication with the Supernode:
	void requestPathFromSupernode(void); //Requests the 3 routers path from the supernode.
	const bool checkStatus(void); //Checks the status of the message from the supernode.
	void parsePathFromSupernode(void); //Reads a path from the supernode and parses it.

	//Handshake:
	void dhHandshake(const uint8_t routerIndex); //Performs a handshake with Diffie-Hellman to send an AES key.
	void dhHandshakeFull(void); //Performs a handshake with Diffie-Hellman to send an AES key with all of the routers.
	void createAndSendDhPublicKey(const uint8_t routerIndex); //Sends the Diffie-Hellman public key.
	void readOtherDhPublicKeyAndAgree(const uint8_t routerIndex); //Reads the Diffie-Hellman public key of the second user and create a Diffie-Hellman shared key.
	void addTargetServerData(const std::string& ip, const uint16_t port); //Adds the target server to the routers list, not really a router though.

	//Packets' constrcution and deconstruction:
	const CryptoPP::SecByteBlock constructOnionPacket(const CryptoPP::SecByteBlock& data, uint8_t routerIndex = ROUTERS_AMOUNT) const;
	const CryptoPP::SecByteBlock deconstructOnionPacket(const CryptoPP::SecByteBlock& onionPacket, const uint8_t routerIndex) const;

	//Simple sends/reads:
	void sendInteger(const Integer& integer) const; //Sends an integer and its size with the socket.
	void sendString(const std::string& string) const; //Sends a string and its size with the socket.
	void sendByteArray(const byte* array, const uint32_t size) const; //Sends a byte array and its size with the socket.
	void sendSecByteBlock(const CryptoPP::SecByteBlock& secByteBlock) const; //Sends a SecByteBlock and its size with the socket.
	const Integer readInteger(void) const; //Reads an Integer from the socket.
	const CryptoPP::SecByteBlock readSecByteBlock(uint32_t size = 0) const; //Reads a SecByteBlock from the socket.

public:
	//Constructors:
	Communicator(void);

	//main:
	void run(const std::string& ip, const uint16_t port); //The main function to run the whole program.
	void sendPrintTest(const std::string& text) const;
};