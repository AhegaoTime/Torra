#pragma once
#include <map>
#include <thread>
#include <iostream>
#include <mutex>
#include <ws2tcpip.h>
#include "ServiceRequestHandler.h"
#include "Helper.h"
#include "JsonResponsePacketSerializer.h"

constexpr uint16_t RANDOM_PORT = 8826;
constexpr uint8_t IFACE = 0;

class RequestHandlerFactory;

class Communicator {
public:
	Communicator(RequestHandlerFactory& handlerFactory);
	~Communicator(void);

	void startHandleRequests(void);
private:
	std::map<const SOCKET*, IRequestHandler*> m_clients; //Made the SOCKET pointer so we won't pass the socket by value.
	RequestHandlerFactory& m_handlerFactory;
	SOCKET m_serverSocket;

	void bindAndListen(void) const;
	void handleNewClient(const SOCKET& clientSocket, const std::string& ip);
	void acceptClient(void);

	static RequestInfo getRequest(const SOCKET& clientSocket); //Gets request from the socket and returns it as a RequestInfo object.
	static void sendResponse(const SOCKET& sc, const std::vector<uint8_t>& buffer); //Sends the response buffer to the client.
};