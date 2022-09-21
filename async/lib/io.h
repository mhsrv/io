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


    struct file {
        file(int fd, bool close = false);
        size_t read(const std::span<char>& buf, size_t offset = 0);
        size_t write(const std::span<char>& buf, size_t offset = 0);
        size_t write(const std::string_view& str, size_t offset = 0);
        ~file();
    private:
        int m_fd;
        bool m_should_close;
    };
}

#endif
#endif //IO_IO_H
