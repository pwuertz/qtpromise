// QtQmlPromise
#include "qjspromise.h"
#include "qjspromise_p.h"

// Qt
#include <QJSValueIterator>
#include <QJSEngine>

QTPROMISE_USE_NAMESPACE

QJSPromise::QJSPromise()
    : m_promise(QPromise<QJSValue>::resolve(QJSValue()))
{ }

QJSPromise::QJSPromise(QPromise<QJSValue>&& promise)
    : m_promise(std::move(promise))
{
}

QJSPromise::QJSPromise(QJSEngine* engine, QJSValue resolver)
    : m_promise([=](
        const QPromiseResolve<QJSValue>& resolve,
        const QPromiseReject<QJSValue>& reject) mutable {

            // resolver is part of the Promise wrapper in qtqmlpromise.js
            Q_ASSERT(resolver.isCallable());
            Q_ASSERT(engine);

            auto proxy = QtPromisePrivate::JSPromiseResolver(resolve, reject);
            auto ret = resolver.call(QJSValueList() << engine->toScriptValue(proxy));
            if (ret.isError()) {
                throw ret;
            }
        })
{ }

QJSPromise QJSPromise::then(QJSValue fulfilled, QJSValue rejected) const
{
    const bool fulfillable = fulfilled.isCallable();
    const bool rejectable = rejected.isCallable();
    if (!fulfillable && !rejectable) {
        // WARNING
        return *this;
    }

    auto _rejected = [=](const QJSValue& err) mutable {
        return rejected.call(QJSValueList() << err);
    };

    if (!fulfillable) {
        return m_promise.then(nullptr, _rejected);
    }

    auto _fulfilled = [=](const QJSValue& res) mutable {
        return fulfilled.call(QJSValueList() << res);
    };

    if (!rejectable) {
        return m_promise.then(_fulfilled);
    }

    return m_promise.then(_fulfilled, _rejected);
}

QJSPromise QJSPromise::fail(QJSValue handler) const
{
    return then(QJSValue(), handler);
}

QJSPromise QJSPromise::finally(QJSValue handler) const
{
    return m_promise.finally([=]() mutable {
        return handler.call();
    });
}

QJSPromise QJSPromise::delay(int msec) const
{
    return m_promise.delay(msec);
}

QJSPromise QJSPromise::wait() const
{
    return m_promise.wait();
}

QJSPromise QJSPromise::resolve(QJSValue&& value)
{
    return QPromise<QJSValue>::resolve(std::forward<QJSValue>(value));
}

QJSPromise QJSPromise::reject(QJSValue&& error)
{
    return QPromise<QJSValue>::reject(std::forward<QJSValue>(error));
}

QJSPromise QJSPromise::all(QJSValue&& input)
{
    if (!input.isArray()) {
        // TODO TYPEERROR!
        return QPromise<QJSValue>::reject("foobar");
    }

    Q_ASSERT(input.hasProperty("length"));
    const int count = input.property("length").toInt();
    if (!count) {
        return QPromise<QJSValue>::resolve(QJSValue(input));
    }

    QVector<QPromise<QJSValue> > promises;
    for (int i = 0; i < count; ++i) {
        QJSValue value = input.property(i);
        const QVariant variant = value.toVariant();
        if (variant.userType() == qMetaTypeId<QJSPromise>()) {
            //promises[i] = variant.value<QJSPromise>().m_promise;
        } else {
            //promises[i] = QPromise<QJSValue>::resolve(std::move(value));
        }
    }

    qDebug() << "TODO!!!!!! replace QVector API by QList";

    return QPromise<QJSValue>::all(promises)
        .then([](const QVector<QJSValue>& results) {
            return QJSValue();
        });
}
