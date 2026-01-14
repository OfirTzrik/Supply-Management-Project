#include <iostream>
#include <thread>
#include <string>
#include <sstream>
#include <vector>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "InventoryManager.h"

using namespace std;

const int gNumClients = 5;

// Receive the string character by character
bool recv_line(int fd, std::string& out) {
    out.clear();
    char c;
    while (true) {
        ssize_t n = recv(fd, &c, 1, 0);
        // String is empty, nothing received
        if (n <= 0) {
            return false;
        }
        // End of the string was reached
        if (c == '\n' || c == '\0') {
            break;
        }
        out.push_back(c);
    }
    return true;
}

// Send the answer character by character
void send_all(int fd, const std::string& msg) {
    size_t sent = 0;
    // Keep going as long as there is something to send
    while (sent < msg.size()) {
        ssize_t n = send(fd, msg.data() + sent, msg.size() - sent, 0);
        // Sent everything
        if (n <= 0) {
            return;
        }
        sent += n;
    }
}

void handle_client(int client_fd, InventoryManager& inv) {
    send_all(client_fd, "Type commands: HELLO <username>, LIST, BORROW <id>, WAIT <id>\n");

    string message_buffer;
    bool auth = false;
    string username;

    // Keep receiving messages until the client says QUIT
    while (true) {
        if (!recv_line(client_fd, message_buffer)) {
            close(client_fd);
            return;
        }

        // Split message into words
        istringstream iss(message_buffer);
        vector<string> words;
        string w;
        while (iss >> w) {
            words.push_back(w);
        }
        cout << words.size() << "\n";

        if (words.at(0) == "QUIT") {
            send_all(client_fd, "OK BYE\n");
            cout << "Client disconnected\n";
            close(client_fd);
            return;
        }
        if (!auth) {
            if (words.size() == 2 && words.at(0) == "HELLO") {
                auth = true;
                username = words.at(1);
                send_all(client_fd, "OK HELLO\n");
                continue;
            } else {
                send_all(client_fd, "ERR: Invalid command \'" + message_buffer + "\'\n");
            }
        }

        if (auth) {
            if (words.size() == 1 && words.at(0) == "LIST") {
                std::string list = inv.listItems();
                send_all(client_fd, list);
            } else {
                cout << "TEST\n";
                send_all(client_fd, "WTF IS GOING ON\n");
            }
        }

    }
}

int main(int argc, char* argv[]) {

    if (argc < 2) {
        cerr << "ERR: No port to listen on was passed\n";
        return 1;
    }

    // Create a socket and if it was not created successfully let the user know
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        cerr << "ERR: Failed to create socket\n";
        return 1;
    }

    sockaddr_in server_addr {};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(atoi(argv[1]));

    /* Bind the socket with the address the server is going to listen on, if the
    process failed then close the socket */
    if (bind(server_fd, (sockaddr*) & server_addr, sizeof(server_addr))) {
        cerr << "Bind failed, port still not released by the operating system\n";
        close(server_fd);
        return 1;
    }

    if (listen(server_fd, gNumClients) < 0) {
        cerr << "Listen failed\n";
        return 1;
    }
    cout << "Server listening on port " << argv[1] << "\n";
    
    // Create and pass the shared inventory to each client
    InventoryManager inv;
    while (true) {
        int client_fd = accept(server_fd, nullptr, nullptr);
        cout << "Client connected\n";
        thread t(handle_client, client_fd, ref(inv));
        t.detach();
    }
    
    close(server_fd);
    return 0;
}