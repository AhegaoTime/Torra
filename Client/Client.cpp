#include "Client.h"

Client::Client(void) : m_communicator() {}

//The main function to run the whole program.
void Client::run(const std::string& ip, const uint16_t port) {
	this->m_communicator.run(ip, port);
}