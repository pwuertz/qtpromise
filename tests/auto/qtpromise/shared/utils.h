#ifndef QTPROMISE_TESTS_AUTO_SHARED_UTILS_H
#define QTPROMISE_TESTS_AUTO_SHARED_UTILS_H

#include <QtPromise>

template <typename T>
static inline T waitForValue(const QTPROMISE_PREPEND_NAMESPACE(QPromise<T>)& promise, const T& initial)
{
    T value(initial);
    promise.then([&](const T& res) {
        value = res;
    }).wait();
    return value;
}

template <typename T>
static inline T waitForValue(const QTPROMISE_PREPEND_NAMESPACE(QPromise<void>)& promise, const T& initial, const T& expected)
{
    T value(initial);
    promise.then([&]() {
        value = expected;
    }).wait();
    return value;
}

template <typename T, typename E>
static inline E waitForError(const QTPROMISE_PREPEND_NAMESPACE(QPromise<T>)& promise, const E& initial)
{
    E error(initial);
    promise.fail([&](const E& err) {
        error = err;
        return T();
    }).wait();
    return error;
}

template <typename E>
static inline E waitForError(const QTPROMISE_PREPEND_NAMESPACE(QPromise<void>)& promise, const E& initial)
{
    E error(initial);
    promise.fail([&](const E& err) {
        error = err;
    }).wait();
    return error;
}

#endif // QTPROMISE_TESTS_AUTO_SHARED_UTILS_H
