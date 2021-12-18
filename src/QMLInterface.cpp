#include "QMLInterface.h"

#include "Backend.h"
#include "Challenges/AgainstAnyWall.h"
#include "Challenges/Altruism.h"
#include "Challenges/AltruismSacrifice.h"
#include "Challenges/CenterSparsed.h"
#include "Challenges/CenterUnweighted.h"
#include "Challenges/CenterWeighted.h"
#include "Challenges/Circle.h"
#include "Challenges/Corner.h"
#include "Challenges/CornerWeighted.h"
#include "Challenges/EastWestEighths.h"
#include "Challenges/iChallenges.h"
#include "Challenges/LeftEighth.h"
#include "Challenges/NeighborCount.h"
#include "Challenges/Pairs.h"
#include "Challenges/RightHalf.h"
#include "Challenges/RightQuarter.h"
#include "Challenges/RadioactiveWalls.h"
#include "Challenges/TouchAnyWall.h"

#include <iostream>

namespace QML
{

//-------------------------------------------------------------------------
QObject* QMLInterface::StartBackend()
{
    std::cout << "Starting backend..." << std::endl;
    qmlRegisterUncreatableType<QML::Challenge>("backendGuiInterface", 1, 0, "Challenge", "Not creatable as it is an enum type");
    qRegisterMetaType<QML::AltruismSetup>("AltruismSetup");
    qRegisterMetaType<QML::CircleSetup>("CircleSetup");
    qRegisterMetaType<QML::CornerSetup>("CornerSetup");
    qRegisterMetaType<QML::AnyWallSetup>("AnyWallSetup");
    qRegisterMetaType<QML::PairsSetup>("PairsSetup");
    qRegisterMetaType<QML::RectangleSetup>("RectangleSetup");
    qRegisterMetaType<QML::DoubleRectangleSetup>("DoubleRectangleSetup");
    qRegisterMetaType<QML::RadioactiveWallSetup>("RadioactiveWallSetup");
    qRegisterMetaType<QML::CenterSparsedSetup>("CenterSparsedSetup");
    qRegisterMetaType<QML::NeighborCountSetup>("NeighborCountSetup");
    m_pBackendWorker = new Backend();
    m_pBackendWorker->moveToThread(&m_WorkerThread);
    connect(&m_WorkerThread, &QThread::finished, m_pBackendWorker, &Backend::deleteLater);
    connect(&m_WorkerThread, &QThread::started, m_pBackendWorker, &Backend::Run);
    m_WorkerThread.start();
    return m_pBackendWorker;
}

//-------------------------------------------------------------------------
QML::AltruismSetup QMLInterface::GetAltruismSetup()
{
    m_Mutex.lock();
    auto pChallenge = m_pBackendWorker->GetChallenge();
    auto setupInternal = static_cast<Challenges::Altruism*>(pChallenge)->GetSetup();
    m_Mutex.unlock();
    QML::AltruismSetup setup;
    setup.altruismColor = QColor(0, 255, 0, 50);
    setup.altruismRadius = setupInternal.radius;
    setup.altruismCenter = QPoint(setupInternal.center.x, setupInternal.center.y);
    setup.sacrificeColor = QColor(255, 0, 0, 50);
    setup.sacrificeRadius = setupInternal.sacrificeRadius;
    setup.sacrificeCenter = QPoint(setupInternal.sacrificeCenter.x, setupInternal.sacrificeCenter.y);
    return setup;
}

//-------------------------------------------------------------------------
QML::CircleSetup QMLInterface::GetAltruismSacrificeSetup()
{
    m_Mutex.lock();
    auto pChallenge = m_pBackendWorker->GetChallenge();
    auto setupInternal = static_cast<Challenges::AltruismSacrifice*>(pChallenge)->GetSetup();
    m_Mutex.unlock();
    QML::CircleSetup setup;
    setup.color = QColor(255, 0, 0, 50);
    setup.radius = setupInternal.radius;
    setup.center = QPoint(setupInternal.center.x, setupInternal.center.y);
    return setup;
}

//-------------------------------------------------------------------------
QML::RectangleSetup QMLInterface::GetRightHalfSetup()
{
    m_Mutex.lock();
    auto pChallenge = m_pBackendWorker->GetChallenge();
    auto setupInternal = static_cast<Challenges::RightHalf*>(pChallenge)->GetSetup();
    m_Mutex.unlock();
    QML::RectangleSetup setup;
    setup.color = QColor(0, 255, 0, 50);
    setup.rect = QRect(setupInternal.topLeft.x, setupInternal.topLeft.y, setupInternal.width, setupInternal.height);
    return setup;
}

//-------------------------------------------------------------------------
QML::PairsSetup QMLInterface::GetPairsSetup()
{
    m_Mutex.lock();
    auto pChallenge = m_pBackendWorker->GetChallenge();
    auto setupInternal = static_cast<Challenges::Pairs*>(pChallenge)->GetSetup();
    m_Mutex.unlock();
    QML::PairsSetup setup;
    setup.color = QColor(255, 0, 0, 50);
    for (auto border : setupInternal.borders)
    {
        setup.borders.push_back(border);
    }
    return setup;
}

//-------------------------------------------------------------------------
QML::DoubleRectangleSetup QMLInterface::GetEastWestEighthsSetup()
{
    m_Mutex.lock();
    auto pChallenge = m_pBackendWorker->GetChallenge();
    auto setupInternal = static_cast<Challenges::EastWestEighths*>(pChallenge)->GetSetup();
    m_Mutex.unlock();
    QML::DoubleRectangleSetup setup;
    setup.color = QColor(0, 255, 0, 50);
    setup.rectLeft = QRect(setupInternal.eastTopLeft.x, setupInternal.eastTopLeft.y, setupInternal.width, setupInternal.height);
    setup.rectRight = QRect(setupInternal.westTopLeft.x, setupInternal.westTopLeft.y, setupInternal.width, setupInternal.height);
    return setup;
}

//-------------------------------------------------------------------------
QML::AnyWallSetup QMLInterface::GetTouchAnyWallSetup()
{
    m_Mutex.lock();
    auto pChallenge = m_pBackendWorker->GetChallenge();
    auto setupInternal = static_cast<Challenges::TouchAnyWall*>(pChallenge)->GetSetup();
    m_Mutex.unlock();
    QML::AnyWallSetup setup;
    setup.color = QColor(0, 255, 0, 50);
    for (auto border : setupInternal.borders)
    {
        setup.borders.push_back(border);
    }
    return setup;
}

//-------------------------------------------------------------------------
QML::AnyWallSetup QMLInterface::GetAgainstAnyWallSetup()
{
    m_Mutex.lock();
    auto pChallenge = m_pBackendWorker->GetChallenge();
    auto setupInternal = static_cast<Challenges::AgainstAnyWall*>(pChallenge)->GetSetup();
    m_Mutex.unlock();
    QML::AnyWallSetup setup;
    setup.color = QColor(0, 255, 0, 50);
    for (auto border : setupInternal.borders)
    {
        setup.borders.push_back(border);
    }
    return setup;
}

//-------------------------------------------------------------------------
QML::RadioactiveWallSetup QMLInterface::GetRadioactiveWallSetup()
{
    m_Mutex.lock();
    auto pChallenge = m_pBackendWorker->GetChallenge();
    auto setupInternal = static_cast<Challenges::RadioactiveWalls*>(pChallenge)->GetSetup();
    m_Mutex.unlock();
    QML::RadioactiveWallSetup setup;
    setup.border = setupInternal.border;
    setup.color = QColor(255, 0, 0);
    setup.distance = setupInternal.distance;
    return setup;
}

//-------------------------------------------------------------------------
QML::RectangleSetup QMLInterface::GetRightQuarterSetup()
{
    m_Mutex.lock();
    auto pChallenge = m_pBackendWorker->GetChallenge();
    auto setupInternal = static_cast<Challenges::RightQuarter*>(pChallenge)->GetSetup();
    m_Mutex.unlock();
    QML::RectangleSetup setup;
    setup.color = QColor(0, 255, 0, 50);
    setup.rect = QRect(setupInternal.topLeft.x, setupInternal.topLeft.y, setupInternal.width, setupInternal.height);
    return setup;
}

//-------------------------------------------------------------------------
QML::RectangleSetup QMLInterface::GetLeftEighthSetup()
{
    m_Mutex.lock();
    auto pChallenge = m_pBackendWorker->GetChallenge();
    auto setupInternal = static_cast<Challenges::LeftEighth*>(pChallenge)->GetSetup();
    m_Mutex.unlock();
    QML::RectangleSetup setup;
    setup.color = QColor(0, 255, 0, 50);
    setup.rect = QRect(setupInternal.topLeft.x, setupInternal.topLeft.y, setupInternal.width, setupInternal.height);
    return setup;
}

//-------------------------------------------------------------------------
QML::CircleSetup QMLInterface::GetCircleSetup()
{
    m_Mutex.lock();
    auto pChallenge = m_pBackendWorker->GetChallenge();
    auto setupInternal = static_cast<Challenges::Circle*>(pChallenge)->GetSetup();
    m_Mutex.unlock();
    QML::CircleSetup setup;
    setup.color = QColor(0, 255, 0, 50);
    setup.radius = setupInternal.radius;
    setup.center = QPoint(setupInternal.center.x, setupInternal.center.y);
    return setup;
}

//-------------------------------------------------------------------------
QML::CircleSetup QMLInterface::GetCenterWeightedSetup()
{
    m_Mutex.lock();
    auto pChallenge = m_pBackendWorker->GetChallenge();
    auto setupInternal = static_cast<Challenges::CenterWeighted*>(pChallenge)->GetSetup();
    m_Mutex.unlock();
    QML::CircleSetup setup;
    setup.color = QColor(0, 255, 0, 50);
    setup.radius = setupInternal.radius;
    setup.center = QPoint(setupInternal.safeCenter.x, setupInternal.safeCenter.y);
    return setup;
}

//-------------------------------------------------------------------------
QML::CircleSetup QMLInterface::GetCenterUnweightedSetup()
{
    m_Mutex.lock();
    auto pChallenge = m_pBackendWorker->GetChallenge();
    auto setupInternal = static_cast<Challenges::CenterUnweighted*>(pChallenge)->GetSetup();
    m_Mutex.unlock();
    QML::CircleSetup setup;
    setup.color = QColor(0, 255, 0, 50);
    setup.radius = setupInternal.radius;
    setup.center = QPoint(setupInternal.safeCenter.x, setupInternal.safeCenter.y);
    return setup;
}

//-------------------------------------------------------------------------
QML::CenterSparsedSetup QMLInterface::GetCenterSparsedSetup()
{
    m_Mutex.lock();
    auto pChallenge = m_pBackendWorker->GetChallenge();
    auto setupInternal = static_cast<Challenges::CenterSparsed*>(pChallenge)->GetSetup();
    m_Mutex.unlock();
    QML::CenterSparsedSetup setup;
    setup.color = QColor(0, 255, 0, 50);
    setup.radius = setupInternal.radius;
    setup.center = QPoint(setupInternal.center.x, setupInternal.center.y);
    return setup;
}

//-------------------------------------------------------------------------
QML::CornerSetup QMLInterface::GetCornerSetup()
{
    m_Mutex.lock();
    auto pChallenge = m_pBackendWorker->GetChallenge();
    auto setupInternal = static_cast<Challenges::Corner*>(pChallenge)->GetSetup();
    m_Mutex.unlock();
    QML::CornerSetup setup;
    setup.color = QColor(0, 255, 0, 50);
    setup.radius = setupInternal.radius;
    for (const auto& center : setupInternal.centers)
    {
        setup.centers.push_back(QPoint(center.x, center.y));
    }
    return setup;
}

//-------------------------------------------------------------------------
QML::CornerSetup QMLInterface::GetCornerWeightedSetup()
{
    m_Mutex.lock();
    auto pChallenge = m_pBackendWorker->GetChallenge();
    auto setupInternal = static_cast<Challenges::CornerWeighted*>(pChallenge)->GetSetup();
    m_Mutex.unlock();
    QML::CornerSetup setup;
    setup.color = QColor(0, 255, 0, 50);
    setup.radius = setupInternal.radius;
    for (const auto& center : setupInternal.centers)
    {
        setup.centers.push_back(QPoint(center.x, center.y));
    }
    return setup;
}

//-------------------------------------------------------------------------
QML::NeighborCountSetup QMLInterface::GetNeighborCountSetup()
{
    m_Mutex.lock();
    auto pChallenge = m_pBackendWorker->GetChallenge();
    auto setupInternal = static_cast<Challenges::NeighborCount*>(pChallenge)->GetSetup();
    m_Mutex.unlock();
    QML::NeighborCountSetup setup;
    setup.color = QColor(255, 0, 0, 40);
    for (auto border : setupInternal.borders)
    {
        setup.borders.push_back(border);
    }
    return setup;
}

//---------------------------------------------------------------------------
ImageFrameData QMLInterface::GetImageFrameData()
{
    m_Mutex.lock();
    auto data = m_pBackendWorker->GetImageFrameData();
    m_Mutex.unlock();
    return data;
}

//-------------------------------------------------------------------------
unsigned QMLInterface::GetChallengeId() 
{
    return static_cast<unsigned>(m_pBackendWorker->GetChallengeId()); 
};

//-------------------------------------------------------------------------
void QMLInterface::Quit()
{
    std::cout << "Stopping backend..." << std::endl;
    m_pBackendWorker->Stop();
    m_WorkerThread.quit();
    m_WorkerThread.wait();
}

} // namespace QML
