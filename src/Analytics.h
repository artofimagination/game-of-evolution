#pragma once

#include <cstdint>
#include <string>
#include <vector>


/*! \class Analytics
    \brief Calculates and stores statistical/analytical data.
*/
class Analytics
{
public:
    //! Analytics types.
    enum class eType : uint8_t {
        Survivors,        ///< Stores the survivor count each generation
        GeneticDiversity, ///< Stores the genetic diversity in each generation.
        CompletedTasks,   ///< Stores the completeing peeps counts for each challenge task.
        AvgAge,           ///< Stores the average age of the generation.
        SurvivorToNextGen,///< Stores the count of survivors that will respawn in the next gen.
        NoOfAnalytics
    };
    
    //! Returns the survivor counts. It only sends the vector of counts, that has not been sent out yet.
    //! \a m_ProcessedSurvivors keeps count of processed values.
    //! Returns the pair of the processed index and the vector of data.
    std::pair<unsigned, std::vector<unsigned> > GetSurvivors();
    //! Adds a new survivor count to the vector.
    void AddSurvivorCount(unsigned value) { m_Survivors.push_back(value); }
    //! Returns the survivor to next gen counts. It only sends the vector of counts, that has not been sent out yet.
    //! \a m_ProcessedSurvNextGen keeps count of processed values.
    //! Returns the pair of the processed index and the vector of data.
    std::pair<unsigned, std::vector<unsigned> > GetSurvivorsNextGen();
    //! Adds a new survivor count to the vector.
    void AddSurvivorNextGenCount(unsigned value) { m_SurvivorsToNextGen.push_back(value); }
    //! Returns the avg ages. It only sends the vector of counts, that has not been sent out yet.
    //! \a m_ProcessedAvgAge keeps count of processed values.
    //! Returns the pair of the processed index and the vector of data.
    std::pair<unsigned, std::vector<float> > GetAvgAges();
    //! Adds a new avg age to the vector.
    void AddAvgAge(float value) { m_AvgAge.push_back(value); }
    //! Returns the genetic diversity counts. It only sends the vector of counts, that has not been sent out yet.
    //! \a m_ProcessedGenDiversity keeps count of processed values.
    //! Returns the pair of the processed index and the vector of data.
    std::pair<unsigned, std::vector<float> > GetGeneticDiversity();
    //! Adds a new genetic diversity count to the vector.
    void AddGenDiveristyCount(float value) { m_GeneticDiversity.push_back(value); }
    //! Returns the successful peeps' count for each challenge task. It only sends the vector of counts, that has not been sent out yet.
    //! \a m_ProcessedChallengeTasks keeps count of processed values.
    //! Returns the pair of the processed index and the vector of data.
    std::pair<unsigned, std::vector<std::vector<unsigned> > > GetCompletedChallengeTaskCounts();
    //! Adds a count of successfull peeps for each task.
    void AddCompletedChallengeTaskCounts(const std::vector<unsigned>& values);
    //! Returns the analytics type names.
    static std::vector<std::string> GetAnalyticsNames();
    //! Resets all statistics. Should only happen on "Reset" system request or on Challenge change.
    void Clear();
    //! Clears only the processed counts. Useful when analyticstype is changed.
    void ClearProcessedCounts();
private:
    std::vector<unsigned> m_Survivors{};      ///< Contains the survivor count of each generation.
    unsigned m_ProcessedSurvivors{};          ///< Contains the index of the last polled survivor count.
    std::vector<float> m_GeneticDiversity{};  ///< Contains the genetic diversity count of each generation.
    unsigned m_ProcessedGenDiversity{};       ///< Contains the index of the last polled genetic diversity count.

    std::vector<std::vector<unsigned> > m_CompletedChallengeTasks{};  ///< Contains how many peeps completed each challenge task in each generation
    unsigned m_ProcessedChallengeTasks{};     ///< Contains the index of the last polled completed challenge task peep count.
    unsigned m_ChallengeTaskCount{};          ///< Contains the count of challenge tasks.
    std::vector<float> m_AvgAge{};            ///< Contains the avg age by the end of the generation.
                                              ///< Only valuable if surviveToNextGeneration is enabled.
    unsigned m_ProcessedAvgAge{};             ///< Contains the count of challenge tasks.
    std::vector<unsigned> m_SurvivorsToNextGen{}; ///< Contains the survivors count that will be respawn in the next generation
    unsigned m_ProcessedSurvNextGen{};        ///< Contains the count of challenge tasks.
};

