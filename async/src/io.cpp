#include <io.h>

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
    auto err = io::accept(m_fd, (struct sockaddr *)&address.addr, &address.addrlen, flags);
    if (err > 0) {
        throw std::runtime_error(std::strerror(err));
    }
    return {-err};
}

void io::file::close() const {
    io::close(m_fd);
}
