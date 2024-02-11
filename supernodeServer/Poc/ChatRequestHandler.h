#pragma once
#include <string>
#include "IRequestHandler.h"
#include "JsonRequestPacketDeserializer.h"
#include "JsonResponsePacketSerializer.h"
#include "ChatManager.h"
#include "RequestHandlerFactory.h"

class ChatRequestHandler : public IRequestHandler {
private:
	//Variables:
	ChatManager& m_chatManager;
	RequestHandlerFactory& m_requestHandlerFactory;

	//Functions:
	RequestResult sendMsg(const RequestInfo& requestInfo) const;
	RequestResult closeChat(const RequestInfo& requestInfo) const;
public:
	ChatRequestHandler(ChatManager& chatManager, RequestHandlerFactory& requestHandlerFactory);
	RequestResult handleRequest(const RequestInfo& requestInfo) const override;
};
