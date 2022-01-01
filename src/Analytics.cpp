#include "Analytics.h"

#include <cassert>

//-------------------------------------------------------------------------
void Analytics::Clear()
{
    ClearProcessedCounts();
    m_Survivors.clear();
    m_AvgAge.clear();
    m_GeneticDiversity.clear();
    m_SurvivorsToNextGen.clear();
    m_CompletedChallengeTasks.clear();
    m_ChallengeTaskCount = 0;
}

//-------------------------------------------------------------------------
void Analytics::ClearProcessedCounts()
{
    m_ProcessedSurvivors = 0;
    m_ProcessedGenDiversity = 0;
    m_ProcessedChallengeTasks = 0;
    m_ProcessedAvgAge = 0;
    m_ProcessedSurvNextGen = 0;
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
std::pair<unsigned, std::vector<unsigned> > Analytics::GetSurvivorsNextGen()
{
    assert(m_ProcessedSurvNextGen <= m_SurvivorsToNextGen.size());
    std::vector<unsigned> range{};
    for (auto i = m_ProcessedSurvNextGen; i < m_SurvivorsToNextGen.size(); ++i)
    {
        range.push_back(m_SurvivorsToNextGen.at(i));
    }
    auto oldProcessedCount = m_ProcessedSurvNextGen;
    m_ProcessedSurvNextGen = m_SurvivorsToNextGen.size();
    return {oldProcessedCount, range};
}

//-------------------------------------------------------------------------
std::pair<unsigned, std::vector<float> > Analytics::GetAvgAges()
{
    assert(m_ProcessedAvgAge <= m_AvgAge.size());
    std::vector<float> range{};
    for (auto i = m_ProcessedAvgAge; i < m_AvgAge.size(); ++i)
    {
        range.push_back(m_AvgAge.at(i));
    }
    auto oldProcessedCount = m_ProcessedAvgAge;
    m_ProcessedAvgAge = m_AvgAge.size();
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
      case Analytics::eType::AvgAge:
        names.push_back("Average age");
        break;
      case Analytics::eType::SurvivorToNextGen:
        names.push_back("Survivors to next generation");
        break;
      case Analytics::eType::CompletedTasks:
        names.push_back("Completed Tasks");
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

//-------------------------------------------------------------------------
void Analytics::AddCompletedChallengeTaskCounts(const std::vector<unsigned>& values) 
{   
    for(size_t i = 0; i < values.size(); ++i)
    {
      if (m_CompletedChallengeTasks.size() != values.size())
      {
          m_CompletedChallengeTasks.push_back(std::vector<unsigned>{values.at(i)});
          m_ChallengeTaskCount++;
      }
      else
      {
          m_CompletedChallengeTasks.at(i).push_back(values.at(i));
      }
    }
}

//-------------------------------------------------------------------------
std::pair<unsigned, std::vector<std::vector<unsigned> > > Analytics::GetCompletedChallengeTaskCounts()
{
    auto currentSize = m_ChallengeTaskCount > 0 ? m_CompletedChallengeTasks.at(0).size() : 0;
    assert(m_ProcessedChallengeTasks <= currentSize);
    std::vector<std::vector<unsigned> > range{};
    for (size_t i = 0; i < m_CompletedChallengeTasks.size(); ++i)
    {
        auto task = m_CompletedChallengeTasks.at(i);
        for(size_t j = m_ProcessedChallengeTasks; j < task.size(); ++j)
        {
            if (m_CompletedChallengeTasks.size() != range.size())
                range.push_back(std::vector<unsigned>{task.at(j)});
            else
                range.at(i).push_back(task.at(j));
        }
    }
    auto oldProcessedCount = m_ProcessedChallengeTasks;
    m_ProcessedChallengeTasks = currentSize;
    return {oldProcessedCount, range};
}