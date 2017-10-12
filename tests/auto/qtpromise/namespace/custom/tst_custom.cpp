// QtPromise
#include <QtPromise>

// Qt
#include <QtTest>

class tst_namespace_custom : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void defines();
    void promise();
    void helpers();
    void resolver();
    void error();
};

QTEST_MAIN(tst_namespace_custom)
#include "tst_custom.moc"

QTPROMISE_BEGIN_NAMESPACE
class Bar {};
QTPROMISE_END_NAMESPACE

void tst_namespace_custom::defines()
{
    // Compile time expectations
    Q_STATIC_ASSERT((std::is_same<QTPROMISE_PREPEND_NAMESPACE(Bar), ::Foo::Bar>::value));
    Q_STATIC_ASSERT((std::is_same<QTPROMISE_PREPEND_NAMESPACE(Bar), ::Foo::Bar>::value));

    QTPROMISE_USE_NAMESPACE
    Q_STATIC_ASSERT((std::is_same<Bar, ::Foo::Bar>::value));
}

void tst_namespace_custom::promise()
{
    ::Foo::QPromise<int>::resolve(42);
    ::Foo::QPromise<void>::resolve();

    QTPROMISE_USE_NAMESPACE
    QPromise<int>::resolve(42);
    QPromise<void>::resolve();
}

void tst_namespace_custom::helpers()
{
    ::Foo::qPromise();
    ::Foo::qPromise(42);
    ::Foo::qPromiseAll(QVector<::Foo::QPromise<void>>{::Foo::qPromise()});
    ::Foo::qPromiseAll(QVector<::Foo::QPromise<int>>{::Foo::qPromise(42)});

    QTPROMISE_USE_NAMESPACE
    qPromise();
    qPromise(42);
    qPromiseAll(QVector<::Foo::QPromise<void>>{::Foo::qPromise()});
    qPromiseAll(QVector<::Foo::QPromise<int>>{::Foo::qPromise(42)});
}

void tst_namespace_custom::resolver()
{
    ::Foo::QPromise<int>([](const ::Foo::QPromiseResolve<int>&, const ::Foo::QPromiseReject<int>&) {});
    ::Foo::QPromise<void>([](const ::Foo::QPromiseResolve<void>&, const ::Foo::QPromiseReject<void>&) {});

    QTPROMISE_USE_NAMESPACE
    QPromise<int>([](const QPromiseResolve<int>&, const QPromiseReject<int>&) {});
    QPromise<void>([](const QPromiseResolve<void>&, const QPromiseReject<void>&) {});
}

void tst_namespace_custom::error()
{
    ::Foo::QPromiseError(42);
    ::Foo::QPromiseTimeoutException();
    ::Foo::QPromiseCanceledException();

    QTPROMISE_USE_NAMESPACE
    QPromiseError(42);
    QPromiseTimeoutException();
    QPromiseCanceledException();
}
