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
bool recv_line(int fd, string& out) {
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
void send_all(int fd, const string& msg) {
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

// Handle empty messages from the client and the QUIT command
bool disconnect_client(const vector<string>& words, int client_fd) {
    if (words.size() == 0 || words.at(0) == "QUIT") {
        send_all(client_fd, "OK BYE\n");
        cout << "Client disconnected\n";
        close(client_fd);
        return true;
    }
    return false;
}

/* As long at the client did not authenticate they will only
have the HELLO command available to them, otherwise every other
command is not available */
bool authentication_loop(const vector<string>& words, bool& auth, int client_fd, string& username, const string& message_buffer) {
    if (!auth) {
        // Client authenticated
        if (words.size() == 2 && words.at(0) == "HELLO") {
            auth = true;
            username = words.at(1);
            send_all(client_fd, "OK HELLO\n");
            return true;
        // Client did not authenticate
        } else if (words.at(0) == "LIST" || words.at(0) == "BORROW" || words.at(0) == "WAIT") {
            send_all(client_fd, "ERR STATE not_authenticated\n");
        } else {
            send_all(client_fd, "ERR INVALID COMMAND \'" + message_buffer + "\'\n");
        }
    }
    return false;
}

/* Let the users execute commands that are available to them
after completing authentication with HELLO <username> */
void execute_commands(const vector<string>& words, InventoryManager& inv, const string& username, int client_fd) {
    // Commands with 1 part (Only LIST)
    if (words.size() == 1 && words.at(0) == "LIST") {
        std::string list = inv.listItems();
        send_all(client_fd, list);
    // Commands with 2 parts
    } else if (words.size() == 2) {
        try {
            int itemId = atoi(words.at(1).c_str());
            if (words.at(0) == "BORROW") {
                inv.borrowItem(itemId, username);
                send_all(client_fd, "OK BORROWED " + to_string(itemId) + "\n");
            } else if (words.at(0) == "RETURN") {
                inv.returnItem(itemId, username);
                send_all(client_fd, "OK RETURNED " + to_string(itemId) + "\n");
            } else if (words.at(0) == "WAIT") {
                inv.waitUntilAvailable(itemId, username);
                send_all(client_fd, "OK AVAILABLE " + to_string(itemId) + "\n");
            // Unknown or incomplete commands
            } else {
                send_all(client_fd, "ERR INVALID COMMAND unknown or incomplete\n");
            }
        } catch (const runtime_error& msg) {
            send_all(client_fd, string(msg.what()) + '\n');
        }
    // Unknown or incomplete commands
    } else {
        send_all(client_fd, "ERR INVALID COMMAND unknown or incomplete\n");
    }
}

void handle_client(int client_fd, InventoryManager& inv) {
    send_all(client_fd, "Type commands: QUIT, HELLO <username>, LIST, BORROW <id>, WAIT <id>\n");

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
        vector<string> words {};
        string w;
        while (iss >> w) {
            words.push_back(w);
        }

        /* Handle empty messages (which results in empty words vector)
        and also handle the client sending "QUIT" */
        if (disconnect_client(words, client_fd)) {
            return;
        }

        /* Authenticate the client once they use HELLO <username>,
        otherwise they are "stuck" here */
        if (authentication_loop(words, auth, client_fd, username, message_buffer)) {
            continue;
        }

        /* Execute commands that are available to the user after authentication,
        unknown or incomplete commands are answered accordingly */
        if (auth) {
            execute_commands(words, inv, username, client_fd);
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

    /* Bind the socket with the address the server is going to
    listen on, if the process failed then close the socket */
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