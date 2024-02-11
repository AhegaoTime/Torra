#include "Communicator.h"

//Constructor.
Communicator::Communicator(void) : m_routers(ROUTERS_AMOUNT) {
	/*this->m_onionSocket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (this->m_onionSocket == INVALID_SOCKET) {
		throw std::exception(__FUNCTION__ " - onionSocket");
	}

	this->m_supernodeSocket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (this->m_supernodeSocket == INVALID_SOCKET) {
		throw std::exception(__FUNCTION__ " - supernodeSocket");
	}*/
}

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

//Requests the 3 routers path from the supernode.
void Communicator::requestPathFromSupernode(void) {
	uint8_t requestMessage[5] = { 92, 0, 0, 0, 0 };
	Helper::sendRawData(this->m_supernodeSocket, requestMessage, 5);
	if (this->checkStatus()) {
		requestMessage[0] = 23;
		Helper::sendRawData(this->m_supernodeSocket, requestMessage, 5);

		this->parsePathFromSupernode();
	}
}

//Checks the status of the message from the supernode.
const bool Communicator::checkStatus(void) {
	uint8_t responseCode = 0;
	Helper::getRawPartFromSocket(this->m_supernodeSocket, &responseCode, 1);

	//Clear the socket. It sends more than just the response code.
	uint32_t size = Helper::getDataLength(this->m_supernodeSocket);
	Helper::getStringPartFromSocket(this->m_supernodeSocket, size);

	return responseCode == 200;
}

//Reads a path from the supernode and parses it.
void Communicator::parsePathFromSupernode(void) {
	uint8_t responseCode = 0;
	Helper::getRawPartFromSocket(this->m_supernodeSocket, &responseCode, 1);
	uint32_t size = Helper::getDataLength(this->m_supernodeSocket);
	std::string jsonString = Helper::getStringPartFromSocket(this->m_supernodeSocket, size);

	Json json;
	json = Json::parse(jsonString);

	std::vector<std::string> ips;
	std::vector<uint16_t> ports;
	std::vector<std::string> keys;

	json["ProxiesIp"].get_to(ips);
	json["ProxiesPort"].get_to(ports);
	json["ProxiesKey"].get_to(keys);

	sockaddr_in sa{};

	this->m_entryGuardIp = ips[0];
	this->m_entryGuardPort = ports[0];

	for (uint8_t index = 0; index < this->m_routers.size(); index++) {
		inet_pton(AF_INET, ips[index].c_str(), &(sa.sin_addr));
		this->m_routers[index].ip = sa.sin_addr.s_addr;
		this->m_routers[index].port = ports[index];
		this->m_routers[index].cryptor.hexToSignaturePublicKey(keys[index]);
	}
}

//Performs a handshake with Diffie-Hellman to send an AES key.
void Communicator::dhHandshake(const uint8_t routerIndex) {
	this->createAndSendDhPublicKey(routerIndex);
	this->readOtherDhPublicKeyAndAgree(routerIndex);
}

//Performs a handshake with Diffie-Hellman to send an AES key with all of the routers.
void Communicator::dhHandshakeFull(void) {
	for (uint8_t index = 0; index < ROUTERS_AMOUNT; index++) {
		this->dhHandshake(index);
	}
}


//Sends the Diffie-Hellman public key.
void Communicator::createAndSendDhPublicKey(const uint8_t routerIndex) {
	//Generates the Diffie-Hellman keys.
	this->m_routers[routerIndex].cryptor.createDhKeys();
	//Cryptor::createRsaSignaturePrivateKey();

	//Signs the message.
	const CryptoPP::SecByteBlock& dhPublicKey = this->m_routers[routerIndex].cryptor.getDhPublicKey();
	//const CryptoPP::SecByteBlock signature = Cryptor::signMessageWithRsa(dhPublicKey);

	//Create a message containing all the data.
	uint8_t keySizeArray[4];
	uint32_uint8_converter::uint32_to_uint8(dhPublicKey.size(), keySizeArray);
	CryptoPP::SecByteBlock keySizeBlock(keySizeArray, 4);

	/*uint8_t signatureSizeArray[4];
	uint32_uint8_converter::uint32_to_uint8(signature.size(), signatureSizeArray);
	CryptoPP::SecByteBlock signatureSizeBlock(signatureSizeArray, 4);*/

	uint8_t ipArray[4];
	uint32_uint8_converter::uint32_to_uint8(this->m_routers[routerIndex + 1].ip, ipArray);
	CryptoPP::SecByteBlock ipBlock(ipArray, 4);

	const uint8_t nextPortLow = this->m_routers[routerIndex + 1].port & 0xFF;
	const uint8_t nextPortHigh = (this->m_routers[routerIndex + 1].port >> 8) & 0xFF;

	std::cout << ((int)nextPortHigh << 8) + (int)nextPortLow << std::endl;

	CryptoPP::SecByteBlock messageToEncrypt;
	messageToEncrypt.Append(keySizeBlock);
	//messageToEncrypt.Append(signatureSizeBlock);
	messageToEncrypt.Append(dhPublicKey);
	//messageToEncrypt.Append(signature);
	messageToEncrypt.Append(CryptoPP::SecByteBlock(&routerIndex, 1));
	messageToEncrypt.Append(ipBlock);
	messageToEncrypt.Append(CryptoPP::SecByteBlock(&nextPortHigh, 1));
	messageToEncrypt.Append(CryptoPP::SecByteBlock(&nextPortLow, 1));

	//Encrypt with the routers we already handshaked with.
	const CryptoPP::SecByteBlock messageToSend(this->constructOnionPacket(messageToEncrypt, routerIndex));

	//Sends the encryted message.
	this->sendSecByteBlock(messageToSend);
}

//Reads the Diffie-Hellman public key of the second user and create a Diffie-Hellman shared key.
void Communicator::readOtherDhPublicKeyAndAgree(const uint8_t routerIndex) {
	const CryptoPP::SecByteBlock onionPacket = this->readSecByteBlock();

	//Decrypt with the routers we already handshaked with.
	const CryptoPP::SecByteBlock decryptedMessage = this->deconstructOnionPacket(onionPacket, routerIndex);

	const uint8_t sizesSize = 4;
	const CryptoPP::SecByteBlock keySizeBlock(decryptedMessage, sizesSize);
	const CryptoPP::SecByteBlock signatureSizeBlock(decryptedMessage.data() + sizesSize, sizesSize);
	const uint32_t keySize = uint32_uint8_converter::uint8_to_uint32(keySizeBlock);
	const uint32_t signatureSize = uint32_uint8_converter::uint8_to_uint32(signatureSizeBlock);

	const CryptoPP::SecByteBlock dhPublicKeyOther(decryptedMessage.data() + sizesSize * 2, keySize);
	const CryptoPP::SecByteBlock signature(decryptedMessage.data() + sizesSize * 2 + keySize, signatureSize);
	

	//Verify the signature.
	if (!this->m_routers[routerIndex].cryptor.verifyMessageWithRsa(dhPublicKeyOther, signature)) {
		throw std::exception("RSA signature not matching.");
	}

	//Create shared key.
	this->m_routers[routerIndex].cryptor.createDhSharedKey(dhPublicKeyOther);
}

//Adds the target server to the routers list, not really a router though.
void Communicator::addTargetServerData(const std::string& ip, const uint16_t port) {
	sockaddr_in sa{};
	inet_pton(AF_INET, ip.c_str(), &(sa.sin_addr));

	Router targetServer;
	targetServer.ip = sa.sin_addr.s_addr;
	targetServer.port = port;

	this->m_routers.push_back(targetServer);
}

const CryptoPP::SecByteBlock Communicator::constructOnionPacket(const CryptoPP::SecByteBlock& data, uint8_t routerIndex) const{
	CryptoPP::SecByteBlock encryptedMessage;
	CryptoPP::SecByteBlock onionPacket = data;

	for (; routerIndex; routerIndex--) {
		this->m_routers[routerIndex - 1].cryptor.createAesIv();
		encryptedMessage = this->m_routers[routerIndex - 1].cryptor.encryptAesMessage(onionPacket);
		//We want to add the IV before the data.
		onionPacket.Assign(this->m_routers[routerIndex - 1].cryptor.getAesIv());
		onionPacket.Append(encryptedMessage);
	}

	return onionPacket;
}

const CryptoPP::SecByteBlock Communicator::deconstructOnionPacket(const CryptoPP::SecByteBlock& onionPacket, const uint8_t routerIndex) const {
	CryptoPP::SecByteBlock messageToDecrypt;
	CryptoPP::SecByteBlock decryptedMessage = onionPacket;
	const uint8_t ivSize = CryptoPP::AES::BLOCKSIZE;

	for (uint8_t index = 0; index < routerIndex; index++) {
		CryptoPP::SecByteBlock iv(decryptedMessage.data(), ivSize);
		this->m_routers[index].cryptor.setAesIv(iv);
		messageToDecrypt.Assign(decryptedMessage.data() + ivSize, decryptedMessage.size() - ivSize);
		decryptedMessage = this->m_routers[index].cryptor.decryptAesMessage(messageToDecrypt);
	}

	return decryptedMessage;
}

//Sends an integer and its size with the socket.
void Communicator::sendInteger(const Integer& integer) const {
	uint32_t integerSize = integer.ByteCount();

	//Convert Integer object to byte array.
	uint8_t* integerArray = new uint8_t[integerSize];
	integer.Encode(integerArray, integerSize);

	//Convert encrypted message size to array.
	uint8_t integerSizeArray[4] = { 0 };
	uint32_uint8_converter::uint32_to_uint8(integerSize, integerSizeArray);

	//Send the size.
	Helper::sendRawData(this->m_onionSocket, integerSizeArray, UINT8_PER_UINT32);

	//Send the encrypted message array.
	Helper::sendRawData(this->m_onionSocket, integerArray, integerSize);

	delete[] integerArray;
}

//Sends a string and its size with the socket.
void Communicator::sendString(const std::string& string) const {
	uint32_t stringSize = string.size();
	//Convert string size to array.
	uint8_t stringSizeArray[4] = { 0 };
	uint32_uint8_converter::uint32_to_uint8(stringSize, stringSizeArray);

	//Send the size.
	Helper::sendRawData(this->m_onionSocket, stringSizeArray, UINT8_PER_UINT32);

	//Send the message.
	Helper::sendData(this->m_onionSocket, string);
}

//Sends a byte array and its size with the socket.
void Communicator::sendByteArray(const byte* array, const uint32_t size) const {
	//Convert size to array.
	uint8_t sizeArray[4] = { 0 };
	uint32_uint8_converter::uint32_to_uint8(size, sizeArray);

	//Send the size.
	Helper::sendRawData(this->m_onionSocket, sizeArray, UINT8_PER_UINT32);

	//Send the array.
	Helper::sendRawData(this->m_onionSocket, array, size);
}

//Sends a SecByteBlock and its size with the socket.
void Communicator::sendSecByteBlock(const CryptoPP::SecByteBlock& secByteBlock) const {
	this->sendByteArray(secByteBlock, secByteBlock.size());
}

//Reads an Integer from the socket.
const Integer Communicator::readInteger(void) const {
	uint32_t integerSize = Helper::getDataLength(this->m_onionSocket); //Read size from socket.
	uint8_t* integerArray = new uint8_t[integerSize];
	Helper::getRawPartFromSocket(this->m_onionSocket, integerArray, integerSize); //Read value from socket.

	Integer integer(integerArray, integerSize); //Creaye intger object.

	delete[] integerArray;
	return integer;
}

//Reads a SecByteBlock from the socket.
const CryptoPP::SecByteBlock Communicator::readSecByteBlock(uint32_t size) const {
	if (!size) {
		size = Helper::getDataLength(this->m_onionSocket); //Read size from socket.
	}
	uint8_t* array = new uint8_t[size];
	Helper::getRawPartFromSocket(this->m_onionSocket, array, size); //Read value from socket.

	CryptoPP::SecByteBlock secByteBlock(array, size); //Put in a SecByteBlock.

	return secByteBlock;
}

//The main function to run the whole program.
void Communicator::run(const std::string& ip, const uint16_t port) {
	Communicator::ConnectToServer(this->m_supernodeSocket, ip, port); //Connects to the supernode.
	this->requestPathFromSupernode(); //Retrievs the path from the supernode.
	this->addTargetServerData(ip, port);
	Communicator::ConnectToServer(this->m_onionSocket, this->m_entryGuardIp, this->m_entryGuardPort); //Connects to the supernode.
	this->dhHandshakeFull(); //Handshakes with all of the onion routers in the path.
	this->sendPrintTest("Is it working?");

	while (true);
}

void Communicator::sendPrintTest(const std::string& text) const {
	std::vector<uint8_t> testMessage = { 93 };

	uint32_t size = text.size();
	uint8_t sizeArray[4] = { 0 };
	uint32_uint8_converter::uint32_to_uint8(size, sizeArray);

	std::copy(std::begin(sizeArray), std::end(sizeArray), std::back_inserter(testMessage));
	std::copy(text.begin(), text.end(), std::back_inserter(testMessage));

	const CryptoPP::SecByteBlock testMessageBlock(testMessage.data(), testMessage.size());

	const CryptoPP::SecByteBlock testMessageEncrypted = this->constructOnionPacket(testMessageBlock);

	this->sendSecByteBlock(testMessageEncrypted);
}