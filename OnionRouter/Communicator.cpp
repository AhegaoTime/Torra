#include "Communicator.h"


Communicator::Communicator(const std::string& ip, const uint16_t port) {
	//Create socket.
	this->m_serverSocket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (this->m_serverSocket == INVALID_SOCKET) {
		throw std::exception(__FUNCTION__ " - socket");
	}

	this->bindAndListen();

	Cryptor::createRsaSignaturePrivateKey();
	Communicator::ConnectToServer(this->m_supernodeSocket, ip, port);
	this->joinNetwork();
}

Communicator::~Communicator(void) {}

//Connect to the server
void Communicator::ConnectToServer(SOCKET& socket, const std::string& ip, const uint16_t port) {
	socket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (socket == INVALID_SOCKET) {
		throw std::exception(__FUNCTION__ " - socket creation");
	}

	sockaddr_in serv_addr{};
	serv_addr.sin_port = htons(port);

	serv_addr.sin_family = AF_INET;
	inet_pton(AF_INET, ip.c_str(), &(serv_addr.sin_addr));
	

	if (connect(socket, (sockaddr*)&serv_addr, sizeof(serv_addr)) != 0) {
		throw std::exception(__FUNCTION__ " - socket connection");
	}
}

//Join as a router.
void Communicator::joinNetwork(void) {
	uint8_t requestMessage[5] = { 92, 0, 0, 0, 0 };
	Helper::sendRawData(this->m_supernodeSocket, requestMessage, 5);

	if (this->checkStatus()) {
		const std::string joinMessageJson = Cryptor::signaturePublicKeyToHex();

		std::vector<uint8_t> joinMessage = { 21 };
		uint32_t dataSize = joinMessageJson.size() + 2;
		uint8_t dataSizeArray[UINT8_PER_UINT32] = { 0 };
		uint32_uint8_converter::uint32_to_uint8(dataSize, dataSizeArray);

		joinMessage.insert(joinMessage.begin() + 1, std::begin(dataSizeArray), std::end(dataSizeArray));
		std::copy(joinMessageJson.begin(), joinMessageJson.end(), std::back_inserter(joinMessage));
		
		joinMessage.push_back((this->m_listeningPort >> 8) & 0xFF);
		joinMessage.push_back(this->m_listeningPort & 0xFF);

		Helper::sendRawData(this->m_supernodeSocket, joinMessage.data(), joinMessage.size());

		this->checkStatus();
	}
}

//Checks the status of the message from the supernode.
const bool Communicator::checkStatus(void) {
	uint8_t responseCode = 0;
	Helper::getRawPartFromSocket(this->m_supernodeSocket, &responseCode, 1);

	//Clear the socket. It sends more than just the response code.
	uint32_t size = Helper::getDataLength(this->m_supernodeSocket);

	return responseCode == 200;
}

void Communicator::bindAndListen(void) {
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
	this->m_listeningPort = ntohs(sa.sin_port);
}

void Communicator::startHandleRequests(void) {
	//Accepting new clients in a loop.
	while (true) {
		std::cout << "accepting client..." << std::endl;
		this->acceptClient();
	}
}

//Accepts new client and creates a new thread to handle the communication with that client.
void Communicator::acceptClient(void) {
	SOCKET clientSocket = accept(this->m_serverSocket, NULL, NULL);
	if (clientSocket == INVALID_SOCKET)
		throw std::exception(__FUNCTION__);

	std::cout << "Client accepted !" << std::endl;
	// create new thread for client	and detach from it
	std::thread tr(&Communicator::handleNewClient, clientSocket);
	tr.detach();
}

void Communicator::handleNewClient(const SOCKET& clientSocket) {
	Cryptor cryptor; //Each client has its own cryptor object.

	//Exchange RSA signature keys (only for the demo).
	//Communicator::exchangeRsaSignatureKeys(clientSocket, cryptor);

	//Perform a handshake with Diffie-Hellman key exchange.
	RouterData routerData = dhHandshake(clientSocket, cryptor);

	sockaddr_in sa{};
	char ipStrArray[INET_ADDRSTRLEN] = { '\0' };
	sa.sin_addr.s_addr = routerData.nextIp;
	inet_ntop(AF_INET, &(sa.sin_addr), ipStrArray, INET_ADDRSTRLEN);
	std::string ipString(ipStrArray);

	SOCKET nextRouterSocket;
	Communicator::ConnectToServer(nextRouterSocket, ipString, routerData.nextPort);

	Communicator::passHandshake(clientSocket, nextRouterSocket, cryptor, routerData.routerIndex);

	const CryptoPP::SecByteBlock messageToForwardFromClient = Communicator::readSecByteBlock(clientSocket);
	const CryptoPP::SecByteBlock messageForwardingToNextRouter = Communicator::deconstructLayer(cryptor, messageToForwardFromClient);
	CryptoPP::SecByteBlock messageToForwardFromNextRouter;

	//The Server doesn't want to get the SecByteBlock's size.
	if (routerData.routerIndex == 2) {
		Helper::sendRawData(nextRouterSocket, messageForwardingToNextRouter, messageForwardingToNextRouter.size());

		uint8_t headers[5] = { 0 };
		Helper::getRawPartFromSocket(nextRouterSocket, headers, 5);
		uint32_t size = uint32_uint8_converter::uint8_to_uint32(headers + 1);
		uint8_t* data = new uint8_t[size];
		Helper::getRawPartFromSocket(nextRouterSocket, data, size);
		std::vector<uint8_t> fullMessageFromServer;
		std::copy(std::begin(headers), std::end(headers), std::back_inserter(fullMessageFromServer));
		std::copy(data, data + size, std::back_inserter(fullMessageFromServer));

		delete[] data;

		messageToForwardFromNextRouter.Assign(fullMessageFromServer.data(), fullMessageFromServer.size());
	}
	else {
		Communicator::sendSecByteBlock(nextRouterSocket, messageForwardingToNextRouter);
		messageToForwardFromNextRouter = Communicator::readSecByteBlock(clientSocket);
	}

	const CryptoPP::SecByteBlock messageForwardingToClient = Communicator::addEncryptionLayer(cryptor, messageToForwardFromNextRouter);

	Communicator::sendSecByteBlock(clientSocket, messageForwardingToClient);
}

//Sends the Diffie-Hellman public key.
void Communicator::createAndSendDhPublicKey(const SOCKET& clientSocket, Cryptor& cryptor) {
	//Generates the Diffie-Hellman keys.
	cryptor.createDhKeys();

	//Signs the message.
	const CryptoPP::SecByteBlock& dhPublicKey = cryptor.getDhPublicKey();
	const CryptoPP::SecByteBlock signature = Cryptor::signMessageWithRsa(dhPublicKey);

	//Create a message containing all the data.
	uint8_t keySizeArray[4];
	uint32_uint8_converter::uint32_to_uint8(dhPublicKey.size(), keySizeArray);
	CryptoPP::SecByteBlock keySizeBlock(keySizeArray, 4);

	uint8_t signatureSizeArray[4];
	uint32_uint8_converter::uint32_to_uint8(signature.size(), signatureSizeArray);
	CryptoPP::SecByteBlock signatureSizeBlock(signatureSizeArray, 4);

	CryptoPP::SecByteBlock messageToSend;
	messageToSend.Append(keySizeBlock);
	messageToSend.Append(signatureSizeBlock);
	messageToSend.Append(dhPublicKey);
	messageToSend.Append(signature);

	Communicator::sendSecByteBlock(clientSocket, messageToSend);
}

//Reads the Diffie-Hellman public key of the second user and create a Diffie-Hellman shared key.
const RouterData Communicator::readOtherDhPublicKeyAndAgree(const SOCKET& clientSocket, Cryptor& cryptor) {
	//Reads the message.
	const CryptoPP::SecByteBlock handshakeMessage = Communicator::readSecByteBlock(clientSocket);

	uint32_t offset = 0;
	const uint8_t sizesSize = 4;
	const CryptoPP::SecByteBlock keySizeBlock(handshakeMessage, sizesSize);
	offset += sizesSize;
	//const CryptoPP::SecByteBlock signatureSizeBlock(handshakeMessage.data() + offset, sizesSize);
	//offset += sizesSize;
	const uint32_t keySize = uint32_uint8_converter::uint8_to_uint32(keySizeBlock);
	//const uint32_t signatureSize = uint32_uint8_converter::uint8_to_uint32(signatureSizeBlock);

	const CryptoPP::SecByteBlock dhPublicKeyOther(handshakeMessage.data() + offset, keySize);
	offset += keySize;
	/*const CryptoPP::SecByteBlock signature(handshakeMessage.data() + offset, signatureSize);
	offset += signatureSize;


	//Verify the signature.
	if (!cryptor.verifyMessageWithRsa(dhPublicKeyOther, signature)) {
		throw std::exception("RSA signature not matching.");
	}*/

	//Create shared key.
	cryptor.createDhSharedKey(dhPublicKeyOther);

	RouterData routerData;
	routerData.routerIndex = handshakeMessage[offset];
	offset++;
	routerData.nextIp = uint32_uint8_converter::uint8_to_uint32(handshakeMessage.data() + offset);
	offset += 4;
	routerData.nextPort = (handshakeMessage[offset] << 8) + handshakeMessage[offset + 1];

	return routerData;
}

//Passes the handshake to the next routers.
void Communicator::passHandshake(const SOCKET& clientSocket, const SOCKET& nextRouterSocket, Cryptor& cryptor, const uint8_t routerIndex) {
	for (uint8_t counter = 2; counter > routerIndex; counter--) {
		const CryptoPP::SecByteBlock onionPacketFromClient = Communicator::readSecByteBlock(clientSocket);
		const CryptoPP::SecByteBlock handshakeToPassToNextRouter(Communicator::deconstructLayer(cryptor, onionPacketFromClient));
		Communicator::sendSecByteBlock(nextRouterSocket, handshakeToPassToNextRouter);

		const CryptoPP::SecByteBlock onionPacketFromNextRouter = Communicator::readSecByteBlock(nextRouterSocket);
		const CryptoPP::SecByteBlock handshakeToPassToClient(Communicator::addEncryptionLayer(cryptor, onionPacketFromNextRouter));
		Communicator::sendSecByteBlock(clientSocket, handshakeToPassToClient);
	}
}

//Performs a handshake with Diffie-Hellman encryption to get an AES key.
const RouterData Communicator::dhHandshake(const SOCKET& clientSocket, Cryptor& cryptor) {
	Communicator::createAndSendDhPublicKey(clientSocket, cryptor);
	return Communicator::readOtherDhPublicKeyAndAgree(clientSocket, cryptor);
}

//Performs a full handshake.
const SOCKET Communicator::fullHandshake(const SOCKET& clientSocket, Cryptor& cryptor) {
	RouterData routerData = dhHandshake(clientSocket, cryptor);

	sockaddr_in sa{};
	char ipStrArray[INET_ADDRSTRLEN] = {'\0'};
	sa.sin_addr.s_addr = routerData.nextIp;
	inet_ntop(AF_INET, &(sa.sin_addr), ipStrArray, INET_ADDRSTRLEN);
	std::string ipString(ipStrArray);

	SOCKET nextRouterSocket;
	Communicator::ConnectToServer(nextRouterSocket, ipString, routerData.nextPort);

	Communicator::passHandshake(clientSocket, nextRouterSocket, cryptor, routerData.routerIndex);

	return nextRouterSocket;
}

const CryptoPP::SecByteBlock Communicator::deconstructLayer(Cryptor& cryptor, const CryptoPP::SecByteBlock& onionPacket) {
	const uint8_t ivSize = CryptoPP::AES::BLOCKSIZE;
	const CryptoPP::SecByteBlock iv(onionPacket, ivSize);
	const CryptoPP::SecByteBlock encryptedMessage(onionPacket.data() + ivSize, onionPacket.size() - ivSize);

	//Set the AES IV.
	cryptor.setAesIv(iv);

	const CryptoPP::SecByteBlock decryptedMessage = cryptor.decryptAesMessage(encryptedMessage);

	return decryptedMessage;
}

const CryptoPP::SecByteBlock Communicator::addEncryptionLayer(Cryptor& cryptor, const CryptoPP::SecByteBlock& onionPacket) {
	cryptor.createAesIv();
	CryptoPP::SecByteBlock moreEncryptedMessage(cryptor.getAesIv());
	moreEncryptedMessage.Append(cryptor.encryptAesMessage(onionPacket));

	return moreEncryptedMessage;
}

//Sends an integer and its size with the socket.
void Communicator::sendInteger(const SOCKET& clientSocket, const Integer& integer) {
	uint32_t integerSize = integer.ByteCount();

	//Convert Integer object to byte array.
	uint8_t* integerArray = new uint8_t[integerSize];
	integer.Encode(integerArray, integerSize);

	//Convert message's size to array.
	uint8_t integerSizeArray[4] = { 0 };
	uint32_uint8_converter::uint32_to_uint8(integerSize, integerSizeArray);

	//Send the size.
	Helper::sendRawData(clientSocket, integerSizeArray, UINT8_PER_UINT32);

	//Send the encrypted message array.
	Helper::sendRawData(clientSocket, integerArray, integerSize);

	delete[] integerArray;
}

//Sends a SecByteBlock and its size with the socket.
void Communicator::sendSecByteBlock(const SOCKET& clientSocket, const CryptoPP::SecByteBlock& secByteBlock) {
	Communicator::sendByteArray(clientSocket, secByteBlock, secByteBlock.size());
}

//Sends a byte array and its size with the socket.
void Communicator::sendByteArray(const SOCKET& clientSocket, const byte* array, const uint32_t size) {
	//Convert size to array.
	uint8_t sizeArray[4] = { 0 };
	uint32_uint8_converter::uint32_to_uint8(size, sizeArray);

	//Send the size.
	Helper::sendRawData(clientSocket, sizeArray, UINT8_PER_UINT32);

	//Send the array.
	Helper::sendRawData(clientSocket, array, size);
}

//Reads a byte array from the socket and outputs as a byte vector.
const std::vector<byte> Communicator::readByteVector(const SOCKET& clientSocket) {
	uint32_t size = Helper::getDataLength(clientSocket); //Read size from socket.
	byte* byteArray = new byte[size];
	Helper::getRawPartFromSocket(clientSocket, byteArray, size); //Read value from socket.

	const std::vector<byte> byteVector(byteArray, byteArray + size);

	delete[] byteArray;

	return byteVector;
}

//Reads a byte array from the socket and outputs as a SecByteBlock.
const CryptoPP::SecByteBlock Communicator::readSecByteBlock(const SOCKET& clientSocket) {
	uint32_t size = Helper::getDataLength(clientSocket); //Read size from socket.
	byte* byteArray = new byte[size];
	Helper::getRawPartFromSocket(clientSocket, byteArray, size); //Read value from socket.

	CryptoPP::SecByteBlock byteBlock(byteArray, size);

	delete[] byteArray;

	return byteBlock;
}

//Reads an Integer from the socket.
const Integer Communicator::readInteger(const SOCKET& clientSocket) {
	uint32_t integerSize = Helper::getDataLength(clientSocket); //Read size from socket.
	uint8_t* integerArray = new uint8_t[integerSize];
	Helper::getRawPartFromSocket(clientSocket, integerArray, integerSize); //Read value from socket.

	Integer integer(integerArray, integerSize); //Creaye intger object.

	delete[] integerArray;
	return integer;
}