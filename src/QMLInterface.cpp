#include "QMLInterface.h"

#include "Backend.h"
#include "Barriers/CircleBarrier.h"
#include "Barriers/RectangleBarrier.h"
#include "Challenges/AgainstAnyWall.h"
#include "Challenges/Altruism.h"
#include "Challenges/AltruismSacrifice.h"
#include "Challenges/CenterSparsed.h"
#include "Challenges/CenterUnweighted.h"
#include "Challenges/CenterWeighted.h"
#include "Challenges/Circle.h"
#include "Challenges/CircularSequence.h"
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
    qmlRegisterUncreatableType<QML::Barrier>("backendGuiInterface", 1, 0, "Barrier", "Not creatable as it is an enum type");
    qmlRegisterUncreatableType<QML::AnalyticsTypes>("backendGuiInterface", 1, 0, "AnalyticsTypes", "Not creatable as it is an enum type");
    qRegisterMetaType<QML::AltruismSetup>("AltruismSetup");
    qRegisterMetaType<QML::CircleSetup>("CircleSetup");
    qRegisterMetaType<QML::MultiCircleSetup>("MultiCircleSetup");
    qRegisterMetaType<QML::AnyWallSetup>("AnyWallSetup");
    qRegisterMetaType<QML::PairsSetup>("PairsSetup");
    qRegisterMetaType<QML::RectangleSetup>("RectangleSetup");
    qRegisterMetaType<QML::DoubleRectangleSetup>("DoubleRectangleSetup");
    qRegisterMetaType<QML::RadioactiveWallSetup>("RadioactiveWallSetup");
    qRegisterMetaType<QML::CenterSparsedSetup>("CenterSparsedSetup");
    qRegisterMetaType<QML::NeighborCountSetup>("NeighborCountSetup");
    qRegisterMetaType<QML::CircleBarrierSetup>("CircleBarrierSetup");
    qRegisterMetaType<QML::RectBarrierSetup>("RectBarrierSetup");
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
QML::MultiCircleSetup QMLInterface::GetCornerSetup()
{
    m_Mutex.lock();
    auto pChallenge = m_pBackendWorker->GetChallenge();
    auto setupInternal = static_cast<Challenges::Corner*>(pChallenge)->GetSetup();
    m_Mutex.unlock();
    QML::MultiCircleSetup setup;
    setup.color = QColor(0, 255, 0, 50);
    setup.radius = setupInternal.radius;
    for (const auto& center : setupInternal.centers)
    {
        setup.centers.push_back(QPoint(center.x, center.y));
    }
    return setup;
}

//-------------------------------------------------------------------------
QML::MultiCircleSetup QMLInterface::GetCircularSequenceSetup()
{
    m_Mutex.lock();
    auto pChallenge = m_pBackendWorker->GetChallenge();
    auto setupInternal = static_cast<Challenges::CircularSequence*>(pChallenge)->GetSetup();
    m_Mutex.unlock();
    QML::MultiCircleSetup setup;
    setup.color = QColor(0, 255, 0, 50);
    setup.radius = setupInternal.radius;
    for (const auto& center : setupInternal.centers)
    {
        setup.centers.push_back(QPoint(center.x, center.y));
    }
    return setup;
}

//-------------------------------------------------------------------------
QML::MultiCircleSetup QMLInterface::GetCornerWeightedSetup()
{
    m_Mutex.lock();
    auto pChallenge = m_pBackendWorker->GetChallenge();
    auto setupInternal = static_cast<Challenges::CornerWeighted*>(pChallenge)->GetSetup();
    m_Mutex.unlock();
    QML::MultiCircleSetup setup;
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

//-------------------------------------------------------------------------
QML::CircleBarrierSetup QMLInterface::GetCircleBarriers()
{
    QML::CircleBarrierSetup setup;
    m_Mutex.lock();
    auto& barriers = m_pBackendWorker->GetBarriers();
    for (size_t i = 0; i < barriers.size(); ++i)
    {
        auto setupInternal = static_cast<Barriers::Circle*>(barriers[i].get())->GetSetup();
        setup.radius = setupInternal.radius;
        setup.barriers.push_back(QPoint(setupInternal.center.x, setupInternal.center.y));
    }
    m_Mutex.unlock();
    
    setup.color = QColor(Qt::darkGray);
    return setup;
}

//-------------------------------------------------------------------------
QML::RectBarrierSetup QMLInterface::GetRectBarriers()
{
    QML::RectBarrierSetup setup;
    m_Mutex.lock();
    auto& barriers = m_pBackendWorker->GetBarriers();
    for (size_t i = 0; i < barriers.size(); ++i)
    {
        auto setupInternal = static_cast<Barriers::Rectangle*>(barriers[i].get())->GetSetup();
        setup.barriers.push_back(QRect(setupInternal.topLeft.x, setupInternal.topLeft.y, setupInternal.width + 1, setupInternal.height + 1));
    }
    m_Mutex.unlock();
    
    setup.color = QColor(Qt::darkGray);
    return setup;
}

//---------------------------------------------------------------------------
WorldData QMLInterface::GetWorldData()
{
    m_Mutex.lock();
    auto data = m_pBackendWorker->GetWorldData();
    m_Mutex.unlock();
    return data;
}

//---------------------------------------------------------------------------
QVariantList QMLInterface::GetSensorNames()
{
    QVariantList uiNames;
    auto names = m_pBackendWorker->GetSensorNames();
    for (const auto& name : names)
    {
        uiNames.push_back(QString::fromUtf8(name.c_str()));
    }
    return uiNames;
}

//---------------------------------------------------------------------------
QVariantList QMLInterface::GetActionNames()
{
    QVariantList uiNames;
    auto names = m_pBackendWorker->GetActionNames();
    for (const auto& name : names)
    {
        uiNames.push_back(QString::fromUtf8(name.c_str()));
    }
    return uiNames;
}

//---------------------------------------------------------------------------
void QMLInterface::UpdateSensorsActions(const QVariantList& sensors, const QVariantList& actions)
{
    m_Mutex.lock();
    m_pBackendWorker->UpdateSensorsActions(sensors, actions);
    m_Mutex.unlock();
}

//-------------------------------------------------------------------------
unsigned QMLInterface::GetChallengeId() const
{
    return static_cast<unsigned>(m_pBackendWorker->GetChallengeId()); 
};

//-------------------------------------------------------------------------
void QMLInterface::SetChallengeId(unsigned id)
{
    m_pBackendWorker->SetChallengeId(id); 
};

//-------------------------------------------------------------------------
unsigned QMLInterface::GetBarrierType() const
{
    return static_cast<unsigned>(m_pBackendWorker->GetBarrierType()); 
};

//-------------------------------------------------------------------------
void QMLInterface::Quit()
{
    std::cout << "Stopping backend..." << std::endl;
    m_pBackendWorker->StopThread();
    m_WorkerThread.quit();
    m_WorkerThread.wait();
}

//-------------------------------------------------------------------------
void QMLInterface::StartSim()
{
    m_pBackendWorker->StartSim();
}

//-------------------------------------------------------------------------
void QMLInterface::StopSim()
{
    m_pBackendWorker->StopSim();
}

//-------------------------------------------------------------------------
void QMLInterface::ResetSim()
{
    m_pBackendWorker->ResetSim();
}

//-------------------------------------------------------------------------
QVariantList QMLInterface::GetChallengeNames() const
{
    QVariantList uiNames;
    auto names = m_pBackendWorker->GetChallengeNames();
    for (const auto& name : names)
    {
        uiNames.push_back(QString::fromUtf8(name.c_str()));
    }
    return uiNames;
}

//-------------------------------------------------------------------------
QVariantList QMLInterface::GetSurvivors() const
{
    QVariantList lineGraphs{};
    QList<QVariant> survivorsUI{};
    auto survivors = m_pBackendWorker->GetSurvivors();
    for (size_t i = 0; i < survivors.second.size(); ++i)
    {
        survivorsUI.push_back(QPointF(survivors.first + i, survivors.second.at(i)));
    }
    lineGraphs.push_back(survivorsUI);
    return lineGraphs;
}

//-------------------------------------------------------------------------
QVariantList QMLInterface::GetSurvivorsToNextGen() const
{
    QVariantList lineGraphs{};
    QList<QVariant> survivorsUI{};
    auto survivors = m_pBackendWorker->GetSurvivorsToNextGen();
    for (size_t i = 0; i < survivors.second.size(); ++i)
    {
        survivorsUI.push_back(QPointF(survivors.first + i, survivors.second.at(i)));
    }
    lineGraphs.push_back(survivorsUI);
    return lineGraphs;
}

//-------------------------------------------------------------------------
QVariantList QMLInterface::GetAvgAges() const
{
    QVariantList lineGraphs{};
    QList<QVariant> uiData{};
    auto data = m_pBackendWorker->GetAvgAges();
    for (size_t i = 0; i < data.second.size(); ++i)
    {
        uiData.push_back(QPointF(data.first + i, data.second.at(i)));
    }
    lineGraphs.push_back(uiData);
    return lineGraphs;
}

//-------------------------------------------------------------------------
QVariantList QMLInterface::GetGeneticDiversity() const
{
    QVariantList lineGraphs{};
    QList<QVariant> dataUI{};
    auto dataVector = m_pBackendWorker->GetGeneticDiversity();
    for (size_t i = 0; i < dataVector.second.size(); ++i)
    {
        auto y = dataVector.second.at(i);
        dataUI.push_back(QPointF(dataVector.first + i, y));
    }
    lineGraphs.push_back(dataUI);
    return lineGraphs;
}

//-------------------------------------------------------------------------
QVariantList QMLInterface::GetCompletedChallengeTaskCounts() const
{
    QVariantList lineGraphs{};
    QList<QVariant> dataUI{};
    auto dataVector = m_pBackendWorker->GetCompletedChallengeTaskCounts();
    for (size_t i = 0; i < dataVector.second.size(); ++i)
    { 
        auto tasks = dataVector.second.at(i);
        for (size_t j = 0; j < tasks.size(); ++j)
        {
            auto y = tasks.at(j);
            dataUI.push_back(QPointF(dataVector.first + j, y));
        }
        lineGraphs.push_back(dataUI);
        dataUI.clear();
    }
    
    return lineGraphs;
}

//-------------------------------------------------------------------------
QVariantList QMLInterface::GetAnalyticsNames() const
{
    QVariantList uiNames;
    auto names = m_pBackendWorker->GetAnalyticsTypes();
    for (const auto& name : names)
    {
        uiNames.push_back(QString::fromUtf8(name.c_str()));
    }
    return uiNames;
}

//-------------------------------------------------------------------------
void QMLInterface::ClearAnalyticsProcessedCount()
{
    m_pBackendWorker->ClearAnalyticsProcessedCount();
}

//-------------------------------------------------------------------------
QSize QMLInterface::GetFrameSize() const
{
    auto size = m_pBackendWorker->GetFrameSize();
    return QSize(size.first, size.second);
}

} // namespace QML
