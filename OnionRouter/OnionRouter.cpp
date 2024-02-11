#include "OnionRouter.h"

OnionRouter::OnionRouter(const std::string& ip, const uint16_t port) : m_communicator(ip, port) {}

void OnionRouter::run(void) {
	//std::thread tr(&Communicator::startHandleRequests, &this->m_communicator);
	this->m_communicator.startHandleRequests();
}