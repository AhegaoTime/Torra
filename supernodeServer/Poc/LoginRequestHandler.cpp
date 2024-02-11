#include "LoginRequestHandler.h"

LoginRequestHandler::LoginRequestHandler(LoginManager& loginManager, RequestHandlerFactory& requestHandlerFactory) : m_loginManager(loginManager), m_requestHandlerFactory(requestHandlerFactory), IRequestHandler(RequestCodes::RequestTypeHandler::Login) {}

RequestResult LoginRequestHandler::handleRequest(const RequestInfo& requestInfo) const
{
	switch (requestInfo.requestCode) 
	{
	case RequestCodes::RequestCodes::Login: return this->login(requestInfo);
	case RequestCodes::RequestCodes::Signup: return this->singup(requestInfo);
	default: throw std::exception("Non existing code in NetworkRequestHandler::handleRequest()"); //Will never happen. I hope.
	}
}

RequestResult LoginRequestHandler::login(const RequestInfo& requestInfo) const
{
	LoginRequest loginRequest = JsonRequestPacketDeserializer::desirializeLoginRequest(requestInfo.buffer);

	//Checks if the function succeeds.
	if (this->m_loginManager.login(loginRequest.Username, loginRequest.Password)){
		LoginResponse loginResponse{ RequestReponseStatus::Success };
		RequestResult requestResult;

		requestResult.response = JsonResponsePacketSerializer::serializeResponse(loginResponse);
		requestResult.newHandler = (IRequestHandler*)this->m_requestHandlerFactory.createChatRequestHandler();
		return requestResult;
	}
	LoginResponse loginResponse = { loginResponse.status = RequestReponseStatus::Fail };
	RequestResult requestResult;

	requestResult.response = JsonResponsePacketSerializer::serializeResponse(loginResponse);
	requestResult.newHandler = (IRequestHandler*)this;

	return requestResult;
}

RequestResult LoginRequestHandler::singup(const RequestInfo& requestInfo) const
{
	SignupRequest signupRequest = JsonRequestPacketDeserializer::desirializeSignupRequest(requestInfo.buffer);

	//Checks if the function succeeds.
	if (this->m_loginManager.signup(signupRequest.Username, signupRequest.Password)) {
		SignupResponse signupResponse{ RequestReponseStatus::Success };
		RequestResult requestResult;

		requestResult.response = JsonResponsePacketSerializer::serializeResponse(signupResponse);
		requestResult.newHandler = (IRequestHandler*)this->m_requestHandlerFactory.createChatRequestHandler();
		return requestResult;
	}
	SignupResponse signupResponse = { signupResponse.status = RequestReponseStatus::Fail };
	RequestResult requestResult;

	requestResult.response = JsonResponsePacketSerializer::serializeResponse(signupResponse);
	requestResult.newHandler = (IRequestHandler*)this;

	return requestResult;
}
