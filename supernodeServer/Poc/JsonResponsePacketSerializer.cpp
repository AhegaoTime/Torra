#include "JsonResponsePacketSerializer.h"

std::vector<uint8_t> JsonResponsePacketSerializer::serializeResponse(const ErrorResponse& response){
	Json json = { { MESSAGE, response.message } };
	
	return JsonResponsePacketSerializer::serializeJson(json, ResponseCodes::ERR);
}

std::vector<uint8_t> JsonResponsePacketSerializer::serializeResponse(const LoginResponse& response)
{
	Json json = { { STATUS, response.status } };

	return JsonResponsePacketSerializer::serializeJson(json, ResponseCodes::OK);
}

std::vector<uint8_t> JsonResponsePacketSerializer::serializeResponse(const SignupResponse& response)
{
	Json json = { { STATUS, response.status } };

	return JsonResponsePacketSerializer::serializeJson(json, ResponseCodes::OK);
}

std::vector<uint8_t> JsonResponsePacketSerializer::serializeResponse(const JoinResponse& response){
	Json json = { { STATUS, response.status } };
	
	return JsonResponsePacketSerializer::serializeJson(json, ResponseCodes::OK);	
}

std::vector<uint8_t> JsonResponsePacketSerializer::serializeResponse(const LeaveResponse& response){
	Json json = { { STATUS, response.status } };

	return JsonResponsePacketSerializer::serializeJson(json, ResponseCodes::OK);
}

std::vector<uint8_t> JsonResponsePacketSerializer::serializeResponse(const RouteResponse& response) {
	Json proxyIpVec(response.proxiesIp);
	Json proxyPortVec(response.proxiesPort);
	Json proxyKeyVec(response.proxiesKey);
	Json json = {
		{STATUS, response.status},
		{PROXIESIP,  proxyIpVec},
		{PROXIESPORT, proxyPortVec},
		{PROXIESKEY, proxyKeyVec}
	};

	return JsonResponsePacketSerializer::serializeJson(json, ResponseCodes::OK);
}

std::vector<uint8_t> JsonResponsePacketSerializer::serializeResponse(const CreateChatResponse& response)
{
	Json json = { { STATUS, response.status } };

	return JsonResponsePacketSerializer::serializeJson(json, ResponseCodes::OK);
}

std::vector<uint8_t> JsonResponsePacketSerializer::serializeResponse(const CloseChatResponse& response)
{
	Json json = { { STATUS, response.status } };

	return JsonResponsePacketSerializer::serializeJson(json, ResponseCodes::OK);
}

std::vector<uint8_t> JsonResponsePacketSerializer::serializeResponse(const SignoutResponse& response)
{
	Json json = { { STATUS, response.status } };

	return JsonResponsePacketSerializer::serializeJson(json, ResponseCodes::OK);
}

std::vector<uint8_t> JsonResponsePacketSerializer::serializeResponse(const MsgChatResponse& response)
{
	Json msg(response.msg);
	Json json = {
		{STATUS, response.status},
		{CLIENTMSG,  msg}
	};

	return JsonResponsePacketSerializer::serializeJson(json, ResponseCodes::OK);
}

std::vector<uint8_t> JsonResponsePacketSerializer::serializeResponse(const OnionClientResponse& response)
{
	Json json = { { STATUS, response.status } };

	return JsonResponsePacketSerializer::serializeJson(json, ResponseCodes::OK);
}

std::vector<uint8_t> JsonResponsePacketSerializer::serializeResponse(const GuiClientResponse& response)
{
	Json json = { { STATUS, response.status } };

	return JsonResponsePacketSerializer::serializeJson(json, ResponseCodes::OK);
}



std::vector<uint8_t> JsonResponsePacketSerializer::serializeJson(const Json& json, const ResponseCodes& responseCode) {
	std::string data = json.dump();
	uint32_t dataLength = data.length();
	uint8_t dataLengthChar[4] = { 0 };
	uint32_uint8_converter::uint32_to_uint8(dataLength, dataLengthChar);

	std::vector<uint8_t> message;
	message.push_back((uint8_t)responseCode);
	message.push_back(dataLengthChar[0]);
	message.push_back(dataLengthChar[1]);
	message.push_back(dataLengthChar[2]);
	message.push_back(dataLengthChar[3]);

	for (uint32_t counter = 0; counter < dataLength; counter++) {
		message.push_back(data[counter]);
	}

	return message;
}