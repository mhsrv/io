#ifndef IO_IO_H
#define IO_IO_H

#include <async.h>

#ifdef __cplusplus
extern "C" {
#endif

int32_t io_mkdir(char *path, mode_t mode);
int32_t io_write(int fd, const char *buf, uint32_t amount, size_t offset);
int32_t io_read(int fd, char *buf, uint32_t amount, size_t offset);
int32_t io_close(int fd);
int32_t io_socket(int domain, int type, int protocol, int flags);
int32_t io_accept(int fd, sockaddr *addr, socklen_t *addrlen, int flags);
int32_t io_bind(int fd, sockaddr *addr, socklen_t addrlen);
int32_t io_listen(int fd, int queue);
#ifdef __cplusplus
}

#include <span>
#include <cstring>

namespace io {
    static inline int32_t mkdir(char *path, mode_t mode) {
        return io_mkdir(path, mode);
    }

    static inline int32_t write(int fd, const char *buf, uint32_t amount, size_t offset = 0) {
        return io_write(fd, buf, amount, offset);
    }

    static inline int32_t read(int fd, char *buf, uint32_t amount, size_t offset = 0) {
        return io_read(fd, buf, amount, offset);
    }

    static inline int32_t close(int fd) {
        return io_close(fd);
    }

    static inline int32_t accept(int fd, sockaddr *addr, socklen_t *addrlen, int flags) {
        return io_accept(fd, addr, addrlen, flags);
    }

    static inline int32_t socket(int domain, int type, int protocol, int flags) {
        return io_socket(domain, type, protocol, flags);
    }

    static inline int32_t bind(int fd, sockaddr *addr, socklen_t addrlen) {
        return io_bind(fd, addr, addrlen);
    }

    static inline int32_t listen(int fd, int queue) {
        return io_listen(fd, queue);
    }

    struct address {
        sockaddr* m_addr{};
        socklen_t m_addrlen{};
        address();
        address(const address&& old);
        address(const address& old);
        address(void* addr, size_t size);
        address operator=(const address& old);
        std::string ip() const;
        uint16_t port() const;
        ~address();
        static address from(const std::string& ip, short port);
    };

    struct file {
        file(int fd, bool close = false);
        size_t read(const std::span<char>& buf, size_t offset = 0) const;
        size_t write(const std::span<char>& buf, size_t offset = 0) const;
        size_t write(const std::string_view& str, size_t offset = 0) const;
        file accept(address& address, int flags = 0) const;
        void close() const;
        void bind(const io::address& addr) const;
        void listen(int queue = 0) const;
        void set_socket_options(int level, int name, int value = 1) const;
        static file create_socket(int domain, int type, int protocol, int flags);
        static file create_tcp(int options = SO_REUSEADDR | SO_REUSEPORT);
        static file create_tcp(const std::string& ip, int port, int queue = 0, int options = SO_REUSEADDR | SO_REUSEPORT);
        ~file();
    private:
        int m_fd;
        bool m_should_close;
    };
}

#endif
#endif //IO_IO_H
