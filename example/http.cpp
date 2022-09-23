#include <io.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define ENDL "\r\n"

constexpr auto headers =
        "HTTP/1.1 200 OK" ENDL
        "Content-Type: text/plain" ENDL
        "Connection: Closed" ENDL
        ENDL;


void on_http_request(const io::file client, const io::address address);

int main() {
    async::init([] {
        int sock;
        if ((sock = -io::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0)) < 0) {
            throw std::runtime_error("io_socket()");
        }
        int opt = 1;
        if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) < 0) {
            throw std::runtime_error("setsockopt()");
        }
        struct sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_port = htons(1234);
        inet_aton("127.0.0.1", &addr.sin_addr);
        socklen_t addr_len = sizeof(addr);
        if (bind(sock, (struct sockaddr *)&addr, addr_len) < 0) {
            throw std::runtime_error("bind()");

        }
        if (listen(sock, 1024) == -1) {
            throw std::runtime_error("listen()");
        }

        io::file socket{sock, true};


        while(true) {
            io::address client_address{};
            try {
                auto client = socket.accept(client_address);
                async::defer(std::bind(on_http_request, client, client_address));
            } catch(std::runtime_error& error) {
                io::file(2).write(error.what());
                break;
            }

        }
    });
}


void on_http_request(const io::file client, const io::address address) {
    client.write(headers);

    auto in_addr = (struct sockaddr_in *) &address.addr;
    auto ip = inet_ntoa(in_addr->sin_addr);
    auto port = ntohs(in_addr->sin_port);
    client.write(std::string(ip) + ":" + std::to_string(port));
    client.close();
}