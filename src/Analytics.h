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
        NoOfAnalytics
    };
    
    //! Returns the survivor counts. It only sends the vector of counts, that has not been sent out yet.
    //! \a m_ProcessedSurvivors keeps count of processed values.
    //! Returns the pair of the processed index and the vector of data.
    std::pair<unsigned, std::vector<unsigned> > GetSurvivors();
    //! Adds a new survivor count to the vector.
    void AddSurvivorCount(unsigned value) { m_Survivors.push_back(value); }
    //! Returns the genetic diversity counts. It only sends the vector of counts, that has not been sent out yet.
    //! \a m_ProcessedGenDiversity keeps count of processed values.
    //! Returns the pair of the processed index and the vector of data.
    std::pair<unsigned, std::vector<float> > GetGeneticDiversity();
    //! Adds a new genetic diversity count to the vector.
    void AddGenDiveristyCount(float value) { m_GeneticDiversity.push_back(value); }
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
};

