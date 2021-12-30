#pragma once

#include "QMLChallengeItems.h"
#include "Backend.h"

#include <QMetaType>
#include <QMutex>
#include <QObject>
#include <QQmlEngine>
#include <QThread>

#include <cstdint>

class Backend;

namespace QML {

//! Responsible for interfacing the backend engine, with the QML UI.
//! Also starts the backend thread.
class QMLInterface : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QML::AltruismSetup altruismSetup READ GetAltruismSetup)
    Q_PROPERTY(unsigned challengeId READ GetChallengeId)
    Q_PROPERTY(WorldData imageData READ GetWorldData)
public:

    //! Starts the backend thread.
    Q_INVOKABLE QObject* StartBackend();
    //! Requests a simulation start from QML UI.
    Q_INVOKABLE void StartSim();
    //! Requests a Simulation stop from QML UI.
    Q_INVOKABLE void StopSim();
    //! Requests a simulation reset from QML UI.
    Q_INVOKABLE void ResetSim();
    //! Shuts down the back end thread when quit is requested from QML UI.
    Q_INVOKABLE void Quit();

    //! The world is a low resolution canvas. In order the get a UI friendly representation scaling is applied
    Q_INVOKABLE int UiScale() const { return cUiScaling; }
    //! Returns the raw world size.
    Q_INVOKABLE QSize GetFrameSize() const;

    Q_INVOKABLE unsigned GetChallengeId() const;
    Q_INVOKABLE void SetChallengeId(unsigned id);
    //! Returns the world data of the current simulation step.
    Q_INVOKABLE WorldData GetWorldData();

    ///////////////////////////////////////////////////////////////////////////////
    //! QML invokable getters for challenge UI setups
    Q_INVOKABLE QML::AltruismSetup GetAltruismSetup();
    Q_INVOKABLE QML::CircleSetup GetCircleSetup();
    Q_INVOKABLE QML::AnyWallSetup GetAgainstAnyWallSetup();
    Q_INVOKABLE QML::DoubleRectangleSetup GetEastWestEighthsSetup();
    Q_INVOKABLE QML::AnyWallSetup GetTouchAnyWallSetup();
    Q_INVOKABLE QML::PairsSetup GetPairsSetup();
    Q_INVOKABLE QML::CircleSetup GetCenterWeightedSetup();
    Q_INVOKABLE QML::CenterSparsedSetup GetCenterSparsedSetup();
    Q_INVOKABLE QML::CornerSetup GetCornerSetup();
    Q_INVOKABLE QML::CornerSetup GetCornerWeightedSetup();
    Q_INVOKABLE QML::RadioactiveWallSetup GetRadioactiveWallSetup();
    Q_INVOKABLE QML::CircleSetup GetCenterUnweightedSetup();
    Q_INVOKABLE QML::RectangleSetup GetRightHalfSetup();
    Q_INVOKABLE QML::RectangleSetup GetRightQuarterSetup();
    Q_INVOKABLE QML::RectangleSetup GetLeftEighthSetup();
    Q_INVOKABLE QML::NeighborCountSetup GetNeighborCountSetup();
    Q_INVOKABLE QML::CircleSetup GetAltruismSacrificeSetup();
    ///////////////////////////////////////////////////////////////////////////////
    
    ///////////////////////////////////////////////////////////////////////////////
    //! QML invokable gettes for barrier UI setups
    Q_INVOKABLE unsigned GetBarrierType() const;
    Q_INVOKABLE QML::CircleBarrierSetup GetCircleBarriers();
    Q_INVOKABLE QML::RectBarrierSetup GetRectBarriers();
    ///////////////////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////////////////////////
    //! QML invokable getters for various configs/details/analytics
    Q_INVOKABLE QVariantList GetSensorNames();
    Q_INVOKABLE QVariantList GetActionNames();
    Q_INVOKABLE void UpdateSensorsActions(const QVariantList& sensors, const QVariantList& actions);
    Q_INVOKABLE QVariantList GetChallengeNames() const;
    Q_INVOKABLE QVariantList GetSurvivors() const;
    Q_INVOKABLE QVariantList GetGeneticDiversity() const;
    Q_INVOKABLE QVariantList GetAnalyticsNames() const;
    Q_INVOKABLE void ClearAnalyticsProcessedCount();
    ///////////////////////////////////////////////////////////////////////////////
private:

    //! Enlarge the simulation data.
    const int cUiScaling = 6;

    QMutex      m_Mutex;
    QThread     m_WorkerThread;     ///< Worker thread.
    Backend*    m_pBackendWorker;   ///< Pointer to the backend worker
};

} // namespace QML