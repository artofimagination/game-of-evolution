#include "QMLInterface.h"

#include "Backend.h"

#include <iostream>

//-------------------------------------------------------------------------
QObject* QMLInterface::StartBackend()
{
    std::cout << "Starting backend..." << std::endl;
    m_pBackendWorker = new Backend();
    m_pBackendWorker->moveToThread(&m_WorkerThread);
    connect(&m_WorkerThread, &QThread::finished, m_pBackendWorker, &Backend::deleteLater);
    connect(&m_WorkerThread, &QThread::started, m_pBackendWorker, &Backend::Run);
    m_WorkerThread.start();
    return m_pBackendWorker;
}

//-------------------------------------------------------------------------
void QMLInterface::Quit()
{
    std::cout << "Stopping backend..." << std::endl;
    m_pBackendWorker->Stop();
    m_WorkerThread.quit();
    m_WorkerThread.wait();
}
