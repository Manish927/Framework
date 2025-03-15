#include "connection_pool.h"

#include <cassert>

namespace common {

    ConnectionPool::ConnectionPool(const cnx_opt& opts, const pool_opt& pool_opts) :
    cnx_opts(opts), _pool_opts(pool_opts) {
        if (_pool_opts.size == 0) {
            throw std::invalid_argument("Pool size must not be empty");
        }
    }

    ConnectionPool::ConnectionPool(ConnectionPool &&that) {
        std::scoped_lock lock{that._mutex};
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
        auto cnx_idle_time = _pool_opts.cnx_ide_time;

        try {
            connection = _create(cnx_opts);
        } catch (const std::exception& e) {
            release(std::move(connection));
            throw;
        }

        return connection;
    }

    Connection ConnectionPool::_fetch(std::unique_lock<std::mutex>& lock) {
        if (_pool.empty()) {
            if (_used_cnx == _pool_opts.size) {
                _wait_for_cnx(lock);
            }
            else {
                ++_used_cnx;
                return Connection(cnx_opt, Connection::Dummy{});
            }
        }

        return _fetch();
    }

    Connection ConnectionPool::_fetch() {
        assert(!_pool.empty());

        auto connection = std::move(_pool.front());
        _pool.pop_front();
        return connection;
    }

    void ConnectionPool::release(Connection cnx) {
        {
            std::scoped_lock lock{_mutex};
            _pool.push_back(cnx);
        }
        _cv.notify_one();
    }

    Connection ConnectionPool::create() {
        std::scoped_lock lock{_mutex};

        return Connection(cnx_opts);
    }

}
