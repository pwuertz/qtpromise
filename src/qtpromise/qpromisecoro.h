/*
 * Copyright (c) Simon Brunel, https://github.com/simonbrunel
 *
 * This source code is licensed under the MIT license found in
 * the LICENSE file in the root directory of this source tree.
 */

#ifndef QTPROMISE_QPROMISECORO_H
#define QTPROMISE_QPROMISECORO_H

#include <QtPromise>
#include <optional>

// Include <coroutine>, bypass GCC / MSVC guards when clang is used for code analysis
#if defined(__GNUC__) && defined(__clang__) && !defined(__cpp_impl_coroutine)
#define __cpp_impl_coroutine true
#elif defined(_MSC_VER) && defined(__clang__) && !defined(__cpp_lib_coroutine)
#define __cpp_lib_coroutine true
#endif
#include <coroutine>

// If clang is used for code analysis, export coroutine types from std to experimental
#if defined(__clang__)
namespace std::experimental {
using std::coroutine_traits;
using std::coroutine_handle;
using std::suspend_always;
using std::suspend_never;
}
#endif


// Allow using QPromise<void> as coroutine return type
template <typename... Args>
struct std::coroutine_traits<QtPromise::QPromise<void>, Args...>
{
    struct promise_type
    {
        QtPromise::QPromise<void> get_return_object() noexcept
        {
            return QtPromise::QPromise<void>([&](const auto& resolve_, const auto& reject_) {
                resolve = resolve_;
                reject = reject_;
            });
        }

        constexpr std::suspend_never initial_suspend() const noexcept { return {}; }

        constexpr std::suspend_never final_suspend() const noexcept { return {}; }

        void return_void() const noexcept
        {
            (*resolve)();
        };

        void unhandled_exception() const noexcept
        {
            (*reject)(std::current_exception());
        }

    private:
        std::optional<QtPromise::QPromiseResolve<void>> resolve;
        std::optional<QtPromise::QPromiseReject<void>> reject;
    };
};


// Allow using non-void QPromise<T> as coroutine return type
template <typename T, typename... Args>
requires(!std::is_void_v<T>)
struct std::coroutine_traits<QtPromise::QPromise<T>, Args...>
{
    struct promise_type
    {
        QtPromise::QPromise<T> get_return_object() noexcept
        {
            return QtPromise::QPromise<T>([&](const auto& resolve_, const auto& reject_) {
                resolve = resolve_;
                reject = reject_;
            });
        }

        constexpr std::suspend_never initial_suspend() const noexcept { return {}; }

        constexpr std::suspend_never final_suspend() const noexcept { return {}; }

        void return_value(const T& value) const noexcept(std::is_nothrow_copy_constructible_v<T>)
        {
            (*resolve)(value);
        }

        void return_value(T&& value) const noexcept(std::is_nothrow_move_constructible_v<T>)
        {
            (*resolve)(std::move(value));
        }

        void unhandled_exception() const noexcept
        {
            (*reject)(std::current_exception());
        }

    private:
        std::optional<QtPromise::QPromiseResolve<T>> resolve;
        std::optional<QtPromise::QPromiseReject<T>> reject;
    };
};


// Allow using non-void QPromise<T> in co_await expressions
template <typename T>
auto operator co_await(QtPromise::QPromise<T> promise) noexcept
{
    struct awaiter
    {
        QtPromise::QPromise<T> promise;
        const T * value = nullptr;
        std::exception_ptr eptr = nullptr;

        constexpr bool await_ready() const noexcept { return false; }

        void await_suspend(std::coroutine_handle<> cont) noexcept
        {
            promise.then(
                [this, cont](const T& v) { value = std::addressof(v); cont(); },
                [this, cont]() { eptr = std::current_exception(); cont(); }
                );
        }

        T await_resume() const
        {
            if (eptr) { std::rethrow_exception(eptr); }
            return *value;
        }
    };

    return awaiter{ .promise = std::move(promise) };
}


// Allow using QPromise<void> in co_await expressions
template <>
auto operator co_await(QtPromise::QPromise<void> promise) noexcept
{
    struct awaiter
    {
        QtPromise::QPromise<void> promise;
        std::exception_ptr eptr = nullptr;

        constexpr bool await_ready() const noexcept { return false; }

        void await_suspend(std::coroutine_handle<> cont) noexcept
        {
            promise.then(
                cont,
                [this, cont]() { eptr = std::current_exception(); cont(); }
                );
        }

        void await_resume() const
        {
            if (eptr) { std::rethrow_exception(eptr); }
        }
    };

    return awaiter{ .promise = std::move(promise) };
}

#endif // QTPROMISE_QPROMISECORO_H
