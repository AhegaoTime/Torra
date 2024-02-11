#include "IRequestHandler.h"

IRequestHandler::IRequestHandler(const RequestCodes::RequestTypeHandler& requestTypeHandler) : m_requestTypeHandler(requestTypeHandler) {}

bool IRequestHandler::isRequestRelevant(const RequestInfo& requestInfo) const {
	return RequestCodes::isRequestRelevant(requestInfo.requestCode, this->m_requestTypeHandler);
}

const bool IRequestHandler::isNetworkRequestHandler(void) const {
	return this->m_requestTypeHandler == RequestCodes::RequestTypeHandler::Network;
}