#include "ChatRequestHandler.h"

ChatRequestHandler::ChatRequestHandler(ChatManager& chatManager, RequestHandlerFactory& requestHandlerFactory) : m_chatManager(chatManager), m_requestHandlerFactory(requestHandlerFactory), IRequestHandler(RequestCodes::RequestTypeHandler::Chat) {}

RequestResult ChatRequestHandler::handleRequest(const RequestInfo& requestInfo) const
{
	switch (requestInfo.requestCode)
	{
	case RequestCodes::RequestCodes::SendMsg: return this->sendMsg(requestInfo);
	case RequestCodes::RequestCodes::CloseChat: return this->closeChat(requestInfo);
	default: throw std::exception("Non existing code in NetworkRequestHandler::handleRequest()"); //Will never happen. I hope.
	}
}

RequestResult ChatRequestHandler::sendMsg(const RequestInfo& requestInfo) const
{
	MsgChatRequest msgChatRequest = JsonRequestPacketDeserializer::desirializeSendMsgRequest(requestInfo.buffer);

	//Checks if the function succeeds.
	if (this->m_chatManager.sendMsg(msgChatRequest.Username, msgChatRequest.Password, msgChatRequest.Msg)) {
		MsgChatResponse msgChatResponse{ RequestReponseStatus::Success };
		RequestResult requestResult;

		requestResult.response = JsonResponsePacketSerializer::serializeResponse(msgChatResponse);
		requestResult.newHandler = (IRequestHandler*)this->m_requestHandlerFactory.createChatRequestHandler();
		return requestResult;
	}
	MsgChatResponse msgChatResponse = { msgChatResponse.status = RequestReponseStatus::Fail };
	RequestResult requestResult;

	requestResult.response = JsonResponsePacketSerializer::serializeResponse(msgChatResponse);
	requestResult.newHandler = (IRequestHandler*)this;

	return requestResult;
}

RequestResult ChatRequestHandler::closeChat(const RequestInfo& requestInfo) const
{
	CloseChatRequest closeChatRequest = JsonRequestPacketDeserializer::desirializeCloseChatRequest(requestInfo.buffer);

	//Checks if the function succeeds.
	if (this->m_chatManager.endChat(closeChatRequest.Username, closeChatRequest.Password)) {
		CloseChatResponse closeChatResponse{ RequestReponseStatus::Success };
		RequestResult requestResult;

		requestResult.response = JsonResponsePacketSerializer::serializeResponse(closeChatResponse);
		requestResult.newHandler = (IRequestHandler*)this->m_requestHandlerFactory.createMenuRequestHandler();
		return requestResult;
	}
	CloseChatResponse closeChatResponse = { closeChatResponse.status = RequestReponseStatus::Fail };
	RequestResult requestResult;

	requestResult.response = JsonResponsePacketSerializer::serializeResponse(closeChatResponse);
	requestResult.newHandler = (IRequestHandler*)this;

	return requestResult;
}
