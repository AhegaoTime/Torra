#include "Helper.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>

using std::string;

// Receives the type code of the message from socket (1 byte) and returns the code. 
// If no message found in the socket returns Ignore (which means the client disconnected).
RequestCodes::RequestCodes Helper::getMessageTypeCode(const SOCKET sc)
{
	std::string msg = getPartFromSocket(sc, 1, 0);

	if (msg != "" && RequestCodes::isValidRequestCode(msg[0])) {
		return (RequestCodes::RequestCodes)msg[0];
	}

	return RequestCodes::RequestCodes::Ignore;
}

//Receives the length field and returns it in uint32_t format.
uint32_t Helper::getDataLength(const SOCKET sc) {
	uint8_t length_uint8[UINT8_PER_UINT32] = { '\0' };
	Helper::getRawPartFromSocket(sc, length_uint8, UINT8_PER_UINT32);
	return uint32_uint8_converter::uint8_to_uint32(length_uint8);
}

// recieve data from socket according byteSize
// returns the data as int
int Helper::getIntPartFromSocket(const SOCKET sc, const int bytesNum)
{
	return atoi(getPartFromSocket(sc, bytesNum, 0).c_str());
}

// recieve data from socket according byteSize
// returns the data as string
string Helper::getStringPartFromSocket(const SOCKET sc, const int bytesNum)
{
	return getPartFromSocket(sc, bytesNum, 0);
}

// return string after padding zeros if necessary
string Helper::getPaddedNumber(const int num, const int digits)
{
	std::ostringstream ostr;
	ostr << std::setw(digits) << std::setfill('0') << num;
	return ostr.str();

}

// recieve data from socket according byteSize
// this is private function
std::string Helper::getPartFromSocket(const SOCKET sc, const int bytesNum)
{
	return getPartFromSocket(sc, bytesNum, 0);
}

// send data to socket
// this is private function
void Helper::sendData(const SOCKET sc, const std::string& message)
{
	const char* data = message.c_str();

	if (send(sc, data, message.size(), 0) == INVALID_SOCKET)
	{
		throw std::exception("Error while sending message to client");
	}
}

std::string Helper::getPartFromSocket(const SOCKET sc, const int bytesNum, const int flags)
{
	if (bytesNum == 0)
	{
		return "";
	}

	char* data = new char[bytesNum + 1];
	int res = recv(sc, data, bytesNum, flags);
	if (res == INVALID_SOCKET)
	{
		std::string s = "Error while recieving from socket: ";
		s += std::to_string(sc);
		throw std::exception(s.c_str());
	}
	data[bytesNum] = 0;
	std::string received(data);
	delete[] data;
	return received;
}

void Helper::getRawPartFromSocket(const SOCKET& sc, uint8_t* destination, const uint16_t bytesAmount) {
	int res = recv(sc, (char*)destination, bytesAmount, 0);

	if (res == INVALID_SOCKET){
		std::string s = "Error while recieving from socket: ";
		s += std::to_string(sc);
		throw std::exception(s.c_str());
	}
}