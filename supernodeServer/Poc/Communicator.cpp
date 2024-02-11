#include "Communicator.h"
#include "RequestHandlerFactory.h"

std::mutex clientMutex;
std::mutex managerMutex;

Communicator::Communicator(RequestHandlerFactory& handlerFactory) : m_handlerFactory(handlerFactory){
	//Create socket.
	this->m_serverSocket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (this->m_serverSocket == INVALID_SOCKET)
		throw std::exception(__FUNCTION__ " - socket");
}

Communicator::~Communicator(void) {
	//Delete all requestHandlers (dynamically allocated).
	for (std::pair<const SOCKET*, IRequestHandler*> client : this->m_clients) {
		delete client.second;
	}
}

void Communicator::bindAndListen(void) const{
	struct sockaddr_in sa = { 0 };
	sa.sin_port = htons(RANDOM_PORT);
	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = IFACE;
	// again stepping out to the global namespace
	if (::bind(this->m_serverSocket, (struct sockaddr*)&sa, sizeof(sa)) == SOCKET_ERROR)
		throw std::exception(__FUNCTION__ " - bind");
	TRACE("binded");

	int slen = sizeof(sa);

	if (getsockname(this->m_serverSocket, (struct sockaddr*)&sa, &slen) < 0) {
		std::cerr << "failed to get hostname with errno: " << errno << std::endl;
		exit(1);
	}

	if (::listen(this->m_serverSocket, SOMAXCONN) == SOCKET_ERROR) {
		throw std::exception(__FUNCTION__ " - listen");
	}

	std::cout << "listening on port - " << ntohs(sa.sin_port) << std::endl;
}

void Communicator::startHandleRequests(void) {
	this->bindAndListen();

	//Accepting new clients in a loop.
	while (true){
		TRACE("accepting client...");
		this->acceptClient();
	}
}

void Communicator::handleNewClient(const SOCKET& clientSocket, const std::string& ip) {
	IRequestHandler* currentHandler = this->m_handlerFactory.createServiceRequestHandler(); //Need to free it.

	//Loop which handles a new request each iteration.
	while (true) {
		try {
			RequestInfo requestInfo = Communicator::getRequest(clientSocket); //Receiving request from the client.
			std::pair<const SOCKET*, IRequestHandler*> client(&clientSocket, currentHandler);
			std::unique_lock<std::mutex> locker(clientMutex); //Many threads may work in the same time on this same line so we need a mutex here.
			this->m_clients.emplace(client);
			locker.unlock();


			//Make sure it's a relevant request.
			if (currentHandler->isRequestRelevant(requestInfo)) {
				//I decided to add just one mutex here and lock the whole handleRequest function.
				//I did it this way so I won't miss any place needed and won't need to add about 100 mutexes every sigle place we are using one or more of the managers (the managers change their fields most of the times).
				std::unique_lock<std::mutex> locker(managerMutex);
				if (currentHandler->isNetworkRequestHandler() && requestInfo.requestCode != RequestCodes::RequestCodes::GetRoute) {
					//The IP and Port of the client must be figured by the server.
					const std::string publicKeyString(requestInfo.buffer.begin(), requestInfo.buffer.end() - 2);
					const uint16_t port = (requestInfo.buffer[requestInfo.buffer.size() - 2] * 0x100) + requestInfo.buffer[requestInfo.buffer.size() - 1];
					Json json;
					json[PUBLICKEY] = publicKeyString;
					json[IP] = ip;
					json[PORT] = port;

					std::string jsonData = json.dump();
					requestInfo.buffer.clear();
					std::copy(jsonData.begin(), jsonData.end(), std::back_inserter(requestInfo.buffer));
				}
				RequestResult requestResult = currentHandler->handleRequest(requestInfo); //Handle the request.
				locker.unlock();
				Communicator::sendResponse(clientSocket, requestResult.response);

				//Move to the new handler if it's a different handler.
				if (requestResult.newHandler != currentHandler) {
					delete currentHandler;
					currentHandler = requestResult.newHandler;
				}
			}
			else {
				ErrorResponse errorResponse;
				errorResponse.message = "Unrelevant request.";
				std::vector<uint8_t> errorMessageVector = JsonResponsePacketSerializer::serializeResponse(errorResponse);
				Communicator::sendResponse(clientSocket, errorMessageVector);

				//No neeed to move to a new handler because the state is left unchanged.
			}
		}
		catch (...) {
			closesocket(clientSocket);
		}
	}
}

//Accepts new client and creates a new thread to handle the communication with that client.
void Communicator::acceptClient(void) {
	struct sockaddr_in client_addr;
	int slen = sizeof(client_addr);
	SOCKET clientSocket = accept(this->m_serverSocket, (struct sockaddr*)&client_addr, &slen);
	if (clientSocket == INVALID_SOCKET)
		throw std::exception(__FUNCTION__);

	TRACE("Client accepted !");

	char ipStrArray[INET_ADDRSTRLEN] = { '\0' };
	inet_ntop(AF_INET, &(client_addr.sin_addr), ipStrArray, INET_ADDRSTRLEN);
	// create new thread for client	and detach from it
	const std::string ipString = ipStrArray;
	std::thread tr(&Communicator::handleNewClient, this, clientSocket, ipString);
	tr.detach();
}

//Gets request from the socket and returns it as a RequestInfo object.
RequestInfo Communicator::getRequest(const SOCKET& clientSocket) {
	RequestInfo requestInfo;
	requestInfo.requestCode = Helper::getMessageTypeCode(clientSocket);
	requestInfo.receivalTime = time(NULL);
	uint32_t dataLength = Helper::getDataLength(clientSocket);
	
	if (dataLength) {
		uint8_t* bufferArray = new uint8_t[dataLength];
		Helper::getRawPartFromSocket(clientSocket, bufferArray, dataLength);
		requestInfo.buffer.assign(bufferArray, bufferArray + dataLength);
	}

	return requestInfo;
}

void Communicator::sendResponse(const SOCKET& sc, const std::vector<uint8_t>& buffer) {
	std::string message(buffer.begin(), buffer.end());
	Helper::sendData(sc, message);
}