#include "connection_pool.h"

namespace common {

    ConnectionPool::ConnectionPool(const cnx_opt& opts, const pool_opt& pool_opts) :
    cnx_opts(opts), _pool_opts(pool_opts) {
        if (_pool_opts.size == 0) {
            throw std::invalid_argument("Pool size must not be empty");
        }
    }

    ConnectionPool::ConnectionPool(ConnectionPool &&that) {
        std::lock_guard<std::mutex> lock(that._mutex);
        _move(std::move(that));
    }

    ConnectionPool& ConnectionPool::operator=(ConnectionPool &&that) {
        if (this != &that) {
            std::lock(_mutex, that._mutex);
            std::lock_guard<std::mutex> lock_this(_mutex, std::adopt_lock);
            std::lock_guard<std::mutex> lock(that._mutex, std::adopt_lock);

            _move(std::move(that));
        }

        return *this;
    }

    void ConnectionPool::_move(ConnectionPool &&that) {
        cnx_opts = std::move(that.cnx_opts);
        _pool_opts = std::move(that._pool_opts);
        _pool = std::move(that._pool);
        _used_cnx = that._used_cnx;
    }


    Connection ConnectionPool::fetch() {
        std::unique_lock<std::mutex> lock(_mutex);

        auto connection = _fetch(lock);

        auto cnx_lifetime = _pool_opts.cnx_lifetime;
        auto cnx_idle_time = _pool_opts.cnx_ide_time
    }
}