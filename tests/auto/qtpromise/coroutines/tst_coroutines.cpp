/*
 * Copyright (c) Simon Brunel, https://github.com/simonbrunel
 *
 * This source code is licensed under the MIT license found in
 * the LICENSE file in the root directory of this source tree.
 */

#include <QtPromise>
#include <QtTest>
#include <optional>

using namespace QtPromise;

template<typename T>
inline T p_await(const QPromise<T>& promise)
{
    std::optional<T> value;
    std::exception_ptr eptr;
    promise
        .then([&](const T& res) { value = res; })
        .fail([&]() { eptr = std::current_exception(); })
        .wait();
    if (eptr) { std::rethrow_exception(eptr); }
    return *value;
}

template<>
inline void p_await(const QPromise<void>& promise)
{
    std::exception_ptr eptr;
    promise
        .fail([&]() { eptr = std::current_exception(); })
        .wait();
    if (eptr) { std::rethrow_exception(eptr); }
}

struct custom_exception : public std::exception
{
};

class tst_coroutines : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void resolveSyncVoid();
    void resolveSyncValue();
    void rejectSyncVoid();
    void rejectSyncValue();
    void resolveAwaitVoid();
    void resolveAwaitValue();
    void rejectAwaitVoid();
    void rejectAwaitValue();

}; // class tst_coroutines

QTEST_MAIN(tst_coroutines)
#include "tst_coroutines.moc"

void tst_coroutines::resolveSyncVoid()
{
    auto p = ([]() -> QPromise<void> {
        co_return;
    })();
    QCOMPARE(p.isPending(), false);
    p_await(p);
}

void tst_coroutines::resolveSyncValue()
{
    auto p = ([]() -> QPromise<int> {
        co_return 42;
    })();
    QCOMPARE(p.isPending(), false);
    QCOMPARE(p_await(p), 42);
}

void tst_coroutines::rejectSyncVoid()
{
    auto p = ([]() -> QPromise<void> {
        throw custom_exception{};
        co_return;
    })();
    QCOMPARE(p.isPending(), false);
    QVERIFY_EXCEPTION_THROWN(p_await(p), custom_exception);
}

void tst_coroutines::rejectSyncValue()
{
    auto p = ([]() -> QPromise<int> {
        throw custom_exception{};
        co_return 42;
    })();
    QCOMPARE(p.isPending(), false);
    QVERIFY_EXCEPTION_THROWN(p_await(p), custom_exception);
}

void tst_coroutines::resolveAwaitVoid()
{
    auto p = ([]() -> QPromise<void> {
        co_await QPromise<void>::resolve();
        co_return;
    })();
    QCOMPARE(p.isPending(), true);
    p_await(p);
}

void tst_coroutines::resolveAwaitValue()
{
    auto p = ([]() -> QPromise<int> {
        const auto v = co_await QPromise<int>::resolve(42);
        co_return v;
    })();
    QCOMPARE(p.isPending(), true);
    QCOMPARE(p_await(p), 42);
}

void tst_coroutines::rejectAwaitVoid()
{
    auto p = ([]() -> QPromise<void> {
        co_await QPromise<void>::reject(custom_exception{});
        co_return;
    })();
    QCOMPARE(p.isPending(), true);
    QVERIFY_EXCEPTION_THROWN(p_await(p), custom_exception);
}

void tst_coroutines::rejectAwaitValue()
{
    auto p = ([]() -> QPromise<int> {
        const auto v = co_await QPromise<int>::reject(custom_exception{});
        co_return v;
    })();
    QCOMPARE(p.isPending(), true);
    QVERIFY_EXCEPTION_THROWN(p_await(p), custom_exception);
}
