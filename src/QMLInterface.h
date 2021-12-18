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
    Q_PROPERTY(ImageFrameData imageData READ GetImageFrameData)
public:

    //! Starts the backend thread.
    Q_INVOKABLE QObject* StartBackend();

    Q_INVOKABLE int UiScale() { return cUiScaling; }

    Q_INVOKABLE void Quit();
    Q_INVOKABLE unsigned GetChallengeId();
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
    Q_INVOKABLE ImageFrameData GetImageFrameData();
private:

    //! Enlarge the simulation data.
    const int cUiScaling = 6;

    QMutex      m_Mutex;
    QThread     m_WorkerThread;
    Backend*    m_pBackendWorker;
};

} // namespace QML