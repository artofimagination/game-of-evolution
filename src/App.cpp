#include "App.h"

#include "QMLInterface.h"
#include "qml/ChartsConnector.h"

#include <QQmlEngine>
#include <QQmlContext>

#include <iostream>


//-----------------------------------------------------------------------------
App::App(int argc, char *argv[])
    : QApplication(argc, argv)
{
    qmlRegisterType<QML::QMLInterface>("backendGuiInterface", 1, 0, "QMLInterface");
    qmlRegisterType<QML::ChartsConnector>("backendGuiInterface", 1, 0, "ChartsConnector");
    m_MainView.engine()->rootContext()->setContextProperty("_window", &m_MainView);
    m_MainView.setResizeMode(QQuickView::SizeRootObjectToView);
    m_MainView.setSource(QUrl("qrc:/qml/main.qml"));

    QObject::connect(m_MainView.engine(), &QQmlEngine::quit, this, &App::Quit);
}

//-----------------------------------------------------------------------------
int App::Exec()
{
    m_MainView.showFullScreen();
    return exec();
}

//-----------------------------------------------------------------------------
void App::Quit()
{            
    quit();
    std::cout << "Exiting application..." << std::endl;
}

