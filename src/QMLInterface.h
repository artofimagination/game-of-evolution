#pragma once

#include <QMutex>
#include <QObject>
#include <QQmlEngine>
#include <QThread>

class Backend;

//! Responsible for interfacing the backend engine, with the QML UI.
//! Also starts the backend thread.
class QMLInterface : public QObject
{
    Q_OBJECT
public:
    //! Starts the backend thread.
    Q_INVOKABLE QObject* StartBackend();

    Q_INVOKABLE int UiScale() { return cUiScaling; }

    Q_INVOKABLE void Quit();

private:
    //! Enlarge the simulation data.
    const int cUiScaling = 6;

    QThread     m_WorkerThread;
    Backend*    m_pBackendWorker;
};
