#pragma once

#include "Grid.h"
#include "Parameters.h"
#include "PeepsPool.h"
#include "PheromoneSignals.h"
#include "GenerationGenerator.h"

#include <QMetaType>
#include <QObject>
#include <QReadWriteLock>
#include <QSize>
#include <QVariantList>

#include <memory>

// This holds all data needed to construct one image frame. The data is
// cached in this structure so that the image writer can work on it in
// a separate thread while the main thread starts a new simstep.
struct ImageFrameData {
    Q_GADGET
public:
    unsigned maxPopulation;
    unsigned simStep;
    unsigned generation;
    QVariantList peepsPositions{};
    QVariantList peepsColors{};
    std::vector<Coord> barrierLocs;
    typedef std::vector<std::vector<uint8_t>> SignalLayer;  // [x][y]
    std::vector<SignalLayer> signalLayers; // [layer][x][y]
    Q_PROPERTY(unsigned maxPopulation MEMBER maxPopulation)
    Q_PROPERTY(unsigned simStep MEMBER simStep)
    Q_PROPERTY(unsigned generation MEMBER generation)
    Q_PROPERTY(QVariantList peepsPositions MEMBER peepsPositions)
    Q_PROPERTY(QVariantList peepsColors MEMBER peepsColors)
};

class Backend : public QObject
{
    Q_OBJECT
public:

    Backend();

    Q_INVOKABLE QSize GetFrameSize() { return QSize(m_xParameterIO->GetParamRef().sizeX, m_xParameterIO->GetParamRef().sizeY); };

    //! Stops the work.
    Q_INVOKABLE void Stop() { m_Stop = true; }

    //! Returns the current challenge id.
    eChallenges GetChallengeId() const;

    //! Returns the current challenge.
    Challenges::iChallenge* GetChallenge() const;

signals:
    void ParametersUpdated();

public slots:
    void Run();

    Q_INVOKABLE ImageFrameData GetImageFrameData();

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

    /**********************************************************************************
    Action levels are driven by sensors or internal neurons as connected by an agent's
    neural net brain. Each agent's neural net is reevaluated once each simulator
    step (simStep). After evaluating the action neuron outputs, this function is
    called to execute the actions according to their output levels. This function is
    called in multi-threaded mode and operates on a single individual while other
    threads are doing to the same to other peeps.

    Action (their output) values arrive here as floating point values of arbitrary
    range (because they are the raw sums of zero or more weighted inputs) and will
    eventually be converted in this function to a probability 0.0..1.0 of actually
    getting executed.

    For the various possible action neurons, if they are driven by a sufficiently
    strong level, we do this:

        MOVE_* actions- queue our agent for deferred movement with peeps.queueForMove(); the
            queue will be executed at the end of the multithreaded loop in a single thread.
        SET_RESPONSIVENESS action - immediately change indiv.responsiveness to the action
            level scaled to 0.0..1.0 (because we have exclusive access to this member in
            our own individual during this function)
        SET_OSCILLATOR_PERIOD action - immediately change our individual's indiv.oscPeriod
            to the action level exponentially scaled to 2..2048 (TBD)
        EMIT_SIGNALn action(s) - immediately increment the signal level at our agent's
            location using signals.increment() (using a thread-safe call)
        KILL_FORWARD action - queue the other agent for deferred death with
            peeps.queueForDeath()

    The deferred movement and death queues will be emptied by the caller at the end of the
    simulator step by endOfSimStep() in a single thread after all peeps have been
    evaluated multithreadedly.
    **********************************************************************************/
    void executeActions(Peep &peep, std::array<float, SensorsActions::Action::NUM_ACTIONS> &actionLevels);

    // Synchronous version, always returns true
    void saveVideoFrameSync(unsigned simStep, unsigned generation);

    QReadWriteLock m_Lock;
    bool m_Stop{false};  ///!< When set to true stop the work.
    ImageFrameData m_UIFrameData{};

    std::unique_ptr<RandomUintGenerator> m_xRandomGenerator{};
    std::unique_ptr<ParameterIO> m_xParameterIO{};
    std::unique_ptr<Grid> m_xGrid{};
    std::unique_ptr<PheromoneSignals> m_xSignals{};
    std::unique_ptr<PeepsPool> m_xPeeps{};
    std::unique_ptr<GenerationGenerator> m_xGenerationGenerator{};
};