#include <io.h>
#define ENDL "\r\n"

constexpr auto headers =
        "HTTP/1.1 200 OK" ENDL
        "Content-Type: text/plain" ENDL
        "Connection: Closed" ENDL
        ENDL;


io::void_t on_http_request(const io::client client, const io::address address);

int main() {
    async::init([] -> io::void_t {
        auto socket = IO_TRY(io::server::create_tcp("127.0.0.1", 1234)).use();
        while(true) {
            io::address client_address{};
            auto client = IO_TRY(socket.accept(client_address));
            async::defer(std::bind(on_http_request, client, client_address));
        }

        IO_VOID_DONE;
    });
}

io::void_t on_http_request(const io::client client, const io::address address) {
    auto stream = client.use();
    IO_TRY(stream.send(headers));
    IO_TRY(stream.send(address.ip() + ":" + std::to_string(address.port())));
    IO_VOID_RETURN(stream.shutdown());
}
