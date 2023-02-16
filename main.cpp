#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "gamecore.h"
#include "boardmodel.h"


int main(int argc, char *argv[])
{
    //Класс GameCore хранит основную логику игры. Через класс DBHandler осуществляется его работа с базой данных.
    //BoardModel - прослойка между "ядром" игры и qml-ной разметкой.
    //Всю графику я постарался уместить в qml-решение, чтобы явно разделить "бизнес-логику" и графическую оболочку игры.
    //BoardModel используется как модель для вьюшки qml-я, в то время как GameCore пробрасывается в qml как экземпляр класса.
    //Таким образом, рассуждая в идеологии "mvd", роль модели, как основы данных, играет связка GameCore+BoardModel,
    //а виюшка, как чистая визуализация, вся сделана на qml-разметке.

    GameCore test_core(9);

    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);

    qmlRegisterType<BoardModel>("GameModel", 1, 0, "BoardModel");

    QQmlApplicationEngine engine;
    engine.addImportPath(":/qml");
    engine.rootContext()->setContextProperty(QStringLiteral("game_core"), &test_core);
    const QUrl url(QStringLiteral("qrc:/qml/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
