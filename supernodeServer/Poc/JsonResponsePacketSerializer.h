#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include "json.hpp"
#include "ResponseCodes.h"
#include "uint32_uint8_converter.h"
#include "ResponseStructs.h"
#include "Constants.h"

using Json = nlohmann::json;

class JsonResponsePacketSerializer{
private:
	static std::vector<uint8_t> serializeJson(const Json& json, const ResponseCodes& responseCode); //Serialize Json to a message.
public:
	//Error:
	static std::vector<uint8_t> serializeResponse(const ErrorResponse& response);
	
	//Login:
	static std::vector<uint8_t> serializeResponse(const LoginResponse& response);
	static std::vector<uint8_t> serializeResponse(const SignupResponse& response);

	static std::vector<uint8_t> serializeResponse(const JoinResponse& response);
	static std::vector<uint8_t> serializeResponse(const LeaveResponse& response);
	static std::vector<uint8_t> serializeResponse(const RouteResponse& response);

	//Menu:
	static std::vector<uint8_t> serializeResponse(const CreateChatResponse& response);
	static std::vector<uint8_t> serializeResponse(const CloseChatResponse& response);
	static std::vector<uint8_t> serializeResponse(const SignoutResponse& response);

	//chat:
	static std::vector<uint8_t> serializeResponse(const MsgChatResponse& response);

	//service:
	static std::vector<uint8_t> serializeResponse(const OnionClientResponse& response);
	static std::vector<uint8_t> serializeResponse(const GuiClientResponse& response);
};