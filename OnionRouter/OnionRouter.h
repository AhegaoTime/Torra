#pragma once

#include "Communicator.h"


class OnionRouter {
private:
	Communicator m_communicator;

public:
	OnionRouter(const std::string& ip, const uint16_t port);

	void run(void);
};