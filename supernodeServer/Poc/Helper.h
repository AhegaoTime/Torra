#pragma once

#include <vector>
#include <string>
#include <WinSock2.h>
#include <cstdint>
#include "RequestCodes.h"
#include "uint32_uint8_converter.h"

class Helper
{
public:
	static RequestCodes::RequestCodes getMessageTypeCode(const SOCKET sc);
	static uint32_t getDataLength(const SOCKET sc);
	static int getIntPartFromSocket(const SOCKET sc, const int bytesNum);
	static std::string getStringPartFromSocket(SOCKET sc, const int bytesNum);
	static void sendData(const SOCKET sc, const std::string& message);
	static std::string getPaddedNumber(const int num, const int digits);
	static void getRawPartFromSocket(const SOCKET& sc, uint8_t* destination, const uint16_t bytesAmount);

private:
	static std::string getPartFromSocket(const SOCKET sc, const int bytesNum);
	static std::string getPartFromSocket(const SOCKET sc, const int bytesNum, const int flags);

};


#ifdef _DEBUG // vs add this define in debug mode
#include <stdio.h>
// Q: why do we need traces ?
// A: traces are a nice and easy way to detect bugs without even debugging
// or to understand what happened in case we miss the bug in the first time
#define TRACE(msg, ...) printf(msg "\n", __VA_ARGS__);
// for convenient reasons we did the traces in stdout
// at general we would do this in the error stream like that
// #define TRACE(msg, ...) fprintf(stderr, msg "\n", __VA_ARGS__);

#else // we want nothing to be printed in release version
#define TRACE(msg, ...) printf(msg "\n", __VA_ARGS__);
#define TRACE(msg, ...) // do nothing
#endif