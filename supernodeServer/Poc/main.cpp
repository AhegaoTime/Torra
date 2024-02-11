#pragma comment (lib, "ws2_32.lib")
#include "Server.h"
#include "WSAInitializer.h"
#include <exception>
#include <thread>
#include <iostream>

int main(void) {
	std::cout << "Hi welcome whatever\n";

	try {
		WSAInitializer wsa_init;
		Server server;
		server.run();
	}
	catch (const std::exception& e) {
		std::cout << e.what() << std::endl;
	}

	return 0;
}





/*
#include "DB.h"
#include <iostream>
#include <cstdlib>

int main()
{
	DBServer db;
	int option = 0, port;
	int random1;
	int random2;
	int random3;
	int flag = 0;
	std::string ip;
	while (option != 3)
	{
		std::cout << "\nWelcome to super node poc!" << std::endl;
		std::cout << "Enter 1 - to become a proxy" << std::endl;
		std::cout << "Enter 2 - to get a proxy route" << std::endl;
		std::cout << "Enter 3 - to Exit" << std::endl;
		std::cout << "Option: ";
		std::cin >> option;
		
		if (option == 1)
		{
			std::cout << "Enter a random port: ";
			std::cin >> port;
			std::cout << "Enter a random ip: ";
			std::cin >> ip;
			db.addNewProxy(ip, port);
			std::cout << "Proxy added!";
		}
		else if (option == 2)
		{
			// Providing a seed value
			srand((unsigned)time(NULL));
			random1 = 1 + (rand() % db.getProxiesCount());
			random2 = 1 + (rand() % db.getProxiesCount());
			random3 = 1 + (rand() % db.getProxiesCount());
			while (flag != 1)
			{
				if (random1 == random2)
				{
					random2 = 1 + (rand() % db.getProxiesCount());
				}
				else if (random1 == random3)
				{
					random1 = 1 + (rand() % db.getProxiesCount());
				}
				else if (random2 == random3)
				{
					random3 = 1 + (rand() % db.getProxiesCount());
				}
				else
				{
					flag = 1;
				}
			}
			flag = 0;
			myProxy p1 = db.getProxy(random1);
			myProxy p2 = db.getProxy(random2);
			myProxy p3 = db.getProxy(random3);
			std::cout << "The route is: " << p1.getIp() << ":" << p1.getPort() << "->" << p2.getIp() << ":" << p2.getPort() << "->" << p3.getIp() << ":" << p3.getPort() << std::endl;
		}
	}
	std::cout << "Goodbye!" << std::endl;
	return 0;
}
*/