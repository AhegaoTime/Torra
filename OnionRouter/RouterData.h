#pragma once
#include <cstdint>


struct RouterData {
	uint8_t routerIndex;
	uint32_t nextIp;
	uint16_t nextPort;

	RouterData(void) {
		this->routerIndex = 0;
		this->nextIp = 0;
		this->nextPort = 0;
	}

	RouterData(uint8_t routerIndex, uint32_t nextIp, uint16_t nextPort) {
		this->routerIndex = routerIndex;
		this->nextIp = nextIp;
		this->nextPort = nextPort;
	}
};