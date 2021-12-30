#pragma once

#include "Analytics.h"
#include "GenerationGenerator.h"
#include "Grid.h"
#include "Parameters.h"
#include "PeepsPool.h"
#include "PheromoneSignals.h"
#include "SysStateMachine.h"

#include <QMetaType>
#include <QObject>
#include <QReadWriteLock>
#include <QSize>
#include <QVariantList>

#include <memory>

class Sensors;
class Actions;

// This holds all data needed to construct one image frame. The data is
// cached in this structure so that the image writer can work on it in
// a separate thread while the main thread starts a new simstep.
struct WorldData {
    Q_GADGET
public:
    unsigned maxPopulation;
    unsigned simStep;
    unsigned generation;
    QVariantList peepsPositions{};
    QVariantList peepsColors{};
    QVariantList barrierLocs;
    typedef std::vector<std::vector<uint8_t>> SignalLayer;  // [x][y]
    std::vector<SignalLayer> signalLayers; // [layer][x][y]
    Q_PROPERTY(unsigned maxPopulation MEMBER maxPopulation)
    Q_PROPERTY(unsigned simStep MEMBER simStep)
    Q_PROPERTY(unsigned generation MEMBER generation)
    Q_PROPERTY(QVariantList peepsPositions MEMBER peepsPositions)
    Q_PROPERTY(QVariantList barrierLocs MEMBER barrierLocs)
    Q_PROPERTY(QVariantList peepsColors MEMBER peepsColors)
};

class Backend : public QObject
{
    Q_OBJECT
public:

    Backend();

    //! Returns the world size.
    std::pair<uint16_t, uint16_t> GetFrameSize() const;
    //! Stops the work.
    Q_INVOKABLE void StopThread() { m_ThreadStop = true; }
    //! Returns the current challenge id.
    eChallenges GetChallengeId() const;
    //! Set challenge id.
    void SetChallengeId(unsigned id);
    //! Returns the current challenge.
    Challenges::iChallenge* GetChallenge() const;
    //! Returns the barrier type.
    eBarrierType GetBarrierType() const { return m_BarrierType; };
    //! Returns the barrier vector.
    const std::vector<std::unique_ptr<Barriers::iBarrier> >& GetBarriers() const;
    //! Returns the sensor names.
    std::vector<std::string> GetSensorNames() const;
    //! Returns the action names.
    std::vector<std::string> GetActionNames() const;
    //! Updates the sensors actions vectors.
    void UpdateSensorsActions(const QVariantList& sensors, const QVariantList& actions);
    //! Starts simulation.
    void StartSim();
    //! Stops simulation.
    void StopSim();
    //! Reset simulation.
    void ResetSim();
    //! Returns all challenge names.
    std::vector<std::string> GetChallengeNames() const;
    //! Returns the vector of survivors not sent out yet alongside the last processed index.
    std::pair<unsigned, std::vector<unsigned> > GetSurvivors() const;
    //! Returns the vector of genetic diversities not sent out yet alongside the last processed index.
    std::pair<unsigned, std::vector<float> > GetGeneticDiversity() const;
    //! Returns the available analytics types.
    std::vector<std::string> GetAnalyticsTypes() const;
    //! Clears all processed counts in analytics.
    void ClearAnalyticsProcessedCount();

signals:
    //! Signals when the parameters are read and processed.
    void ParametersUpdated();

public slots:
    //! Main work load of the class
    void Run();

    //! Returns the world data.
    WorldData GetWorldData();

private:
    /**********************************************************************************************
    Execute one simStep for one individual.

    This executes in its own thread, invoked from the main simulator thread. First we execute
    indiv.feedForward() which computes action values to be executed here. Some actions such as
    signal emission(s) (pheromones), agent movement, or deaths will have been queued for
    later execution at the end of the generation in single-threaded mode (the deferred queues
    allow the main data structures (e.g., grid, signals) to be freely accessed read-only in all threads).

    In order to be thread-safe, the main simulator-wide data structures and their
    accessibility are:

        grid - read-only
        signals - (pheromones) read-write for the location where our agent lives
            using signals.increment(), read-only for other locations
        peeps - for other peeps, we can only read their index and genome.
            We have read-write access to our individual through the indiv argument.

    The other important variables are:

        simStep - the current age of our agent, reset to 0 at the start of each generation.
            For many simulation scenarios, this matches our indiv.age member.
        randomUint - global random number generator, a private instance is given to each thread
    **********************************************************************************************/
    void SimStepOnePeep(Peep &indiv, unsigned simStep, RandomUintGenerator& random);
    //! Checks parameters that are neccessary to run the simulation. If any of the conditions are not met
    //! The simulation goes to a halt.
    bool CheckParameters();
    //! Reset all simulation data
    void Reset();

    /*
    At the end of each sim step, this function is called in single-thread
    mode to take care of several things:

    1. We may kill off some agents if a "radioactive" scenario is in progress.
    2. We may flag some agents as meeting some challenge criteria, if such
      a scenario is in progress.
    3. We then drain the deferred death queue.
    4. We then drain the deferred movement queue.
    5. We fade the signal layer(s) (pheromones).
    6. We save the resulting world condition as a single image frame (if
      p.saveVideo is true).
    */
    void endOfSimStep(unsigned simStep, unsigned generation);

    //! At the end of each generation, we save a video file (if p.saveVideo is true) and
    //! print some genomic statistics to stdout (if p.updateGraphLog is true).
    void endOfGeneration(unsigned generation);

    // Synchronous version, always returns true
    void saveVideoFrameSync(unsigned simStep, unsigned generation);

    QReadWriteLock                                    m_Lock;
    bool                                              m_ThreadStop{false};  ///!< When set to true stop the work.
    WorldData                                         m_WorldData{};        ///< Contains the world data for the current sim step.
                                                                            ///< Processed by an external thread (for example UI)

    std::unique_ptr<RandomUintGenerator>              m_xRandomGenerator{}; ///< Random number generator
    std::unique_ptr<ParameterIO>                      m_xParameterIO{};     ///< Parameter IO handler
    std::unique_ptr<Grid>                             m_xGrid{};            ///< World grid manager
    std::unique_ptr<PheromoneSignals>                 m_xSignals{};         ///< Pheromon signal manager
    std::unique_ptr<Sensors>                          m_xSensors{};         ///< Sensors manager
    std::unique_ptr<PeepsPool>                        m_xPeeps{};           ///< Peeps life cycle manager
    std::unique_ptr<Actions>                          m_xActions{};         ///< Peep actions manager
    std::unique_ptr<Challenges::iChallenge>           m_xChallenge{};       ///< Holds the current challenge
    std::unique_ptr<Analytics>                        m_xAnalytics{};       ///< Analytics manager
    std::unique_ptr<GenerationGenerator>              m_xGenerationGenerator{};                     ///< Handles generation evaluation and regeneration
    std::unique_ptr<SysStateMachine>                  m_xSysStateMachine{};                         ///< System state machine
    Analytics::eType                                  m_AnalyticsType{Analytics::eType::Survivors}; ///< Holds the current active analytics type
    eChallenges                                       m_CurrentChallenge{eChallenges::Altruism};    ///< Holds the current active challenge type

    eBarrierType                                      m_BarrierType{eBarrierType::NoBarrier};       ///< Holds the current active barrier type
    std::vector<std::unique_ptr<Barriers::iBarrier> > m_Barriers{};                                 ///< Holds the current barriers
    unsigned                                          m_Generation{0};                              ///< Stores the generation count
    unsigned                                          m_SimStep{0};                                 ///< Stores the simulation step count.
    
};