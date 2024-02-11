#pragma once
#include <string>
#include "IRequestHandler.h"
#include "JsonRequestPacketDeserializer.h"
#include "JsonResponsePacketSerializer.h"
#include "LoginManager.h"
#include "RequestHandlerFactory.h"

class LoginRequestHandler : public IRequestHandler {
private:
	//Variables:
	LoginManager& m_loginManager;
	RequestHandlerFactory& m_requestHandlerFactory;

	//Functions:
	RequestResult login(const RequestInfo& requestInfo) const;
	RequestResult singup(const RequestInfo& requestInfo) const;
public:
	LoginRequestHandler(LoginManager& loginManager, RequestHandlerFactory& requestHandlerFactory);
	RequestResult handleRequest(const RequestInfo& requestInfo) const override;
};