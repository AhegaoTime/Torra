#pragma once
#include "IDataBase.h"
#include "LoggedUser.h"
#include <vector>

class ChatManager
{
public:
	ChatManager(IDatabase& m_database);
	~ChatManager(void) = default;

	bool sendMsg(const std::string& username, const std::string& password, const std::string& msg);
	bool endChat(const std::string& username, const std::string& password);   //other user
private:
	IDatabase& m_database;
	std::vector<LoggedUser> m_loggedUsers;
};
