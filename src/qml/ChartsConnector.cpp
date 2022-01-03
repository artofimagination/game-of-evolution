#include "ChartsConnector.h"

#include <QtCharts/QXYSeries> 

#include <QThread>

QT_CHARTS_USE_NAMESPACE

namespace QML
{

//-----------------------------------------------------------------------------
void ChartsConnector::AddInput(QAbstractSeries* pSeries)
{ 
    QMutexLocker locker(m_pMutex);
    m_SeriesPtrMap.emplace(std::pair<int, QAbstractSeries*>(m_Data.size(), pSeries));
    m_Data.append(QVector<QPointF>());
};

//-----------------------------------------------------------------------------
void ChartsConnector::RemoveAllSeries()
{
    QMutexLocker locker(m_pMutex);
    m_Data.clear();
    for (auto& series : m_SeriesPtrMap)
    {
        auto axes = static_cast<QXYSeries*>(series.second)->attachedAxes();
        for (const auto& axis : axes)
        {
            static_cast<QXYSeries*>(series.second)->detachAxis(axis);
        }
        static_cast<QXYSeries*>(series.second)->clear();
    }
    m_SeriesPtrMap.clear();
    m_MinX = { std::numeric_limits<double>::max() };
    m_MaxX = { std::numeric_limits<double>::min() };
    m_MinY1 = { std::numeric_limits<double>::max() };
    m_MaxY1 = { std::numeric_limits<double>::min() };
    m_MinY2 = { std::numeric_limits<double>::max() };
    m_MaxY2 = { std::numeric_limits<double>::min() };
}

//-----------------------------------------------------------------------------
void ChartsConnector::ChangeMargin(double max, double min)
{
    m_MinMargin = min;
    m_MaxMargin = max;
}

//-----------------------------------------------------------------------------
void ChartsConnector::UpdateSeries()
{
    {
        QMutexLocker locker(m_pMutex);
        assert(static_cast<int>(m_SeriesPtrMap.size()) == m_Data.size());
        int graphIndex = 0;
        for (auto it = m_Data.begin(); it != m_Data.end(); ++it)
        {
            //if (m_SeriesPtrMap.empty() == false && m_Data.isEmpty() == false)
            {
                QVector<QPointF> points = *it;
                // Use replace instead of clear + append, it's optimized for performance
                auto pSeries = static_cast<QXYSeries*>(m_SeriesPtrMap[graphIndex]);
                pSeries->replace(points);
                auto axes = pSeries->attachedAxes();
                axes[0]->setMin(m_MinX);
                axes[0]->setMax(m_MaxX);
                axes[1]->setMin(m_MinY1 - m_MinMargin);
                axes[1]->setMax(m_MaxY1 + m_MaxMargin);
                graphIndex++;
            }
        }
    }
    emit ValueChanged();
}

//-----------------------------------------------------------------------------
void ChartsConnector::UpdateHistory(const QVariantList& newData)
{
    QMutexLocker locker(m_pMutex);
    int graphIndex = 0;
    for (auto it = m_Data.begin(); it != m_Data.end() && graphIndex < newData.size(); ++it)
    {
        auto lineGraph = newData.at(graphIndex).toList();
        // It is preferable to send smaller set of data, in order to reduce the length of this loop.
        for(int dataIndex = 0; dataIndex < lineGraph.size(); ++dataIndex)
        {
            auto x = lineGraph.at(dataIndex).toPointF().x();
            auto y = lineGraph.at(dataIndex).toPointF().y();
            if (x < m_MinX)
            {   
                // Chart doesn't draw properly if the min x exactly equals to the smallest x value.
                m_MinX = x - 0.00001;
            }

            if (x > m_MaxX)
            {
                m_MaxX = x;
            }

            if (y < m_MinY1)
            {
                m_MinY1 = y;
            }

            if (y > m_MaxY1)
            {
                m_MaxY1 = y;
            }
            m_MinMargin = (m_MaxY1 - m_MinY1) / 2.0;
            m_MaxMargin = m_MinMargin;
            it->append(lineGraph.at(dataIndex).toPointF());
        }
        ++graphIndex;
    }
}

} // namespace QML