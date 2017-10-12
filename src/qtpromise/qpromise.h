#ifndef QTPROMISE_QPROMISE_H
#define QTPROMISE_QPROMISE_H

// QtPromise
#include "qpromise_p.h"
#include "qpromiseglobal.h"

// Qt
#include <QExplicitlySharedDataPointer>

QTPROMISE_BEGIN_NAMESPACE

template <typename T>
class QPromiseBase
{
public:
    using Type = T;

    QPromiseBase(const QPromiseBase<T>& other): m_d(other.m_d) {}
    QPromiseBase(const QPromise<T>& other): m_d(other.m_d) {}
    QPromiseBase(QPromiseBase<T>&& other) { swap(other); }

    template <typename F, typename std::enable_if<QtPromisePrivate::ArgsOf<F>::count == 1, int>::type = 0>
    inline QPromiseBase(F resolver);

    template <typename F, typename std::enable_if<QtPromisePrivate::ArgsOf<F>::count != 1, int>::type = 0>
    inline QPromiseBase(F resolver);

    virtual ~QPromiseBase() { }

    bool isFulfilled() const { return m_d->isFulfilled(); }
    bool isRejected() const { return m_d->isRejected(); }
    bool isPending() const { return m_d->isPending(); }

    template <typename TFulfilled, typename TRejected>
    inline typename QtPromisePrivate::PromiseHandler<T, TFulfilled>::Promise
    then(const TFulfilled& fulfilled, const TRejected& rejected) const;

    template <typename TFulfilled>
    inline typename QtPromisePrivate::PromiseHandler<T, TFulfilled>::Promise
    then(TFulfilled&& fulfilled) const;

    template <typename TRejected>
    inline typename QtPromisePrivate::PromiseHandler<T, std::nullptr_t>::Promise
    fail(TRejected&& rejected) const;

    template <typename THandler>
    inline QPromise<T> finally(THandler handler) const;

    template <typename THandler>
    inline QPromise<T> tap(THandler handler) const;

    template <typename E = QPromiseTimeoutException>
    inline QPromise<T> timeout(int msec, E&& error = E()) const;

    inline QPromise<T> delay(int msec) const;
    inline QPromise<T> wait() const;

    void swap(QPromiseBase<T>& other) { qSwap(m_d, other.m_d); }

public: // STATIC
    template <typename E>
    inline static QPromise<T> reject(E&& error);

protected:
    friend struct QtPromisePrivate::PromiseFulfill<QPromise<T> >;
    friend class QPromiseResolve<T>;
    friend class QPromiseReject<T>;

    QExplicitlySharedDataPointer<QtPromisePrivate::PromiseData<T> > m_d;
};

template <typename T>
class QPromise : public QPromiseBase<T>
{
public:
    template <typename F>
    QPromise(F&& resolver): QPromiseBase<T>(std::forward<F>(resolver)) { }

public: // STATIC
    inline static QPromise<QVector<T> > all(const QVector<QPromise<T> >& promises);
    inline static QPromise<T> resolve(T&& value);

private:
    friend class QPromiseBase<T>;
};

template <>
class QPromise<void> : public QPromiseBase<void>
{
public:
    template <typename F>
    QPromise(F&& resolver): QPromiseBase<void>(std::forward<F>(resolver)) { }

public: // STATIC
    inline static QPromise<void> all(const QVector<QPromise<void> >& promises);
    inline static QPromise<void> resolve();

private:
    friend class QPromiseBase<void>;
};

QTPROMISE_END_NAMESPACE

#include "qpromise.inl"

#endif // ifndef QTPROMISE_QPROMISE_H
