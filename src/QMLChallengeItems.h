#pragma once

#include "Analytics.h"
#include "Barriers/iBarriers.h"
#include "Challenges/iChallenges.h"

#include <QColor>
#include <QMetaType>
#include <QPoint>
#include <QObject>
#include <QRect>

namespace QML
{

//! QML enums for challenges, it is always identical to the corresponding backend enum
//! If not a compile time asert wil happen
class Challenge
{
    Q_GADGET
public:

    enum class Value : uint16_t
    {
        Circle,
        RightHalf,
        RightQuarter,
        NeighborCount,
        CenterWeighted,
        CenterUnweighted,
        Corner,
        CornerWeighted,
        MigrateDistance,
        CenterSparsed,
        LeftEighth,
        RadioActiveWalls,
        AgainstAnyWall,
        TouchAnyWall,
        EastWestEighths,
        NearBarrier,
        Pairs,
        LocationSequence,
        Altruism,
        AltruismSacrifice,
        CircularSequence,
        NoOfChallenges,
    };
    Q_ENUM(Value)
    static_assert(static_cast<unsigned>(QML::Challenge::Value::NoOfChallenges) == static_cast<unsigned>(eChallenges::NoOfChallenges));
};

//! QML enums for barriers, it is always identical to the corresponding backend enum
//! If not a compile time asert wil happen
class Barrier
{
    Q_GADGET
public:
    enum class Value : uint8_t
    {
        NoBarrier,
        VerticalBarConstantLoc,
        VerticalBarRandomLoc,
        FiveBlocksStaggered,
        HorizontalBarConstantLoc,
        ThreeFloatingIslands,
        SpotsSpecified,
        NoOfTypes
    };
    Q_ENUM(Value)
    static_assert(static_cast<unsigned>(QML::Barrier::Value::NoOfTypes) == static_cast<unsigned>(eBarrierType::NoOfTypes));
};

//! QML enums for analytics types, it is always identical to the corresponding backend enum
//! If not a compile time asert wil happen
class AnalyticsTypes
{
    Q_GADGET
public:
    enum class Value : uint8_t
    {
        Survivors,
        GeneticDiversity,
        NoOfAnalytics
    };
    Q_ENUM(Value)
    static_assert(static_cast<unsigned>(QML::AnalyticsTypes::Value::NoOfAnalytics) == static_cast<unsigned>(Analytics::eType::NoOfAnalytics));
};

///////////////////////////////////////////////////////////////////////////////
//! QML compatible structures for challenges and barriers
struct AltruismSetup
{
    Q_GADGET
public:
    QPoint altruismCenter{};
    float altruismRadius{};
    QColor altruismColor{};
    QPoint sacrificeCenter{};
    float sacrificeRadius{};
    QColor sacrificeColor{};
    Q_PROPERTY(QPoint altruismCenter MEMBER altruismCenter)
    Q_PROPERTY(float altruismRadius MEMBER altruismRadius)
    Q_PROPERTY(QColor altruismColor MEMBER altruismColor)
    Q_PROPERTY(QPoint sacrificeCenter MEMBER sacrificeCenter)
    Q_PROPERTY(float sacrificeRadius MEMBER sacrificeRadius)
    Q_PROPERTY(QColor sacrificeColor MEMBER sacrificeColor)
};

struct CircleSetup
{
    Q_GADGET
public:
    QPoint center{};
    float radius{};
    QColor color{};
    Q_PROPERTY(QPoint center MEMBER center)
    Q_PROPERTY(float radius MEMBER radius)
    Q_PROPERTY(QColor color MEMBER color)
};

struct RectangleSetup
{
    Q_GADGET
public:
    QRect rect{};
    QColor color{};
    Q_PROPERTY(QRect rect MEMBER rect)
    Q_PROPERTY(QColor rectColor MEMBER color)
};

struct DoubleRectangleSetup
{
    Q_GADGET
public:
    QRect rectLeft{};
    QRect rectRight{};
    QColor color{};
    Q_PROPERTY(QRect rectLeft MEMBER rectLeft)
    Q_PROPERTY(QColor doubleRectColor MEMBER color)
    Q_PROPERTY(QRect rectRight MEMBER rectRight)
};

struct NeighborCountSetup
{
    Q_GADGET
public:
    QVariantList borders{};
    QColor color{};
    Q_PROPERTY(QVariantList neighborCountBorders MEMBER borders)
    Q_PROPERTY(QColor neighborCountColor MEMBER color)
};

struct PairsSetup
{
    Q_GADGET
public:
    QVariantList borders{};
    QColor color{};
    Q_PROPERTY(QVariantList pairsBorders MEMBER borders)
    Q_PROPERTY(QColor pairsColor MEMBER color)
};

struct AnyWallSetup
{
    Q_GADGET
public:
    QVariantList borders{};
    QColor color{};
    Q_PROPERTY(QVariantList anyWallBorders MEMBER borders)
    Q_PROPERTY(QColor anyWallColor MEMBER color)
};

struct CenterSparsedSetup
{
    Q_GADGET
public:
    QPoint center{};
    float radius{};
    QColor color{};
    Q_PROPERTY(QPoint centerSparsedCenter MEMBER center)
    Q_PROPERTY(float centerSparsedRadius MEMBER radius)
    Q_PROPERTY(QColor centerSparsedColor MEMBER color)
};

struct MultiCircleSetup
{
    Q_GADGET
public:
    QVariantList centers{};
    float radius{};
    QColor color{};
    Q_PROPERTY(QVariantList cornerCenters MEMBER centers)
    Q_PROPERTY(float cornerRadius MEMBER radius)
    Q_PROPERTY(QColor cornerColor MEMBER color)
};

struct RadioactiveWallSetup
{
    Q_GADGET
public:
    unsigned border{};
    unsigned distance{};
    QColor color{};
    Q_PROPERTY(unsigned border MEMBER border)
    Q_PROPERTY(unsigned distance MEMBER distance)
    Q_PROPERTY(QColor radioactiveColor MEMBER color)
};

struct RectBarrierSetup
{
    Q_GADGET
public:
    QVariantList barriers{};
    QColor color{};
    Q_PROPERTY(QVariantList rectBarriers MEMBER barriers)
    Q_PROPERTY(QColor rectBarrierColor MEMBER color)
};

struct CircleBarrierSetup
{
    Q_GADGET
public:
    QVariantList barriers{};
    float radius{};
    QColor color{};
    Q_PROPERTY(QVariantList circleBarriers MEMBER barriers)
    Q_PROPERTY(float circleBarrierRadius MEMBER radius)
    Q_PROPERTY(QColor circleBarrierColor MEMBER color)
};
///////////////////////////////////////////////////////////////////////////////

} // namespace QML
