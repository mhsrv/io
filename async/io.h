#pragma once

#include "event-loop.h"

namespace io {
    uint32_t mkdir(const std::string& path, mode_t mode) {
        return event_loop::self().request([path, mode](io_uring_sqe* sqe) {
            io_uring_prep_mkdir(sqe, path.c_str(), mode);
        });
    }
    uint32_t write(int fd, const char *buf, uint32_t amount, size_t offset = 0) {
        return event_loop::self().request([fd, buf, amount, offset](io_uring_sqe* sqe) {
            io_uring_prep_write(sqe, fd, buf, amount, offset);
        });
    }
    uint32_t read(int fd, char *buf, uint32_t amount, size_t offset = 0) {
        return event_loop::self().request([fd, buf, amount, offset](io_uring_sqe* sqe) {
            io_uring_prep_read(sqe, fd, buf, amount, offset);
        });
    }
}