#pragma once
#include "Communicator.h"

class Client {
private:
	Communicator m_communicator;

public:
	Client(void);

	//main:
	void run(const std::string& ip, const uint16_t port); //The main function to run the whole program.
};