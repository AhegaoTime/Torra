#include "DB.h"


DBServer::DBServer()
{
	this->open();
}

DBServer::~DBServer()
{
	this->close();
}

const void DBServer::addNewProxy(const std::string& ip, int port, const std::string& publickey)
{
	std::string sqlStatment = "INSERT INTO PROXIES(IP, PORT, PUBLICKEY) VALUES('" + ip + "', " + std::to_string(port) + ",'" + publickey + "'); ";
	send_statment(sqlStatment, this->db);
}

const myProxy DBServer::getProxy(int id) const
{
	std::string sqlStatment = "SELECT * FROM PROXIES WHERE (ID = " + std::to_string(id) + ");";
	myProxy proxy;
	send_callback_statment(sqlStatment, this->db, &getProxyCallback, &proxy);
	return proxy;
}

int DBServer::getProxiesCount()
{
	std::string sqlStatment = "SELECT COUNT(ID) FROM PROXIES;";
	int count = 0;
	send_callback_statment(sqlStatment, this->db, &getProxyCountCallback, &count);
	return count;
}

void DBServer::deleteProxy(const std::string& ip, int port, const std::string& publickey)
{
	std::string sqlStatment = "DELETE FROM PROXIES WHERE (IP='" + ip + "',PORT=" + std::to_string(port) + ", PUBLICKEY='" + publickey +"'); ";
	send_statment(sqlStatment, this->db);
}

const void DBServer::addNewUser(const std::string& name, const std::string& password)
{
	std::string sqlStatment = "INSERT INTO USERS(USERNAME, PASSWORD) VALUES('" + name + "','" + password + "');";
	send_statment(sqlStatment, this->db);
}

const myUser DBServer::getUser(const std::string& name, const std::string& password) const
{
	std::string sqlStatment = "SELECT * FROM USERS WHERE (USERNAME='" + name + "',PASSWORD='" + password +"');";
	myUser user;
	send_callback_statment(sqlStatment, this->db, &getUserCallback, &user);
	return user;
}

void DBServer::deleteUser(const std::string& name, const std::string& password)
{
	std::string sqlStatment = "DELETE FROM USERS WHERE(USERNAME='" + name + "',PASSWORD='" + password + "');";
	send_statment(sqlStatment, this->db);
}

int DBServer::getProxyCountCallback(void* data, int argc, char** argv, char** azColName)
{
	int* count = (int*)data;
	*count = atoi(argv[0]);
	return 0;
}

int DBServer::getProxyCallback(void* data, int argc, char** argv, char** azColName)
{
	myProxy* proxy = (myProxy*)data;
	for (int i = 0; i < argc; i++)
	{
		if (std::string(azColName[i]) == "IP")
		{
			proxy->setIp(argv[i]);
		}
		else if (std::string(azColName[i]) == "PORT")
		{
			proxy->setPort(std::stoi(argv[i]));
		}
		else if (std::string(azColName[i]) == "PUBLICKEY")
		{
			proxy->setPublicKey(argv[i]);
		}
	}
	return 0;
}

int DBServer::getUserCallback(void* data, int argc, char** argv, char** azColName)
{
	myUser* user = (myUser*)data;
	for (int i = 0; i < argc; i++)
	{
		if (std::string(azColName[i]) == "USERNAME")
		{
			user->setUsername(argv[i]);
		}
		else if (std::string(azColName[i]) == "PASSWORD")
		{
			user->setPassword(argv[i]);
		}
	}
	return 0;
}

bool DBServer::open()
{
	std::string sqlStatement;
	char* errMessage = nullptr;
	int doesFileExist = _access(this->dbFileName.c_str(), 0);
	int res = sqlite3_open(this->dbFileName.c_str(), &this->db);
	if (res != SQLITE_OK) {
		this->db = nullptr;
		std::cout << "Failed to open DB" << std::endl;
		return false;
	}

	sqlStatement = "CREATE TABLE IF NOT EXISTS PROXIES(\
		ID INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL UNIQUE,\
		IP TEXT NOT NULL,\
		PORT INTEGER NOT NULL,\
		PUBLICKEY TEXT NOT NULL\
		);";

	send_statment(sqlStatement, this->db);

	sqlStatement = "CREATE TABLE IF NOT EXISTS USERS(\
		ID INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL UNIQUE,\
		USERNAME TEXT NOT NULL,\
		PASSWORD TEXT NOT NULL\
		);";

	send_statment(sqlStatement, this->db);

	return true;
}

void DBServer::close()
{
	sqlite3_close(this->db);
	this->db = nullptr;
}

void DBServer::clear()
{
	std::string sqlStatement = "DROP TABLE PROXIES;";
	send_statment(sqlStatement, this->db);
}

void DBServer::send_statment(const std::string& statement, sqlite3* db)
{
	char* errMessage = nullptr;
	int res = sqlite3_exec(db, statement.c_str(), nullptr, nullptr, &errMessage);
	if (res != SQLITE_OK)
		std::cout << "Failed to make Statement" << std::endl;
}

void DBServer::send_callback_statment(const std::string& statement, sqlite3* db, int(*callback)(void*, int, char**, char**), void* data)
{
	char* errMessage = nullptr;
	int res = sqlite3_exec(db, statement.c_str(), callback, data, &errMessage);
	if (res != SQLITE_OK)
		std::cout << "Failed to get answer" << std::endl;
}
