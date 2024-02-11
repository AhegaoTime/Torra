#pragma once
#include <string>
#include <cstdint>
#include <vector>
#include "json.hpp"
#include "RequestStructs.h"
#include "Constants.h"

using Json = nlohmann::json;

class JsonRequestPacketDeserializer {
public:
	//Login:
	static JoinRequest desirializeJoinRequest(const std::vector<uint8_t>& buffer);
	static LeaveRequest desirializeLeaveRequest(const std::vector<uint8_t>& buffer);
	static LoginRequest desirializeLoginRequest(const std::vector<uint8_t>& buffer);
	static SignupRequest desirializeSignupRequest(const std::vector<uint8_t>& buffer);
	static CreateChatRequest desirializeCreateChatRequest(const std::vector<uint8_t>& buffer);
	static CloseChatRequest desirializeCloseChatRequest(const std::vector<uint8_t>& buffer);
	static SignoutRequest desirializeSignoutRequest(const std::vector<uint8_t>& buffer);
	static MsgChatRequest desirializeSendMsgRequest(const std::vector<uint8_t>& buffer);


};