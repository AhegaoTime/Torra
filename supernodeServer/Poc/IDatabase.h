#pragma once
#include <string>
#include <list>
#include "helperClasses.h"

class IDatabase
{
public:
	virtual ~IDatabase() = default;

	virtual const void addNewProxy(const std::string& ip, int port, const std::string& publickey) = 0;
	virtual const myProxy getProxy(int id) const = 0;
	virtual void deleteProxy(const std::string& ip, int port, const std::string& publickey) = 0;
	virtual int getProxiesCount() = 0;

	virtual const void addNewUser(const std::string& name, const std::string& password) = 0;
	virtual const myUser getUser(const std::string& name, const std::string& password) const = 0;
	virtual void deleteUser(const std::string& name, const std::string& password) = 0;

	virtual bool open() = 0;
	virtual void close() = 0;
	virtual void clear() = 0;
};