#include <io.h>
#include <netinet/in.h>
#include <arpa/inet.h>

extern "C" {

int32_t io_mkdir(char *path, mode_t mode) {
    return async::request([path, mode](io_uring_sqe *sqe) {
        io_uring_prep_mkdir(sqe, path, mode);
    });
}

int32_t io_write(int fd, const char *buf, uint32_t amount, size_t offset) {
    return async::request([fd, buf, amount, offset](io_uring_sqe *sqe) {
        io_uring_prep_write(sqe, fd, buf, amount, offset);
    });
}

int32_t io_read(int fd, char *buf, uint32_t amount, size_t offset) {
    return async::request([fd, buf, amount, offset](io_uring_sqe *sqe) {
        io_uring_prep_read(sqe, fd, buf, amount, offset);
    });
}

int32_t io_close(int fd) {
    return async::request([fd](io_uring_sqe* sqe) {
        io_uring_prep_close(sqe, fd);
    });
}

}

int32_t io_socket(int domain, int type, int protocol, int flags) {
    return async::request([domain, type, protocol, flags](io_uring_sqe *sqe) {
        io_uring_prep_socket(sqe, domain, type, protocol, flags);
    });
}

int32_t io_accept(int fd, sockaddr *addr, socklen_t *addrlen, int flags) {
    return async::request([fd, addr, addrlen, flags](io_uring_sqe *sqe) {
        io_uring_prep_accept(sqe, fd, addr, addrlen, flags);
    });
}

int32_t io_bind(int fd, sockaddr *addr, socklen_t addrlen) {
    if (bind(fd, addr, addrlen) < 0) {
        return errno;
    }
    return 0;
}

int32_t io_listen(int fd, int queue) {
    if (listen(fd, queue) < 0) {
        return errno;
    }
    return 0;
}

io::file::file(int fd, bool close) {
    m_fd = fd;
    m_should_close = close;
}

size_t io::file::read(const std::span<char> &buf, size_t offset) const {
    auto err = io::read(m_fd, buf.data(), buf.size(), offset);
    if (err > 0) {
        throw std::runtime_error(std::strerror(err));
    }
    return -err;
}

size_t io::file::write(const std::span<char> &buf, size_t offset) const {
    auto err = io::write(m_fd, buf.data(), buf.size(), offset);
    if (err > 0) {
        throw std::runtime_error(std::strerror(err));
    }
    return -err;
}

io::file::~file() {
    if (m_should_close) {
        io::close(m_fd);
    }
}

size_t io::file::write(const std::string_view &buf, size_t offset) const {
    auto err = io::write(m_fd, buf.data(), buf.size(), offset);
    if (err > 0) {
        throw std::runtime_error(std::strerror(err));
    }
    return -err;
}

io::file io::file::accept(io::address& address, int flags) const {
    address = {};
    auto err = io::accept(m_fd, (sockaddr *)address.m_addr, &address.m_addrlen, flags);
    if (err > 0) {
        throw std::runtime_error(std::strerror(err));
    }
    return {-err};
}

void io::file::close() const {
    io::close(m_fd);
}

void io::file::bind(const io::address& addr) const {
    auto err = io::bind(m_fd, addr.m_addr, addr.m_addrlen);
    if (err) {
        throw std::runtime_error(std::strerror(err));
    }
}

void io::file::listen(int queue) const {
    auto err = io::listen(m_fd, queue);
    if (err) {
        throw std::runtime_error(std::strerror(err));
    }

}

io::file io::file::create_tcp(int options) {
    auto socket = io::file::create_socket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0);
    socket.set_socket_options(SOL_SOCKET, options);
    return socket;
}

io::file io::file::create_tcp(const std::string& ip, int port, int queue, int options) {
    auto socket = io::file::create_tcp(options);
    socket.bind(io::address::from(ip, port));
    socket.listen(queue);
    return socket;
}

io::file io::file::create_socket(int domain, int type, int protocol, int flags) {
    auto err = io::socket(domain, type, protocol, flags);
    if (err > 0) {
        throw std::runtime_error(std::strerror(err));
    }
    return {-err, true};
}

void io::file::set_socket_options(int level, int name, int value) const {
    int opt = 1;
    if (setsockopt(m_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) < 0) {
        throw std::runtime_error(std::strerror(errno));
    }
}


io::address io::address::from(const std::string &ip, short port) {
    auto* addr = new sockaddr_in{};
    addr->sin_family = AF_INET;
    addr->sin_port = htons(port);
    inet_aton(ip.c_str(), &addr->sin_addr);
    return io::address(addr, sizeof(sockaddr_in));
}

io::address::~address() {
    if (m_addr) {
        delete m_addr;
    }
}

io::address::address() {
    if (m_addr) {
        delete m_addr;
    }
    m_addr = new sockaddr{};
    m_addrlen = sizeof(sockaddr);
}

uint16_t io::address::port() const {
    auto in_addr = reinterpret_cast<sockaddr_in *>(m_addr);
    return ntohs(in_addr->sin_port);
}

std::string io::address::ip() const {
    auto in_addr = reinterpret_cast<sockaddr_in *>(m_addr);
    auto ip = inet_ntoa(in_addr->sin_addr);
    return ip;
}

io::address::address(void *addr, size_t size) {
    m_addr = reinterpret_cast<sockaddr*>(addr);
    m_addrlen = size;
}


io::address io::address::operator=(const address& old) {
    return io::address(new sockaddr_in(*reinterpret_cast<sockaddr_in*>(old.m_addr)), old.m_addrlen);
}

io::address::address(const io::address &&old) {
    if (m_addr) {
        delete m_addr;
    }
    m_addr = new sockaddr(*old.m_addr);
    m_addrlen = old.m_addrlen;
}

io::address::address(const io::address &old) {
    if (m_addr) {
        delete m_addr;
    }
    m_addr = new sockaddr(*old.m_addr);
    m_addrlen = old.m_addrlen;
}

