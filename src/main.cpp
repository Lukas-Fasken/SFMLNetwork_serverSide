#include <SFML/Network.hpp>
#include <nlohmann/json.hpp>
#include <iostream>
#include <vector>
#include <memory>
#include <atomic>
#include <csignal>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <string>

// Simple multi-client TCP server using SFML
// Listens on port 54000, accepts clients, receives text messages and broadcasts them to other clients.

std::atomic<bool> g_running{true};

// Outgoing messages typed on server console are queued here. The main loop
// drains the queue and broadcasts messages to connected clients. This is
// simpler and more robust than an atomic flag + single message string.
std::mutex outMutex;
std::condition_variable outCv;
std::vector<std::string> outQueue;

void signal_handler(int) {
	g_running = false;
}

void input_thread_func() {
    std::string line;
    while (g_running) {
        std::getline(std::cin, line);
        if (!g_running) break;

        if (line == "/quit") {
            g_running = false;
            break;
        } else {
			{
				std::lock_guard<std::mutex> lock(outMutex);
				outQueue.push_back(line);
			}
			outCv.notify_one();
        }
    }
}

int main() {
	// Install a basic SIGINT handler so Ctrl+C will stop the server gracefully
	std::signal(SIGINT, signal_handler);

	const unsigned short PORT = 54000;

	sf::TcpListener listener;
	if (listener.listen(PORT) != sf::Socket::Status::Done) {
		std::cerr << "Failed to bind/listen on port " << PORT << "\n";
		return 1;
	}

	std::cout << "Server listening on port " << PORT << "\n";

	sf::SocketSelector selector;
	selector.add(listener);

	// Keep sockets as pointers so they can be moved around and referenced by selector
	std::vector<std::unique_ptr<sf::TcpSocket>> clients;

	auto remoteToString = [](const sf::TcpSocket& sock) {
		std::string addr = "unknown";
		std::string port = "?";
		if (auto a = sock.getRemoteAddress()) addr = a->toString();
		unsigned short p = sock.getRemotePort();
		if (p != 0) port = std::to_string(p);
		return addr + ":" + port;
	};

	std::thread input_thread(input_thread_func);

	while (g_running) {
		// Wait for activity
		if (selector.wait(sf::milliseconds(500))) {
			// New connection?
			if (selector.isReady(listener)) {
				auto client = std::make_unique<sf::TcpSocket>();
				if (listener.accept(*client) == sf::Socket::Status::Done) {
					std::cout << "New client connected: " << remoteToString(*client) << '\n';
					client->setBlocking(false);
					selector.add(*client);
					clients.push_back(std::move(client));
				} else {
					std::cerr << "Failed to accept new connection\n";
				}
			}

			// Check existing clients
			for (size_t i = 0; i < clients.size(); ) {
				sf::TcpSocket& socket = *clients[i];

				if (selector.isReady(socket)) {
					sf::Packet packet;
					sf::Socket::Status status = socket.receive(packet);
					if (status == sf::Socket::Status::Done) {
						std::string msg;
						if (packet >> msg) {
							std::cout << "Received from " << remoteToString(socket) << ": " << msg << '\n';
							// Broadcast to other clients
							for (size_t j = 0; j < clients.size(); ++j) {
								if (i == j) continue; // don't send back to sender
								sf::Packet out;
								out << msg;
								if (clients[j]->send(out) != sf::Socket::Status::Done) {
									std::cerr << "Failed to send to client " << j << '\n';
								}
							}
						}
						++i;
					} else if (status == sf::Socket::Status::Disconnected) {
						std::cout << "Client disconnected: " << remoteToString(socket) << '\n';
						selector.remove(socket);
						// erase this client
						clients.erase(clients.begin() + i);
					} else if (status == sf::Socket::Status::NotReady) {
						// No data available right now
						++i;
					} else {
						std::cerr << "Socket error from " << remoteToString(socket) << '\n';
						selector.remove(socket);
						clients.erase(clients.begin() + i);
					}
				} else {
					++i;
				}
			}
		}
		// else: timeout, just loop again to check g_running

		// Drain any outgoing console messages and broadcast them immediately.
		std::vector<std::string> localOut;
		{
			std::lock_guard<std::mutex> lock(outMutex);
			if (!outQueue.empty()) {
				localOut.swap(outQueue);
			}
		}
		for (auto& msgCopy : localOut) {
			std::cout << "[Server]: " << msgCopy << '\n';
			sf::Packet packet;
			packet << msgCopy;
			for (auto& client : clients) {
				if (client)
					client->send(packet);
			}
		}
	}

	std::cout << "Shutting down server...\n";
	// Close all client sockets cleanly
	for (auto& c : clients) {
		c->disconnect();
	}
	listener.close();

	if (input_thread.joinable()) {
		input_thread.join();
	}
	return 0;
}