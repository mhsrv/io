#include <io.h>

#define ENDL "\r\n"

constexpr auto headers =
        "HTTP/1.1 200 OK" ENDL
        "Content-Type: text/plain" ENDL
        "Connection: Closed" ENDL
        ENDL;


utils::void_t on_http_request(const io::client client, const io::address address);

int main() {
    async::init([] -> utils::void_t {
        auto socket = TRY(io::server::create_tcp("127.0.0.1", 1234)).use();
        while(true) {
            io::address client_address{};
            auto client = TRY(socket.accept(client_address));
            async::defer(std::bind(on_http_request, client, client_address));
        }

        VOID_DONE;
    });
}

utils::void_t on_http_request(const io::client client, const io::address address) {
    auto stream = client.use();
    TRY(stream.send(headers));
    TRY(stream.send(address.ip() + ":" + std::to_string(address.port())));
    VOID_RETURN(stream.shutdown());
}
