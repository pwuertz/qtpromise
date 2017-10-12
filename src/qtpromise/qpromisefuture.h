#ifndef QTPROMISE_QPROMISEFUTURE_P_H
#define QTPROMISE_QPROMISEFUTURE_P_H

// QtPromise
#include "qpromiseglobal.h"

// Qt
#include <QFutureWatcher>
#include <QFuture>

QTPROMISE_BEGIN_NAMESPACE

class QPromiseCanceledException : public QException
{
public:
    void raise() const Q_DECL_OVERRIDE { throw *this; }
    QPromiseCanceledException* clone() const Q_DECL_OVERRIDE
    {
        return new QPromiseCanceledException(*this);
    }
};

QTPROMISE_END_NAMESPACE

namespace QtPromisePrivate {

template <typename T>
struct PromiseDeduce<QFuture<T> >
    : public PromiseDeduce<T>
{ };

template <typename T>
struct PromiseFulfill<QFuture<T> >
{
    static void call(
        const QFuture<T>& future,
        const QTPROMISE_PREPEND_NAMESPACE(QPromiseResolve<T>)& resolve,
        const QTPROMISE_PREPEND_NAMESPACE(QPromiseReject<T>)& reject)
    {
        using Watcher = QFutureWatcher<T>;

        Watcher* watcher = new Watcher();
        QObject::connect(watcher, &Watcher::finished, [=]() mutable {
            try {
                if (watcher->isCanceled()) {
                    // A QFuture is canceled if cancel() has been explicitly called OR if an
                    // exception has been thrown from the associated thread. Trying to call
                    // result() in the first case causes a "read access violation", so let's
                    // rethrown potential exceptions using waitForFinished() and thus detect
                    // if the future has been canceled by the user or an exception.
                    watcher->waitForFinished();
                    reject(QTPROMISE_PREPEND_NAMESPACE(QPromiseCanceledException()));
                } else {
                    PromiseFulfill<T>::call(watcher->result(), resolve, reject);
                }
            } catch (...) {
                reject(std::current_exception());
            }

            watcher->deleteLater();
        });

        watcher->setFuture(future);
    }
};

template <>
struct PromiseFulfill<QFuture<void> >
{
    static void call(
        const QFuture<void>& future,
        const QTPROMISE_PREPEND_NAMESPACE(QPromiseResolve<void>)& resolve,
        const QTPROMISE_PREPEND_NAMESPACE(QPromiseReject<void>)& reject)
    {
        using Watcher = QFutureWatcher<void>;

        Watcher* watcher = new Watcher();
        QObject::connect(watcher, &Watcher::finished, [=]() mutable {
            try {
                if (watcher->isCanceled()) {
                    // let's rethrown potential exception
                    watcher->waitForFinished();
                    reject(QTPROMISE_PREPEND_NAMESPACE(QPromiseCanceledException()));
                } else {
                    resolve();
                }
            } catch (...) {
                reject(std::current_exception());
            }

            watcher->deleteLater();
        });

        watcher->setFuture(future);
    }
};

} // namespace QtPromisePrivate

#endif // QTPROMISE_QPROMISEFUTURE_P_H
