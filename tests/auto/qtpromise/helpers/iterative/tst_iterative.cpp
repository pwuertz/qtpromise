#include "../../shared/object.h"
#include "../../shared/utils.h"

// QtPromise
#include <QtPromise>
#include <qtpromise/qpromiseiterative.h>

// Qt
#include <QtTest>

using namespace QtPromise;

class tst_helpers_iterative : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void testAsyncLoop();

    void testAsyncFor();
    void testAsyncForBreak();

    void testAsyncWhile();
    void testAsyncWhileBreak();
};

QTEST_MAIN(tst_helpers_iterative)
#include "tst_iterative.moc"

void tst_helpers_iterative::testAsyncLoop()
{
    int i = 0;
    auto p = QtPromise::async_loop([&]() {
        if (++i < 3) {
            return resolve();
        }
        throw QPromiseStopIteration();
    });
    p.wait();
    QCOMPARE(p.isPending(), false);
    QCOMPARE(p.isRejected(), false);
    QCOMPARE(i, 3);
}

void tst_helpers_iterative::testAsyncFor()
{
    QVector<int> vec = {1, 2, 3};
    auto p = QtPromise::async_for(vec, [](int) {
        return resolve();
    });
    p.wait();
    QCOMPARE(p.isPending(), false);
    QCOMPARE(p.isRejected(), false);
}

void tst_helpers_iterative::testAsyncForBreak()
{
    QVector<int> vec = {1, 2, 3};
    auto p = QtPromise::async_for(vec, [](int i) {
        if (i == 2) { throw QPromiseStopIteration(); }
        return resolve();
    });
    p.wait();
    QCOMPARE(p.isPending(), false);
    QCOMPARE(p.isRejected(), false);
}

void tst_helpers_iterative::testAsyncWhile()
{
    int i = 0;
    auto p = QtPromise::async_while(
        [&]() { return i < 3; },
        [&]() {
            ++i;
            return resolve();
        }
    );
    p.wait();
    QCOMPARE(p.isPending(), false);
    QCOMPARE(p.isRejected(), false);
}

void tst_helpers_iterative::testAsyncWhileBreak()
{
    int i = 0;
    auto p = QtPromise::async_while(
        [&]() { return i < 3; },
        [&]() {
            ++i;
            throw QPromiseStopIteration();
        }
    );
    p.wait();
    QCOMPARE(p.isPending(), false);
    QCOMPARE(p.isRejected(), false);
}
