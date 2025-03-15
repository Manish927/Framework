#ifndef CONNECTION_POOL_H
#define CONNECTION_POOL_H
#include <chrono>
#include <condition_variable>
#include <deque>
#include <mutex>

#include "connection.h"

namespace common {
    struct pool_opt {
        std::size_t size = 1;

        // Max time to wait for a connection. 0ms means client waits forever.
        std::chrono::milliseconds wait_timeout{0};

        //Max lifetime time of a connection, 0 means never expire the connection
        std::chrono::milliseconds cnx_lifetime{0};

        //Max idle time of a connection, 0 means never expire the connection
        std::chrono::milliseconds cnx_ide_time{0};
    };

    class ConnectionPol {
        Connection _create(const cnx_opt& opts);
        Connection _fetch(std::unique_lock<std::mutex>& lock);
        Connection _fetch();
        void _wait_for_cnx(std::unique_lock<std::mutex>& lock);

        cnx_opt cnx_opts;
        pool_opt _pool_opts;
        std::deque<Connection> _pool;
        std::size_t _used_cnx;
        std::mutex _mutex;
        std::condition_variable _cv;
    };
}

#endif //CONNECTION_POOL_H
