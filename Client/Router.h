#pragma once
#include "Cryptor.h"

struct Router {
public:
	uint32_t ip;
	uint16_t port;
	Cryptor cryptor;

	Router(void) : cryptor() {
		this->ip = 0;
		this->port = 0;
	}
};