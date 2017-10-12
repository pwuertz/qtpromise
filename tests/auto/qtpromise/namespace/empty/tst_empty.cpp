// QtPromise
#include <QtPromise>

// Qt
#include <QtTest>

class tst_namespace_empty : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void defines();
    void promise();
    void helpers();
    void resolver();
    void error();
};

QTEST_MAIN(tst_namespace_empty)
#include "tst_empty.moc"

QTPROMISE_BEGIN_NAMESPACE
class Bar {};
QTPROMISE_END_NAMESPACE

void tst_namespace_empty::defines()
{
    Q_STATIC_ASSERT((std::is_same<QTPROMISE_PREPEND_NAMESPACE(Bar), ::Bar>::value));
    Q_STATIC_ASSERT((std::is_same<QTPROMISE_PREPEND_NAMESPACE(Bar), ::Bar>::value));

    QTPROMISE_USE_NAMESPACE
    Q_STATIC_ASSERT((std::is_same<Bar, ::Bar>::value));
}

void tst_namespace_empty::promise()
{
    ::QPromise<int>::resolve(42);
    ::QPromise<void>::resolve();
}

void tst_namespace_empty::helpers()
{
    ::qPromise();
    ::qPromise(42);
    ::qPromiseAll(QVector<::QPromise<void>>{::qPromise()});
    ::qPromiseAll(QVector<::QPromise<int>>{::qPromise(42)});
}

void tst_namespace_empty::resolver()
{
    ::QPromise<int>([](const ::QPromiseResolve<int>&, const ::QPromiseReject<int>&) {});
    ::QPromise<void>([](const ::QPromiseResolve<void>&, const ::QPromiseReject<void>&) {});
}

void tst_namespace_empty::error()
{
    ::QPromiseError(42);
    ::QPromiseTimeoutException();
    ::QPromiseCanceledException();
}
