#include "HelperClasses.h"

void myProxy::setIp(std::string ip)
{
	this->_ip = ip;
}

void myProxy::setPort(int port)
{
	this->_port = port;
}

void myProxy::setPublicKey(std::string publickey)
{
	this->_publickey = publickey;
}

std::string myProxy::getIp(void)
{
	return this->_ip;
}

int myProxy::getPort(void)
{
	return this->_port;
}

std::string myProxy::getPublicKey(void)
{
	return this->_publickey;
}

void myUser::setUsername(std::string username)
{
	this->_username = username;
}

void myUser::setPassword(std::string password)
{
	this->_password = password;
}

std::string myUser::getUsername(void)
{
	return this->_username;
}

std::string myUser::getPassword(void)
{
	return this->_password;
}
