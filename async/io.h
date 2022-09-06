#pragma once

#include "event-loop.h"

namespace io {
    uint32_t mkdir(const std::string& path, mode_t mode) {
        return event_loop::self().request([path, mode](io_uring_sqe* sqe) {
            io_uring_prep_mkdir(sqe, path.c_str(), mode);
        });
    }
}