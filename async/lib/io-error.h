#pragma once

#include <cstring>
#include <optional>

namespace io {
#ifdef IO_USE_EXCEPTIONS

#define IO_RETURN_TYPE(__x__) __x__
#define IO_TRY(__x__) __x__
#define IO_VOID_RETURN(__x__) ({ __x__; return; })
#define IO_VOID_DONE IO_VOID_RETURN(0)
    typedef void void_t;
    template<typename... TArgs>
    auto handle_call(int32_t fn(TArgs...)) {
        return [fn](TArgs... args) -> IO_RETURN_TYPE(size_t) {
            auto err = fn(args...);
            if (err > 0) {
                throw std::runtime_error(std::strerror(err));
            }
            return -err;
        };
    }
#else

#define IO_RETURN_TYPE(__T__) io::option<__T__>
#define IO_TRY(__x__) ({ io::option __val = __x__; if (!__val.has_value()) { return io::error{}; } __val.value(); })
#define IO_VOID_RETURN(__x__) ({ IO_TRY(__x__); return io::void_opt(); })
#define IO_VOID_DONE IO_VOID_RETURN(0)
    struct void_opt {
    };

    struct error {
    };

    template<typename T>
    struct option {
        option(error) : m_has_value(false) {
        }
        option(T val) : m_value(val), m_has_value(true) {
        }
        option() = delete;

        const T& value() const {
            return *m_value;
        }

        bool has_value() const {
            return m_has_value;
        }

    private:
        std::optional<T> m_value{};
        bool m_has_value{false};
    };

    typedef option<void_opt> void_t;

    template<typename... TArgs>
    auto handle_call(int32_t fn(TArgs...)) {
        return [fn](TArgs... args) -> IO_RETURN_TYPE(size_t) {
            auto err = fn(args...);
            if (err > 0) {
                return io::error();
            }
            return -err;
        };
    }

#endif
}