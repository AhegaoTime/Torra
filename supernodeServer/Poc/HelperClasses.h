#pragma once
#include <string>

class myProxy
{
public:
	myProxy() = default;
	~myProxy() = default;

	void setIp(std::string ip);
	void setPort(int port);
	void setPublicKey(std::string publickey);

	std::string getIp(void);
	int getPort(void);
	std::string getPublicKey(void);

private:
	std::string _ip;
	int _port;
	std::string _publickey;
};


class myUser
{
public:
	myUser() = default;
	~myUser() = default;

	void setUsername(std::string username);
	void setPassword(std::string password);

	std::string getUsername(void);
	std::string getPassword(void);

private:
	std::string _username;
	std::string _password;
};