#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream>
#include <string>
#include <sstream>
#include <thread>
#include <vector>

using namespace std;

bool recv_line(int fd, string& out) {
    out.clear();
    char c;
    while (true) {
        ssize_t n = recv(fd, &c, 1, 0);
        if (n <= 0) return false;
        if (c == '\n') break;
        out.push_back(c);
    }
    istringstream iss(out);
    vector<string> words;
    string w;
    while (iss >> w) {
        words.push_back(w);
    }
    if (words.size() == 3)
    {
        int count = 0;
        if(words[1] == "LIST"){
            out.push_back('\n');
            count = atoi(words[2].c_str());
            while (count > 0) {
                ssize_t n = recv(fd, &c, 1, 0);
                if (n <= 0) return false;
                if (c == '\n') count --;
                out.push_back(c);
            }
        }
    }
    return true;
}

void send_all(int fd, const string& msg) {
    size_t sent = 0;
    while (sent < msg.size()) {
        ssize_t n = send(fd, msg.data() + sent, msg.size() - sent, 0);
        if (n <= 0) return;
        sent += n;
    }
}


int main(int argc, char const *argv[]) {
    if (argc < 3){
    cout << "missing values";
    return 0;
    }
    string ip = (argv[1]);
    int port = atoi(argv[2]);

    // start the connection 

    int fd = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in srv{};
    srv.sin_family = AF_INET;
    srv.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str() , &srv.sin_addr);

    if (connect(fd, (sockaddr*)&srv, sizeof(srv)) < 0){
        cerr << "Server not found";
        close(fd);
        return 0;
    }
    cout << "Connected to " << ip << ":" << to_string(port) << endl;
    string reply;
    recv_line(fd, reply);
    cout << reply << endl;
    
    // sending messeges 
    while(true){
        string sentence;
        getline(cin, sentence) ;

        string msg = sentence + "\n";
        send_all(fd, msg);

        string reply;
        if (!recv_line(fd, reply) && sentence != "QUIT"){
            cerr << "Disconnected from server";
            break;
        }
        cout << reply << endl;

        if (sentence == "QUIT") break;
    }

    close(fd);


}
