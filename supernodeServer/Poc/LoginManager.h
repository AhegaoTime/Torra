#pragma once
#include "IDataBase.h"
#include "LoggedUser.h"
#include <vector>

class LoginManager
{
public:
	LoginManager(IDatabase& database);
	~LoginManager(void) = default;

	bool login(const std::string& username, const std::string& password);
	bool signup(const std::string& username, const std::string& password);

private:
	IDatabase& m_database;
	std::vector<LoggedUser> m_loggedUsers;

};