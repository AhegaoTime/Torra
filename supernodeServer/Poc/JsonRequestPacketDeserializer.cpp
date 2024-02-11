#include "JsonRequestPacketDeserializer.h"
#include <iostream>

JoinRequest JsonRequestPacketDeserializer::desirializeJoinRequest(const std::vector<uint8_t>& buffer) {
	Json json;
	JoinRequest joinRequest;

	//Create json table from the vector.
	try {
		json = Json::parse(buffer);
	}
	catch (std::exception& e) {
		std::cout << e.what();
	}

	//Insert the data into the struct.	
	joinRequest.ip = json[IP];
	joinRequest.port = json[PORT];
	joinRequest.publickey = json[PUBLICKEY];
	return joinRequest;
}

LeaveRequest JsonRequestPacketDeserializer::desirializeLeaveRequest(const std::vector<uint8_t>& buffer) {
	Json json;
	LeaveRequest leaveRequest;

	//Create json table from the vector.
	try {
		json = Json::parse(buffer);
	}
	catch (std::exception& e) {
		std::cout << e.what();
	}

	//Insert the data into the struct.
	leaveRequest.ip = json[IP];
	leaveRequest.port = json[PORT];
	leaveRequest.publickey = json[PUBLICKEY];

	return leaveRequest;
}

LoginRequest JsonRequestPacketDeserializer::desirializeLoginRequest(const std::vector<uint8_t>& buffer)
{
	Json json;
	LoginRequest loginRequest;

	//Create json table from the vector.
	try {
		json = Json::parse(buffer);
	}
	catch (std::exception& e) {
		std::cout << e.what();
	}

	//Insert the data into the struct.
	loginRequest.Username = json[USERNAME];
	loginRequest.Password = json[PASSWORD];

	return loginRequest;
}

SignupRequest JsonRequestPacketDeserializer::desirializeSignupRequest(const std::vector<uint8_t>& buffer)
{
	Json json;
	SignupRequest singupRequest;

	//Create json table from the vector.
	try {
		json = Json::parse(buffer);
	}
	catch (std::exception& e) {
		std::cout << e.what();
	}

	//Insert the data into the struct.
	singupRequest.Username = json[USERNAME];
	singupRequest.Password = json[PASSWORD];

	return singupRequest;
}

CreateChatRequest JsonRequestPacketDeserializer::desirializeCreateChatRequest(const std::vector<uint8_t>& buffer)
{
	Json json;
	CreateChatRequest createChatRequest;

	//Create json table from the vector.
	try {
		json = Json::parse(buffer);
	}
	catch (std::exception& e) {
		std::cout << e.what();
	}

	//Insert the data into the struct.
	createChatRequest.Username = json[USERNAME];

	return createChatRequest;
	
}

CloseChatRequest JsonRequestPacketDeserializer::desirializeCloseChatRequest(const std::vector<uint8_t>& buffer)
{
	Json json;
	CloseChatRequest closeChatRequest;

	//Create json table from the vector.
	try {
		json = Json::parse(buffer);
	}
	catch (std::exception& e) {
		std::cout << e.what();
	}

	//Insert the data into the struct.
	closeChatRequest.Username = json[USERNAME];

	return closeChatRequest;
}

SignoutRequest JsonRequestPacketDeserializer::desirializeSignoutRequest(const std::vector<uint8_t>& buffer)
{
	Json json;
	SignoutRequest singoutRequest;

	//Create json table from the vector.
	try {
		json = Json::parse(buffer);
	}
	catch (std::exception& e) {
		std::cout << e.what();
	}

	//Insert the data into the struct.
	singoutRequest.Username = json[USERNAME];
	singoutRequest.Password = json[PASSWORD];

	return singoutRequest;
}

MsgChatRequest JsonRequestPacketDeserializer::desirializeSendMsgRequest(const std::vector<uint8_t>& buffer)
{
	Json json;
	MsgChatRequest msgRequest;

	//Create json table from the vector.
	try {
		json = Json::parse(buffer);
	}
	catch (std::exception& e) {
		std::cout << e.what();
	}

	//Insert the data into the struct.
	msgRequest.Username = json[USERNAME];
	msgRequest.Msg = json[CLIENTMSG];

	return msgRequest;

}
