#include <iostream>
#include "Client.h"
#include "WSAInitializer.h"


int main(void){
    try {
        WSAInitializer wsaInit;
        Client client;

        std::string ip = "127.0.0.1";
        uint16_t port = 8826;

        //std::cout << "Insert Supernode Port: ";
        //std::cin >> port;

        client.run(ip, port);
    }
    catch (std::exception& e){
        std::cerr << "An error occured (catched in " __FUNCTION__"): " << e.what() << std::endl;
    }
    
    return 0;
}