#ifndef QTPROMISE_QPROMISEHELPERS_H
#define QTPROMISE_QPROMISEHELPERS_H

// QtPromise
#include "qpromise_p.h"
#include <memory>

namespace QtPromise {

template <typename T>
static inline typename QtPromisePrivate::PromiseDeduce<T>::Type qPromise(T&& value)
{
    using namespace QtPromisePrivate;
    using Promise = typename PromiseDeduce<T>::Type;
    return Promise([&](
        const QPromiseResolve<typename Promise::Type>& resolve,
        const QPromiseReject<typename Promise::Type>& reject) {
        PromiseFulfill<T>::call(std::forward<T>(value), resolve, reject);
    });
}

static inline QPromise<void> qPromise()
{
    return QPromise<void>([](
        const QPromiseResolve<void>& resolve) {
        resolve();
    });
}

template <typename T, template <typename, typename...> class Sequence = QVector, typename ...Args>
static inline QPromise<QVector<T> > qPromiseAll(const Sequence<QPromise<T>, Args...>& promises)
{
    return QPromise<T>::all(promises);
}

template <template <typename, typename...> class Sequence = QVector, typename ...Args>
static inline QPromise<void> qPromiseAll(const Sequence<QPromise<void>, Args...>& promises)
{
    return QPromise<void>::all(promises);
}

class ConnectionGuard {
public:
    static std::shared_ptr<ConnectionGuard> create() {
        return std::shared_ptr<ConnectionGuard>(new ConnectionGuard);
    }
    void operator<<(QMetaObject::Connection &&other) {
        connections.emplace_back(std::move(other));
    }
    void disconnect() {
        for (const auto& connection: connections)
            QObject::disconnect(connection);
        connections.clear();
    }
    ~ConnectionGuard() {
        disconnect();
    }
protected:
    std::vector<QMetaObject::Connection> connections;
    ConnectionGuard() = default;
    ConnectionGuard(const ConnectionGuard&) = delete;
    void operator=(const ConnectionGuard&) = delete;
};

} // namespace QtPromise

#endif // QTPROMISE_QPROMISEHELPERS_H
