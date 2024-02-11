#include "LoginManager.h"

LoginManager::LoginManager(IDatabase& database) : m_database(database) {}

bool LoginManager::login(const std::string& username, const std::string& password)
{
	myUser user = this->m_database.getUser(username, password);
	if (user.getUsername() == username && user.getPassword() == password)
	{
		this->m_loggedUsers.push_back(LoggedUser(username, password));
		return true;
	}
	return false;
}

bool LoginManager::signup(const std::string& username, const std::string& password)
{

	for (auto user : this->m_loggedUsers)
	{
		if (user.getUsername() == username)
		{
			return false;
		}
	}
	this->m_database.addNewUser(username, password);
	this->m_loggedUsers.push_back(LoggedUser(username, password));
	return true;
}


