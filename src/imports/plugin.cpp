// QtPromise
#include <QtQmlPromise>

// Qt
#include <QQmlEngine>
#include <QQmlExtensionPlugin>

static const char* kJsPrivateNs = "__qtpromise_private__";

using namespace QtPromisePrivate;

QTPROMISE_USE_NAMESPACE

class QtQmlPromiseObject : public QObject
{
    Q_OBJECT

public:
    QtQmlPromiseObject(QJSEngine* engine)
        : QObject(engine)
        , m_engine(engine)
    {
        Q_ASSERT(engine);
    }

    Q_INVOKABLE QJSValue create(const QJSValue& resolver, const QJSValue& prototype)
    {
        QJSValue value = m_engine->toScriptValue(QJSPromise(m_engine, resolver));
        value.setPrototype(prototype);
        return value;
    }

    Q_INVOKABLE QTQMLPROMISE_METATYPE(QJSPromise) resolve(QJSValue value)
    {
        return QJSPromise::resolve(std::move(value));
    }

    Q_INVOKABLE QTQMLPROMISE_METATYPE(QJSPromise) reject(QJSValue error)
    {
        return QJSPromise::reject(std::move(error));
    }

    Q_INVOKABLE QTQMLPROMISE_METATYPE(QJSPromise) all(QJSValue input)
    {
        return QJSPromise::all(std::move(input));
    }

private:
    QJSEngine* m_engine;
};

class QtQmlPromisePlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QQmlExtensionInterface_iid)

public:
    void registerTypes(const char* uri) Q_DECL_OVERRIDE
    {
        Q_ASSERT(QLatin1String(uri) == QLatin1String("QtPromise"));
        Q_UNUSED(uri);

        qRegisterMetaType<QJSPromise>();
    }

    void initializeEngine(QQmlEngine* engine, const char* uri) Q_DECL_OVERRIDE
    {
        Q_ASSERT(QLatin1String(uri) == QLatin1String("QtPromise"));
        Q_UNUSED(uri);

        QJSValue global = engine->globalObject();
        QJSValue object = engine->newQObject(new QtQmlPromiseObject(engine));
        global.setProperty(kJsPrivateNs, object);
    }

}; // class QtQmlPromisePlugin

#include "plugin.moc"
