#pragma once
#include <string>
class LoggedUser
{
public:
	LoggedUser(const std::string& username, const std::string& password);
	~LoggedUser() = default;
	std::string getUsername(void) const;
	std::string getPassword(void) const;

	bool operator==(const LoggedUser& other) const;

private:
	std::string m_username;
	std::string m_password;
};