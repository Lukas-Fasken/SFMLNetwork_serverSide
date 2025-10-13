#include <SFML/Network.hpp>
#include <iostream>

int main() {
    sf::TcpListener listener;
    listener.listen(53000); // Listen on port 53000

    sf::TcpSocket client;
    listener.accept(client);
    std::cout << "Client connected!\n";

    while (true) {
        sf::Packet packet;
        if (client.receive(packet) == sf::Socket::Status::Done) {
            std::string msg;
            packet >> msg;
            std::cout << "Received: " << msg << "\n";

            // Echo it back
            sf::Packet sendPacket;
            sendPacket << msg;
            client.send(sendPacket);
        }
    }
}