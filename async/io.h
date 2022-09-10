#pragma once

#include "event-loop.h"
#include <span>
#include <cstring>

namespace io {
    int32_t mkdir(const std::string& path, mode_t mode) {
        return event_loop::self()->request([path, mode](io_uring_sqe* sqe) {
            io_uring_prep_mkdir(sqe, path.c_str(), mode);
        });
    }
    int32_t write(int fd, const char *buf, uint32_t amount, size_t offset = 0) {
        return event_loop::self()->request([fd, buf, amount, offset](io_uring_sqe* sqe) {
            io_uring_prep_write(sqe, fd, buf, amount, offset);
        });
    }
    int32_t read(int fd, char *buf, uint32_t amount, size_t offset = 0) {
        return event_loop::self()->request([fd, buf, amount, offset](io_uring_sqe* sqe) {
            io_uring_prep_read(sqe, fd, buf, amount, offset);
        });
    }
    int32_t close(int fd) {
        return event_loop::self()->request([fd](io_uring_sqe* sqe) {
            io_uring_prep_close(sqe, fd);
        });
    }

    struct file {
        file(int fd, bool close = false) {
            m_fd = fd;
            m_should_close = close;
        }

        size_t read(const std::span<char>& buf, size_t offset = 0) {
            auto err = io::read(m_fd, buf.data(), buf.size(), offset);
            if (err > 0) {
                throw std::runtime_error(std::strerror(err));
            }
            return -err;
        }

        size_t write(const std::span<char>& buf, size_t offset = 0) {
            auto err = io::write(m_fd, buf.data(), buf.size(), offset);
            if (err > 0) {
                throw std::runtime_error(std::strerror(err));
            }
            return -err;
        }

        ~file() {
            if (m_should_close) {
                io::close(m_fd);
            }
        }
    private:
        int m_fd;
        bool m_should_close;
    };
}