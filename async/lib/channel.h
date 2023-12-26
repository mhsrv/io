#pragma once

#include <rigtorp/MPMCQueue.h>

namespace io {
    template<typename T, size_t DEPTH = 16>
    class channel {
    public:
        bool try_pop(T* &value) {
            return m_queue.try_pop(value);
        }
        void push(T &&value) {
            m_queue.push(new T(value));
        }
        void destroy() {
            while(!m_queue.empty()) {
                T *value;
                m_queue.pop(value);
                delete value;
            }
        }
    private:
        rigtorp::MPMCQueue<T*> m_queue{DEPTH};
    };
}