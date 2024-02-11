#pragma once
#include "RequestInfo.h"
#include "RequestCodes.h"
#include "RequestResult.h"

struct RequestResult;

class IRequestHandler {
private:
	const RequestCodes::RequestTypeHandler m_requestTypeHandler;
public:
	IRequestHandler(const RequestCodes::RequestTypeHandler& requestTypeHandler);

	bool isRequestRelevant(const RequestInfo& requestInfo) const; //Not virtual anymore. All request handlers use the same function now but they have different value in m_requestTypeHandler (from their constructor).
	const bool isNetworkRequestHandler(void) const;
	virtual RequestResult handleRequest(const RequestInfo& requestInfo) const = 0;
};