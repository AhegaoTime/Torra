#include "ChatManager.h"

ChatManager::ChatManager(IDatabase& database) : m_database(database) {}

bool ChatManager::sendMsg(const std::string& username, const std::string& password, const std::string& msg)
{
	return true;
}

bool ChatManager::endChat(const std::string& username,const std::string& password)
{
	for (auto user : this->m_loggedUsers)
	{
		if (user.getUsername() == username)
		{
			if (user.getPassword() != password)
			{
				return true;
			}
		}
	}
	return false;
}
