#include "Analytics.h"

#include <cassert>

//-------------------------------------------------------------------------
void Analytics::Clear()
{
    m_ProcessedSurvivors = 0;
    m_Survivors.clear();
    m_ProcessedGenDiversity = 0;
    m_GeneticDiversity.clear();
}

//-------------------------------------------------------------------------
std::pair<unsigned, std::vector<unsigned> > Analytics::GetSurvivors()
{
    assert(m_ProcessedSurvivors <= m_Survivors.size());
    std::vector<unsigned> range{};
    for (auto i = m_ProcessedSurvivors; i < m_Survivors.size(); ++i)
    {
        range.push_back(m_Survivors.at(i));
    }
    auto oldProcessedCount = m_ProcessedSurvivors;
    m_ProcessedSurvivors = m_Survivors.size();
    return {oldProcessedCount, range};
}

//-------------------------------------------------------------------------
std::vector<std::string> Analytics::GetAnalyticsNames()
{
    std::vector<std::string> names{};
    for(Analytics::eType a = Analytics::eType::Survivors; a < Analytics::eType::NoOfAnalytics; a = static_cast<Analytics::eType>((size_t)a + 1))
    {
      switch (a)
      {
      case Analytics::eType::Survivors:
        names.push_back("Survivors");
        break;
      case Analytics::eType::GeneticDiversity:
        names.push_back("Genetic Diversity");
        break;
      case Analytics::eType::NoOfAnalytics:
      default:
        break;
      }
    }
    return names;
}

//-------------------------------------------------------------------------
std::pair<unsigned, std::vector<float> > Analytics::GetGeneticDiversity()
{
    assert(m_ProcessedGenDiversity <= m_GeneticDiversity.size());
    std::vector<float> range{};
    for (auto i = m_ProcessedGenDiversity; i < m_GeneticDiversity.size(); ++i)
    {
        range.push_back(m_GeneticDiversity.at(i));
    }
    auto oldProcessedCount = m_ProcessedGenDiversity;
    m_ProcessedGenDiversity = m_GeneticDiversity.size();
    return {oldProcessedCount, range};
}