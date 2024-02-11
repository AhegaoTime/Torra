//I don't expect it to work on other computers yet because it has dependencies on specific paths in my computer.
#include "OnionRouter.h"
#include "WSAInitializer.h"

int main(void) {
	try
	{
		WSAInitializer wsaInit;
		std::string ip = "127.0.0.1";
		uint16_t port = 8826;

		/*//I don't care about valid input currently.
		std::cout << "Insert Supernode IP: ";
		std::cin >> ip;*/
		//std::cout << "Insert Supernode Port: ";
		//std::cin >> port;


		OnionRouter onionRouter(ip, port);

		onionRouter.run();
	}
	catch (std::exception& e)
	{
		std::cerr << "An error occured: " << e.what() << std::endl;
	}

	system("pause");
	return 0;
}