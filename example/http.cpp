#include <io.h>
#define ENDL "\r\n"

constexpr auto headers =
        "HTTP/1.1 200 OK" ENDL
        "Content-Type: text/plain" ENDL
        "Connection: Closed" ENDL
        ENDL;


void on_http_request(const io::client client, const io::address address);

int main() {
    async::init([] {

        auto socket = io::server::create_tcp("127.0.0.1", 1234);

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

        socket.close();
    });
}

void finish_client(const io::network_stream& stream) {
    stream.shutdown();
    stream.close();
}

void on_http_request(const io::client client, const io::address address) {
    client.send(headers);
    client.send(address.ip() + ":" + std::to_string(address.port()));
    finish_client(client);
}