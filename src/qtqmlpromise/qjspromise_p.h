#ifndef QTQMLPROMISE_QJSPROMISE_P_H
#define QTQMLPROMISE_QJSPROMISE_P_H

// QtQmlPromise
#include "qjspromise.h"

// QtPromise
#include <QtPromise>

// Qt
#include <QJSValue>

namespace QtPromisePrivate {

template <>
struct PromiseFulfill<QJSValue>
{
    static void call(
        const QJSValue& value,
        const QTPROMISE_PREPEND_NAMESPACE(QPromiseResolve<QJSValue>)& resolve,
        const QTPROMISE_PREPEND_NAMESPACE(QPromiseReject<QJSValue>)& reject)
    {
        QTPROMISE_USE_NAMESPACE

        if (value.isObject()) {
            const QVariant variant = value.toVariant();
            if (variant.userType() == qMetaTypeId<QJSPromise>()) {
                const auto promise = variant.value<QJSPromise>().m_promise;
                PromiseFulfill<QPromise<QJSValue> >::call(promise, resolve, reject);
                return;
            }
        }

        if (value.isError()) {
            reject(value);
        } else {
            resolve(value);
        }
    }
};

class JSPromiseResolver
{
    Q_GADGET

public:
    JSPromiseResolver() {}
    JSPromiseResolver(
        const QTPROMISE_PREPEND_NAMESPACE(QPromiseResolve<QJSValue>)& resolve,
        const QTPROMISE_PREPEND_NAMESPACE(QPromiseReject<QJSValue>)& reject)
        : m_d(new Data{resolve, reject})
    { }

    Q_INVOKABLE void resolve(QJSValue value = QJSValue()) { m_d->resolve(std::move(value)); }
    Q_INVOKABLE void reject(QJSValue error = QJSValue()) { m_d->reject(std::move(error)); }

private:
    struct Data
    {
        QTPROMISE_PREPEND_NAMESPACE(QPromiseResolve<QJSValue>) resolve;
        QTPROMISE_PREPEND_NAMESPACE(QPromiseReject<QJSValue>) reject;
    }; // struct Data

    QSharedPointer<Data> m_d;
};

} // namespace QtPromisePrivate

#endif // ifndef QTQMLPROMISE_QJSPROMISE_P_H

