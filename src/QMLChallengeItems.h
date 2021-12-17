#pragma once

#include "Challenges/iChallenges.h"

#include <QColor>
#include <QMetaType>
#include <QPoint>
#include <QObject>
#include <QRect>

namespace QML
{

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
        EastWestEights,
        NearBarrier,
        Pairs,
        LocationSequence,
        Altruism,
        AltruismSacrifice,
        NoOfChallenges,
    };
    Q_ENUM(Value)
    static_assert(static_cast<unsigned>(QML::Challenge::Value::NoOfChallenges) == static_cast<unsigned>(eChallenges::NoOfChallenges));

    // explicit Challenge() = default;
};

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

struct NeighborCountSetup
{
    Q_GADGET
public:
    QVariantList borders{};
    QColor color{};
    Q_PROPERTY(QVariantList neighborCountBorders MEMBER borders)
    Q_PROPERTY(QColor neighborCountColor MEMBER color)
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

struct CornerSetup
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

} // namespace QML
