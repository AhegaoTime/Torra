#include "LoggedUser.h"

LoggedUser::LoggedUser(const std::string& username, const std::string& password)
{
    this->m_username = username;
    this->m_password = password;
}

std::string LoggedUser::getUsername(void) const
{
    return this->m_username;
}

std::string LoggedUser::getPassword(void) const
{
    return this->m_password;
}

bool LoggedUser::operator==(const LoggedUser& other) const {
    return (this->m_username == other.m_username) && (this->m_password == other.m_password);
}