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

int32_t io_openat(int dfd, const char *path, int flags, mode_t mode) {
    return async::request([dfd, path, flags, mode](io_uring_sqe *sqe) {
        io_uring_prep_openat(sqe, dfd, path, flags, mode);
    });
}

int32_t io_cancel(task_t *task, int flags) {
    return async::request([flags, task](io_uring_sqe *sqe) {
        io_uring_prep_cancel(sqe, task, flags);
    });
}

int32_t io_cancel_fd(int fd, int flags) {
    return async::request([fd, flags](io_uring_sqe *sqe) {
        io_uring_prep_cancel_fd(sqe, fd, flags);
    });
}

int32_t io_connect(int fd, const sockaddr *addr, socklen_t addrlen) {
    return async::request([fd, addr, addrlen](io_uring_sqe *sqe) {
        io_uring_prep_connect(sqe, fd, addr, addrlen);
    });
}

int32_t io_epoll_ctl(int epfd, int fd, int op, struct epoll_event *ev) {
    return async::request([epfd, fd, op, ev](io_uring_sqe *sqe) {
        io_uring_prep_epoll_ctl(sqe, epfd, fd, op, ev);
    });
}

int32_t io_fadvise(int fd, size_t offset, off_t len, int advice) {
    return async::request([fd, offset, len, advice](io_uring_sqe *sqe) {
        io_uring_prep_fadvise(sqe, fd, offset, len, advice);
    });
}

int32_t io_fallocate( int fd, int mode, off_t offset, off_t len) {
    return async::request([fd, mode, offset, len](io_uring_sqe *sqe) {
        io_uring_prep_fallocate(sqe, fd, mode, offset, len);
    });
}

int32_t io_fgetxattr(int fd, const char *name, const char *value, size_t len) {
    return async::request([fd, name, value, len](io_uring_sqe *sqe) {
        io_uring_prep_fgetxattr(sqe, fd, name, value, len);
    });
}
int32_t io_fsetxattr(int fd, const char *name, const char *value, int flags, size_t len) {
    return async::request([fd, name, value, flags, len](io_uring_sqe *sqe) {
        io_uring_prep_fsetxattr(sqe, fd, name, value, flags, len);
    });
}

int32_t io_files_update(int *fds, unsigned int nr_fds, int offset) {
    return async::request([fds, nr_fds, offset](io_uring_sqe *sqe) {
        io_uring_prep_files_update(sqe, fds, nr_fds, offset);
    });
}

int32_t io_fsync(int fd, unsigned int fsync_flags) {
    return async::request([fd, fsync_flags](io_uring_sqe *sqe) {
        io_uring_prep_fsync(sqe, fd, fsync_flags);
    });
}

int32_t io_link(const char *oldpath, const char *newpath, int flags) {
    return async::request([oldpath, newpath, flags](io_uring_sqe *sqe) {
        io_uring_prep_link(sqe, oldpath, newpath, flags);
    });
}

int32_t io_linkat(int olddfd, const char *oldpath, int newdfd, const char *newpath, int flags) {
    return async::request([olddfd, oldpath, newdfd, newpath, flags](io_uring_sqe *sqe) {
        io_uring_prep_linkat(sqe, olddfd, oldpath, newdfd, newpath, flags);
    });
}

int32_t io_madvise(void *addr, off_t length, int advice) {
    return async::request([addr, length, advice](io_uring_sqe *sqe) {
        io_uring_prep_madvise(sqe, addr, length, advice);
    });
}


int32_t io_mkdirat(int dfd, const char *path, mode_t mode) {
    return async::request([dfd, path, mode](io_uring_sqe *sqe) {
        io_uring_prep_mkdirat(sqe, dfd, path, mode);
    });
}

int32_t io_msg_ring(int fd, unsigned int len, __u64 data, unsigned int flags) {
    return async::request([fd, len, data, flags](io_uring_sqe *sqe) {
        io_uring_prep_msg_ring(sqe, fd, len, data, flags);
    });
}

int32_t io_multishot_accept(int fd, struct sockaddr *addr, socklen_t *addrlen, int flags) {
    return async::request([fd, addr, addrlen, flags](io_uring_sqe *sqe) {
        io_uring_prep_multishot_accept(sqe, fd, addr, addrlen, flags);
    });
}

int32_t io_nop() {
    return async::request([](io_uring_sqe *sqe) {
        io_uring_prep_nop(sqe);
    });
}

int32_t io_setxattr(const char *name, const char *value, const char *path, int flags, size_t len) {
    return async::request([name, value, path, flags, len](io_uring_sqe *sqe) {
        io_uring_prep_setxattr(sqe, name, value, path, flags, len);
    });
}

int32_t io_getxattr(const char *name, const char *value, const char *path, int flags, size_t len) {
    return async::request([name, value, path, flags, len](io_uring_sqe *sqe) {
        io_uring_prep_setxattr(sqe, name, value, path, flags, len);
    });
}


int32_t io_poll_add(int fd, unsigned int poll_mask) {
    return async::request([fd, poll_mask](io_uring_sqe *sqe) {
        io_uring_prep_poll_add(sqe, fd, poll_mask);
    });
}

int32_t io_poll_remove(task_t *task) {
    return async::request([task](io_uring_sqe *sqe) {
        io_uring_prep_poll_remove(sqe, reinterpret_cast<__u64>(task));
    });
}

int32_t io_poll_multishot(int fd, unsigned int poll_mask) {
    return async::request([fd, poll_mask](io_uring_sqe *sqe) {
        io_uring_prep_poll_multishot(sqe, fd, poll_mask);
    });
}

int32_t io_poll_update(task_t *task, unsigned int poll_mask, unsigned int flags) {
    return async::request([task, poll_mask, flags](io_uring_sqe *sqe) {
        io_uring_prep_poll_update(sqe, reinterpret_cast<__u64>(task), reinterpret_cast<__u64>(task), poll_mask, flags);
    });
}

int32_t io_provide_buffers(void *addr, int len, int nr, int bgid, int bid) {
    return async::request([addr, len, nr, bgid, bid](io_uring_sqe *sqe) {
        io_uring_prep_provide_buffers(sqe, addr, len, nr, bgid, bid);
    });
}

int32_t io_remove_buffers(int nr, int bgid) {
    return async::request([nr, bgid](io_uring_sqe *sqe) {
        io_uring_prep_remove_buffers(sqe, nr, bgid);
    });
}

int32_t io_recv(int sockfd, void *buf, size_t len, int flags) {
    return async::request([sockfd, buf, len, flags](io_uring_sqe *sqe) {
        io_uring_prep_recv(sqe, sockfd, buf, len, flags);
    });
}

int32_t io_recvmsg(int fd, struct msghdr *msg, unsigned int flags) {
    return async::request([fd, msg, flags](io_uring_sqe *sqe) {
        io_uring_prep_recvmsg(sqe, fd, msg, flags);
    });
}

int32_t io_rename(const char *oldpath, const char *newpath) {
    return async::request([oldpath, newpath](io_uring_sqe *sqe) {
        io_uring_prep_rename(sqe, oldpath, newpath);
    });
}

int32_t io_renameat(int olddfd, const char *oldpath, int newdfd, const char *newpath, int flags) {
    return async::request([olddfd, oldpath, newdfd, newpath, flags](io_uring_sqe *sqe) {
        io_uring_prep_renameat(sqe, olddfd, oldpath, newdfd, newpath, flags);
    });
}

int32_t io_send(int sockfd, const void *buf, size_t len, int flags) {
    return async::request([sockfd, buf, len, flags](io_uring_sqe *sqe) {
        io_uring_prep_send(sqe, sockfd, buf, len, flags);
    });
}

int32_t io_sendmsg(int fd, const struct msghdr *msg, unsigned int flags) {
    return async::request([fd, msg, flags](io_uring_sqe *sqe) {
        io_uring_prep_sendmsg(sqe, fd, msg, flags);
    });
}

int32_t io_shutdown(int fd, int how) {
    return async::request([fd, how](io_uring_sqe *sqe) {
        io_uring_prep_shutdown(sqe, fd, how);
    });
}

int32_t io_splice(int fd_in, int64_t off_in, int fd_out, int64_t off_out, unsigned int nbytes, unsigned int splice_flags) {
    return async::request([fd_in, off_in, fd_out, off_out, nbytes, splice_flags](io_uring_sqe *sqe) {
        io_uring_prep_splice(sqe, fd_in, off_in, fd_out, off_out, nbytes, splice_flags);
    });
}

int32_t io_statx(int dfd, const char *path, int flags, unsigned int mask, struct statx *statxbuf) {
    return async::request([dfd, path, flags, mask, statxbuf](io_uring_sqe *sqe) {
        io_uring_prep_statx(sqe, dfd, path, flags, mask, statxbuf);
    });
}

int32_t io_symlink(const char *target, const char *linkpath) {
    return async::request([target, linkpath](io_uring_sqe *sqe) {
        io_uring_prep_symlink(sqe, target, linkpath);
    });
}

int32_t io_symlinkat(const char *target, int newdirfd, const char *linkpath) {
    return async::request([target, newdirfd, linkpath](io_uring_sqe *sqe) {
        io_uring_prep_symlinkat(sqe, target, newdirfd, linkpath);
    });
}

int32_t io_sync_file_range(int fd, unsigned int len, size_t offset, int flags) {
    return async::request([fd, len, offset, flags](io_uring_sqe *sqe) {
        io_uring_prep_sync_file_range(sqe, fd, len, offset, flags);
    });
}

int32_t io_timeout(struct __kernel_timespec *ts, unsigned int count, unsigned int flags) {
    return async::request([ts, count, flags](io_uring_sqe *sqe) {
        io_uring_prep_timeout(sqe, ts, count, flags);
    });
}

int32_t io_timeout_remove(task_t *task, unsigned int flags) {
    return async::request([task, flags](io_uring_sqe *sqe) {
        io_uring_prep_timeout_remove(sqe, reinterpret_cast<__u64>(task), flags);
    });
}

int32_t io_timeout_update(task_t *task,  __kernel_timespec *ts, unsigned int flags) {
    return async::request([task, flags, ts](io_uring_sqe *sqe) {
        io_uring_prep_timeout_update(sqe, ts, reinterpret_cast<__u64>(task), flags);
    });
}

int32_t io_unlink(const char *path, int flags) {
    return async::request([path, flags](io_uring_sqe *sqe) {
        io_uring_prep_unlink(sqe, path, flags);
    });
}

int32_t io_unlinkat(int dfd, const char *path, int flags) {
    return async::request([dfd, path, flags](io_uring_sqe *sqe) {
        io_uring_prep_unlinkat(sqe, dfd, path, flags);
    });
}


int32_t io_rw(int op, int fd, const void *addr, unsigned int len, size_t offset) {
    return async::request([op, fd, addr, len, offset](io_uring_sqe *sqe) {
        io_uring_prep_rw(op, sqe, fd, addr, len, offset);
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

}
io::file::file(int fd) : rw_stream<stream>(fd) { }

void io::file::sync(int flags) const {
    auto err = io::fsync(m_fd, flags);
    if (err > 0) {
        throw std::runtime_error(std::strerror(err));
    }
}

void io::file::set_attribute(const std::string &name, const std::string &value, int flags) const {
    auto err = io::fsetxattr(m_fd, name.c_str(), value.c_str(), flags, value.size());
    if (err > 0) {
        throw std::runtime_error(std::strerror(err));
    }
}

void io::file::get_attribute(const std::string &name, std::span<char> value) const {
    auto err = io::fgetxattr(m_fd, name.c_str(), value.data(), value.size());
    if (err > 0) {
        throw std::runtime_error(std::strerror(err));
    }
}

void io::file::advise(size_t offset, off_t len, int advice) const {
    auto err = io::fadvise(m_fd, offset, len, advice);
    if (err > 0) {
        throw std::runtime_error(std::strerror(err));
    }
}

void io::file::allocate(off_t offset, off_t len, int mode) const {
    auto err = io::fallocate(m_fd, mode, offset, len);
    if (err > 0) {
        throw std::runtime_error(std::strerror(err));
    }
}

void io::file::sync(size_t offset, unsigned int len, int flags) const {
    auto err = io::sync_file_range(m_fd, len, offset, flags);
    if (err > 0) {
        throw std::runtime_error(std::strerror(err));
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

io::directory::directory(int dfd) : stream(dfd) { }

io::file io::directory::open(const std::string &path, mode_t mode, int flags) const {
    auto err = io::openat(m_fd, path.c_str(), flags, mode);
    if (err > 0) {
        throw std::runtime_error(std::strerror(err));
    }
    return {-err};
}


void io::directory::link(const std::string &oldpath, const std::string &newpath, int flags) const {
    auto err = io::linkat(m_fd, oldpath.c_str(), m_fd, newpath.c_str(), flags);
    if (err > 0) {
        throw std::runtime_error(std::strerror(err));
    }
}

void io::directory::link(const std::string &oldpath, const relative_path& newpath, int flags) const {
    auto err = io::linkat(m_fd, oldpath.c_str(), newpath.directory().m_fd, newpath.path().c_str(), flags);
    if (err > 0) {
        throw std::runtime_error(std::strerror(err));
    }
}

io::relative_path io::directory::relative(const std::string &path) const {
    return {m_fd, path};
}

void io::directory::mkdir(const std::string &path, mode_t mode) const {
    auto err = io::mkdirat(m_fd, path.c_str(), mode);
    if (err > 0) {
        throw std::runtime_error(std::strerror(err));
    }
}

void io::directory::rename(const std::string &oldpath, const std::string &newpath, int flags) const {
    auto err = io::renameat(m_fd, oldpath.c_str(), m_fd, newpath.c_str(), flags);
    if (err > 0) {
        throw std::runtime_error(std::strerror(err));
    }
}

void io::directory::rename(const std::string &oldpath, const io::relative_path &newpath, int flags) const {
    auto err = io::renameat(m_fd, oldpath.c_str(), newpath.directory().m_fd, newpath.path().c_str(), flags);
    if (err > 0) {
        throw std::runtime_error(std::strerror(err));
    }
}

void io::directory::symlink(const std::string &target, const std::string &path) const {
    auto err =io::symlinkat(target.c_str(), m_fd, path.c_str());
    if (err > 0) {
        throw std::runtime_error(std::strerror(err));
    }
}

void io::directory::unlink(const std::string &path, int flags) const {
    auto err = io::unlinkat(m_fd, path.c_str(), flags);
    if (err > 0) {
        throw std::runtime_error(std::strerror(err));
    }
}

struct statx io::directory::stat(const std::string &path, int mask, int flags) const {
    struct statx buffer{};
    auto err = io::statx(m_fd, path.c_str(), flags, mask, &buffer);
    if (err > 0) {
        throw std::runtime_error(std::strerror(err));
    }
    return buffer;
}

io::relative_path::relative_path(const io::directory &directory, const std::string &path)
:  m_directory(directory), m_path(path) {
}

const io::directory &io::relative_path::directory() const {
    return m_directory;
}

const std::string &io::relative_path::path() const {
    return m_path;
}

void io::stream::close() const {
    auto err = io::close(m_fd);
    if (err > 0) {
        throw std::runtime_error(std::strerror(err));
    }
}

io::stream::stream(int fd) : m_fd(fd) {

}


io::readable_stream::readable_stream(int fd) : stream(fd) {

}

size_t io::readable_stream::read(const std::span<char> &buf, size_t offset) const {
    auto err = io::read(m_fd, buf.data(), buf.size(), offset);
    if (err > 0) {
        throw std::runtime_error(std::strerror(err));
    }
    return -err;
}

io::readable_stream::readable_stream(const io::stream &base) : stream(base) { }

io::writable_stream::writable_stream(int fd) : stream(fd) { }

size_t io::writable_stream::write(const std::span<char> &buf, size_t offset) const {
    auto err = io::write(m_fd, buf.data(), buf.size(), offset);
    if (err > 0) {
        throw std::runtime_error(std::strerror(err));
    }
    return -err;
}

size_t io::writable_stream::write(const std::string_view &buf, size_t offset) const {
    auto err = io::write(m_fd, buf.data(), buf.size(), offset);
    if (err > 0) {
        throw std::runtime_error(std::strerror(err));
    }
    return -err;
}

io::writable_stream::writable_stream(const io::stream &base) : stream(base) { }

io::network_stream::network_stream(int sockfd) : stream(sockfd) { }

io::network_stream io::network_stream::create_socket(int domain, int type, int protocol, int flags) {
    auto err = io::socket(domain, type, protocol, flags);
    if (err > 0) {
        throw std::runtime_error(std::strerror(err));
    }
    return {-err};
}

void io::network_stream::set_socket_options(int level, int name, int value) const {
    int opt = value;
    if (setsockopt(m_fd, level, name, &opt, sizeof(opt)) < 0) {
        throw std::runtime_error(std::strerror(errno));
    }
}

io::network_stream::network_stream(const io::stream &base) : stream(base) { }

void io::network_stream::shutdown(int how) const {
    auto err = io::shutdown(m_fd, how);
    if (err > 0) {
        throw std::runtime_error(std::strerror(err));
    }
}

io::client::client(int sockfd) : rw_stream<network_stream>(sockfd) {

}


size_t io::client::send(const std::span<char> &buf, int flags) const {
    auto err = io::send(m_fd, buf.data(), buf.size(), flags);
    if (err > 0) {
        throw std::runtime_error(std::strerror(err));
    }
    return -err;
}

size_t io::client::send(const std::string_view &str, int flags) const {
    auto err = io::send(m_fd, str.data(), str.size(), flags);
    if (err > 0) {
        throw std::runtime_error(std::strerror(err));
    }
    return -err;
}

size_t io::client::recv(std::span<char> &buf, int flags) const {
    auto err = io::recv(m_fd, buf.data(), buf.size(), flags);
    if (err > 0) {
        throw std::runtime_error(std::strerror(err));
    }
    return -err;
}

io::server::server(int sockfd) : network_stream(sockfd) { }

io::client io::server::accept(io::address &address, bool multishot, int flags) const {
    address = {};
    int32_t err;
    if (multishot) {
        err = io::multishot_accept(m_fd, (sockaddr *)address.m_addr, &address.m_addrlen, flags);
    } else {
        err = io::accept(m_fd, (sockaddr *) address.m_addr, &address.m_addrlen, flags);
    }
    if (err > 0) {
        throw std::runtime_error(std::strerror(err));
    }
    return {-err};
}

void io::server::bind(const io::address &addr) const {
    auto err = io::bind(m_fd, addr.m_addr, addr.m_addrlen);
    if (err) {
        throw std::runtime_error(std::strerror(err));
    }
}

void io::server::listen(int queue) const {
    auto err = io::listen(m_fd, queue);
    if (err) {
        throw std::runtime_error(std::strerror(err));
    }
}

io::server io::server::create_tcp(int options) {
    auto socket = create_socket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0);
    socket.set_socket_options(SOL_SOCKET, options);
    return io::server(socket);
}

io::server io::server::create_tcp(const std::string &ip, int port, int queue, int options) {
    auto socket = io::server::create_tcp(options);
    socket.bind(io::address::from(ip, port));
    socket.listen(queue);
    return socket;
}

io::server::server(const io::stream &base) : network_stream(base) { }


void io::dir::link(const std::string &oldpath, const std::string &newpath, int flags) {
    auto err = io::link(oldpath.c_str(), newpath.c_str(), flags);
    if (err > 0) {
        throw std::runtime_error(std::strerror(err));
    }
}

void io::dir::mkdir(const std::string &path, mode_t mode) {
    auto err = io::mkdir(const_cast<char *>(path.c_str()), mode);
    if (err > 0) {
        throw std::runtime_error(std::strerror(err));
    }
}

void io::dir::set_attribute(const std::string &path, const std::string &name, const std::string &value, int flags) {
    auto err = io::setxattr(path.c_str(), name.c_str(), value.c_str(), flags, value.size());
    if (err > 0) {
        throw std::runtime_error(std::strerror(err));
    }
}

void io::dir::get_attribute(const std::string &path, const std::string &name, std::span<char> value, int flags) {
    auto err = io::setxattr(path.c_str(), name.c_str(), value.data(), flags, value.size());
    if (err > 0) {
        throw std::runtime_error(std::strerror(err));
    }
}

void io::dir::rename(const std::string &oldpath, const std::string &newpath) {
    auto err = io::rename(oldpath.c_str(), newpath.c_str());
    if (err > 0) {
        throw std::runtime_error(std::strerror(err));
    }
}

void io::dir::symlink(const std::string &target, const std::string &path) {
    auto err = io::symlink(target.c_str(), path.c_str());
    if (err > 0) {
        throw std::runtime_error(std::strerror(err));
    }
}

void io::dir::unlink(const std::string &path, int flags) {
    auto err = io::unlink(path.c_str(), flags);
    if (err > 0) {
        throw std::runtime_error(std::strerror(err));
    }

}
