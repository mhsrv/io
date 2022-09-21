#include <io.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define ENDL "\r\n"

int main() {
    async::init([] {
        int sock;
        if ((sock = io::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0)) < 0) {
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
        int client;
        while((client = io::accept(sock, (struct sockaddr *) &addr, &addr_len, 0)) > 0) {
            async::defer([client] {
                io::file connection{client, true};
                connection.write(
                        "HTTP/1.1 200 OK" ENDL
                        "Content-Type: text/html" ENDL
                        "Content-Length: 22" ENDL
                        "Connection: Closed" ENDL
                        ENDL
                        "<h1>Hello, World!</h1>" ENDL
                        );
            });
        }

        io::file(2).write(strerror(client));
        close(sock);

    });
}
