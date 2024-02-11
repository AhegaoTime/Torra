#pragma once
#include "sqlite3.h"
#include "IDatabase.h"
#include <stdio.h>
#include <iostream>
#include <io.h>
#include <string>



class DBServer : public IDatabase
{
public:
	DBServer();
	~DBServer();

	const void addNewProxy(const std::string& ip, int port, const std::string& publickey);
	const myProxy getProxy(int id) const;
	int getProxiesCount();
	void deleteProxy(const std::string& ip, int port, const std::string& publickey);

	const void addNewUser(const std::string& name, const std::string& password);
	const myUser getUser(const std::string& name, const std::string& password) const;
	void deleteUser(const std::string& name, const std::string& password);

	static int getProxyCountCallback(void* data, int argc, char** argv, char** azColName);
	static int getProxyCallback(void* data, int argc, char** argv, char** azColName);

	static int getUserCallback(void* data, int argc, char** argv, char** azColName);

	//database related
	bool open();
	void close();
	void clear();

private:
	sqlite3* db;
	std::string dbFileName = "DBServer.sqlite";

	//helper metohd
	static void send_statment(const std::string& statement, sqlite3* db);
	static void send_callback_statment(const std::string& statement, sqlite3* db, int (*callback)(void*, int, char**, char**), void* data);
};