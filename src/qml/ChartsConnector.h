#pragma once

#include <QtCharts/QAbstractSeries>

#include <QObject>
#include <QPointF>
#include <QList>
#include <QVector>
#include <QMutex>

#include <map>
#include <limits>

QT_CHARTS_USE_NAMESPACE

namespace QML
{

/*! \class ChartsConnector
    \brief Connector to Charts UI in order to execute generic chart back end (updating series)
*/
class ChartsConnector 
    : public QObject
{
    Q_OBJECT
    Q_PROPERTY(double minX READ MinX NOTIFY ValueChanged)
    Q_PROPERTY(double maxX READ MaxX NOTIFY ValueChanged)
    Q_PROPERTY(double minY1 READ MinY1 NOTIFY ValueChanged)
    Q_PROPERTY(double maxY1 READ MaxY1 NOTIFY ValueChanged)
    Q_PROPERTY(double minY2 READ MinY2 NOTIFY ValueChanged)
    Q_PROPERTY(double maxY2 READ MaxY2 NOTIFY ValueChanged)
public:
    //! Constructor
    explicit ChartsConnector(QObject* pParent = nullptr)
        : QObject(pParent)
    {}

    //! Updates series.
    Q_INVOKABLE void UpdateSeries();

    //! Returns x axis min
    double MinX() const { return m_MinX; }
    //! Returns x axis max
    double MaxX() const { return m_MaxX; }
    //! Returns y left axis min
    double MinY1() const { return m_MinY1; }
    //! Returns y left axis max
    double MaxY1() const { return m_MaxY1; }
    //! Returns y right axis min
    double MinY2() const { return m_MinY2; }
    //! Returns y right axis max
    double MaxY2() const { return m_MaxY2; }

public slots:
    //! Updates data sets and min/max that are used to update series. Series and history updates have different timings.
    Q_INVOKABLE void UpdateHistory(const QVariantList& newData);
    //! Adds new series to the chart.
    Q_INVOKABLE void AddInput(QAbstractSeries* pSeries);
    //! Cleans all chart data.
    Q_INVOKABLE void RemoveAllSeries();
    //! Updates margin.
    Q_INVOKABLE void ChangeMargin(double max, double min);

signals:
    //! Signals series value change
    void ValueChanged();
    //! Signals x axis min change.
    void MinXChanged();
    //! Signals x axis max change.
    void MaxXChanged();
    //! Signals y left axis min change.
    void MinY1Changed();
    //! Signals y left max change.
    void MaxY1Changed();
    //! Signals y right min change.
    void MinY2Changed();
    //! Signals y right max change.
    void MaxY2Changed();

private:
    QMutex*                             m_pMutex{ new QMutex() };
    QList<QVector<QPointF> >            m_Data{};                                       ///< Data history container.
    int                                 m_Index{};                                      ///< Series update index.
    std::map<int, QAbstractSeries*>     m_SeriesPtrMap{};                               ///< Series index pointer map.

    double                              m_MinMargin{ 200.0 };                           ///< Margin on the bottom of the graph 
                                                                                        ///< (how many y values are shown below the min y value)
    double                              m_MaxMargin{ 200.0 };                           ///< Margin on the top of the graph 
                                                                                        ///< (how many y values are shown over the max y value)
    double                              m_MinX{ std::numeric_limits<double>::max() };   ///< X axis minimum
    double                              m_MaxX{ std::numeric_limits<double>::min() };   ///< X axis maximum
    double                              m_MinY1{ std::numeric_limits<double>::max() };  ///< Y left axis minimum
    double                              m_MaxY1{ std::numeric_limits<double>::min() };  ///< Y left axis maximum
    double                              m_MinY2{ std::numeric_limits<double>::max() };  ///< Y right axis minimum
    double                              m_MaxY2{ std::numeric_limits<double>::min() };  ///< Y right axis maximum
};

} // namespace QML