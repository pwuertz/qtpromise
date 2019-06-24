#ifndef QPROMISEITERATIVE_H
#define QPROMISEITERATIVE_H

#include "qpromise.h"
#include "qpromiseexceptions.h"

#include <functional>

namespace QtPromisePrivate {

template<typename Collection, typename Func>
struct PromiseAsyncForData {
    using Iterator = typename Collection::iterator;

    PromiseAsyncForData(Collection collection_, Func func_)
        : collection(std::move(collection_))
        , func(std::move(func_))
        , it(std::begin(collection))
    { }

    QtPromise::QPromise<void> next()
    {
        if (it == std::end(collection)) {
            return QtPromise::resolve();
        }
        return func(*(it++)).then([this]() {
            return next();
        });
    }

private:
    Collection collection;
    Func func;
    Iterator it;
};


template<typename FPredicate, typename FBody>
struct PromiseAsyncWhileData {

    PromiseAsyncWhileData(FPredicate predicate_, FBody body_)
        : predicate(std::move(predicate_))
        , body(std::move(body_))
    { }

    QtPromise::QPromise<void> next()
    {
        return QtPromise::attempt([&]() {
            return predicate();
        }).then([this](bool continue_) -> QtPromise::QPromise<void> {
            if (!continue_) {
                return QtPromise::resolve();
            }
            return QtPromise::attempt([&]() {
                return body();
            }).then([this]() {
                return next();
            });
        });
    }

private:
    FPredicate predicate;
    FBody body;
};


} // namespace QtPromisePrivate

namespace QtPromise {

template<typename Func>
QPromise<void> async_loop(Func&& func)
{
    struct Data {
        std::function<QPromise<void>()> func;
        QtPromise::QPromise<void> next()
        {
            return func().then([this]() { return next(); });
        }
    };
    auto data = QSharedPointer<Data>(new Data { std::forward<Func>(func) });
    return data->next().fail([data](const QPromiseStopIteration&) {
        /* ignore StopIteration, extend data lifetime */
    });
}

template<typename Predicate, typename Body>
QPromise<void> async_while(Predicate&& predicate, Body&& body)
{
    using RPredicate = typename std::result_of<Predicate()>::type;
    using RBody = typename std::result_of<Body()>::type;
    using FPredicate = std::function<RPredicate()>;
    using FBody = std::function<RBody()>;
    using Data = QtPromisePrivate::PromiseAsyncWhileData<FPredicate, FBody>;

    auto data = QSharedPointer<Data>(new Data(
        std::forward<Predicate>(predicate), std::forward<Body>(body)));
    return data->next().fail([data](const QPromiseStopIteration&) {
        /* ignore StopIteration, extend data lifetime */
    });
}

template<typename Collection, typename Handler>
QPromise<void> async_for(Collection&& collection, Handler&& handler)
{
    using TStorage = typename std::remove_reference<Collection>::type;
    using HandlerArg = typename QtPromisePrivate::ArgsOf<Handler>::first;
    using HandlerFunc = std::function<QPromise<void>(HandlerArg)>;
    using Data = QtPromisePrivate::PromiseAsyncForData<TStorage, HandlerFunc>;

    auto data = QSharedPointer<Data>(new Data(
        std::forward<Collection>(collection), std::forward<Handler>(handler)));
    return data->next().fail([data](const QPromiseStopIteration&) {
        /* ignore StopIteration, extend data lifetime */
    });
}

} // namespace QtPromise

#endif // QPROMISEITERATIVE_H
