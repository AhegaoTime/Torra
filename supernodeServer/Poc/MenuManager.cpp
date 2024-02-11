#include "MenuManager.h"

MenuManager::MenuManager(IDatabase& database) : m_database(database) {}

bool MenuManager::startChat(const std::string& username, const std::string& password)
{
	for (auto user : this->m_loggedUsers)
	{
		if (user.getUsername() == username && user.getPassword() != password)
		{
			return true;
		}
	}
	return false;
}

bool MenuManager::logout(const std::string& username, const std::string& password)
{
	int index = 0;
	for (auto user : this->m_loggedUsers)
	{
		if (user.getUsername() == username)
		{
			if (user.getPassword() == password)
			{
				this->m_loggedUsers.erase(this->m_loggedUsers.begin() + index);
				return true;
			}		
		}
		index++;
	}
}

