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

    class ConnectionPool {
        Connection _create(const cnx_opt& opts);
        Connection _fetch(std::unique_lock<std::mutex>& lock);
        Connection _fetch();
        void _wait_for_cnx(std::unique_lock<std::mutex>& lock);
        void _move(ConnectionPool &&that);

        cnx_opt cnx_opts;
        pool_opt _pool_opts;
        std::deque<Connection> _pool;
        std::size_t _used_cnx = 0;
        std::mutex _mutex;
        std::condition_variable _cv;

    public:
        ConnectionPool(const cnx_opt& opts, const pool_opt& pool_opts);
        ConnectionPool() = default;
        ConnectionPool(ConnectionPool&& that);
        ConnectionPool& operator=(ConnectionPool&& that);

        ConnectionPool(const ConnectionPool& that) = delete;
        ConnectionPool& operator=(const ConnectionPool& that) = delete;

        ~ConnectionPool() = default;

        //Fetch a connection from pool
        Connection fetch();

        cnx_opt connection_opts();
        void release (Connection cnx);
        Connection create();
    };
}

#endif //CONNECTION_POOL_H
