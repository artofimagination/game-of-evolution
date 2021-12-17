#include "Backend.h"

#include "AlgorithmHelpers.h"
#include "BasicTypes.h"
#include "Challenges/iChallenges.h"
#include "SensorsActions.h"

#include <QColor>
#include <QPoint>


#include <iostream>


//---------------------------------------------------------------------------
Backend::Backend()
  : m_xRandomGenerator(std::make_unique<RandomUintGenerator>())
  , m_xParameterIO(std::make_unique<ParameterIO>())
  , m_xGrid(std::make_unique<Grid>(m_xParameterIO->GetParamRef(), *m_xRandomGenerator.get()))
  , m_xSignals(std::make_unique<PheromoneSignals>(m_xParameterIO->GetParamRef()))
  , m_xPeeps(std::make_unique<PeepsPool>(*m_xGrid.get()))
  , m_xGenerationGenerator(std::make_unique<GenerationGenerator>(
      *m_xGrid.get(), 
      *m_xPeeps.get(), 
      *m_xSignals.get(), 
      m_xParameterIO->GetParamRef(),
      *m_xRandomGenerator.get()))
{
    qRegisterMetaType<ImageFrameData>("ImageFrameData");

}

//---------------------------------------------------------------------------
void Backend::Run()
{
    // Initialize config parameters.
    m_xParameterIO->SetDefaults();
    m_xParameterIO->ReadFromConfigFile(cDefaultFilename);
    auto parameters = m_xParameterIO->GetParamRef();
    emit ParametersUpdated();

    // Allocate container space. Once allocated, these container elements
    // will be reused in each new generation.
    m_xGrid->init(); // the land on which the peeps live
    m_xSignals->init(parameters.signalLayers, parameters.sizeX, parameters.sizeY);  // where the pheromones waft
    m_xPeeps->init(parameters.population, parameters); // the peeps themselves

    unsigned generation = 0;
    m_xGenerationGenerator->initializeGeneration0(); // starting population
    m_xGenerationGenerator->SetStartChallenge(static_cast<eChallenges>(parameters.challenge));

    while (!m_Stop)
    {
        m_xGenerationGenerator->SetChallenge(static_cast<eChallenges>(parameters.challenge));
        while (!m_Stop && generation < parameters.maxGenerations) { // generation loop
            unsigned murderCount = 0; // for reporting purposes
            for (unsigned simStep = 0; simStep < parameters.stepsPerGeneration; ++simStep) {

                // multithreaded loop: index 0 is reserved, start at 1
                auto& randomUint = *m_xRandomGenerator.get();
    #pragma omp parallel for num_threads(parameters.numThreads) default(shared) firstprivate(randomUint) lastprivate(randomUint) schedule(auto)
                for (unsigned peepIndex = 1; peepIndex <= parameters.population; ++peepIndex) {
                    if ((*m_xPeeps.get())[peepIndex].alive) {
                        SimStepOnePeep((*m_xPeeps.get())[peepIndex], simStep, randomUint);
                    }
                }
                // In single-thread mode: this executes deferred, queued deaths and movements,
                // updates signal layers (pheromone), etc.
                murderCount += m_xPeeps->deathQueueSize();
                endOfSimStep(simStep, generation);
            }

            endOfGeneration(generation);
            unsigned numberSurvivors = m_xGenerationGenerator->spawnNewGeneration(generation, murderCount);
            if (numberSurvivors > 0 && (generation % parameters.genomeAnalysisStride == 0)) {
                Genetics::displaySampleGenomes(parameters.displaySampleGenomes, *m_xPeeps.get(), parameters);
            }
            if (numberSurvivors == 0) {
                generation = 0;  // start over
            } else {
                ++generation;
            }
        }
        Genetics::displaySampleGenomes(3, *m_xPeeps.get(), parameters); // final report, for debugging
    }
}

//---------------------------------------------------------------------------
void Backend::SimStepOnePeep(Peep &peep, unsigned simStep, RandomUintGenerator& random)
{
    ++peep.age; // for this implementation, tracks simStep
    auto actionLevels = peep.feedForward(simStep, *m_xPeeps.get(), *m_xSignals.get(), random);
    executeActions(peep, actionLevels);
}

//---------------------------------------------------------------------------
void Backend::endOfSimStep(unsigned simStep, unsigned generation)
{
    auto params = m_xParameterIO->GetParamRef();
    auto challenge = m_xGenerationGenerator->GetChallengeId();
    auto pChallenge = m_xGenerationGenerator->GetChallenge();
    auto settings = Challenges::Settings();
    settings.simStep = simStep;
    pChallenge->EvaluateAtEndOfSimStep(*m_xPeeps.get(), m_xParameterIO->GetParamRef(), *m_xGrid.get(), settings);

    // If the peep is touching any wall, we set its challengeFlag to true.
    // At the end of the generation, all those with the flag true will reproduce.
    if (challenge == eChallenges::TouchAnyWall) {
        for (uint16_t index = 1; index <= params.population; ++index) { // index 0 is reserved
            Peep &peep = (*m_xPeeps.get())[index];
            if (peep.loc.x == 0 || peep.loc.x == params.sizeX - 1
             || peep.loc.y == 0 || peep.loc.y == params.sizeY - 1) {
                peep.challengeBits = true;
            }
        }
    }

    // If this challenge is enabled, the peep gets a bit set in their challengeBits
    // member if they are within a specified radius of a barrier center. They have to
    // visit the barriers in sequential order.
    if (challenge == eChallenges::LocationSequence) {
        float radius = 9.0;
        for (uint16_t index = 1; index <= params.population; ++index) { // index 0 is reserved
            Peep &peep = (*m_xPeeps.get())[index];
            for (unsigned n = 0; n < m_xGrid->getBarrierCenters().size(); ++n) {
                unsigned bit = 1 << n;
                if ((peep.challengeBits & bit) == 0) {
                    if ((peep.loc - m_xGrid->getBarrierCenters()[n]).length() <= radius) {
                        peep.challengeBits |= bit;
                    }
                    break;
                }
            }
        }
    }

    m_xPeeps->drainDeathQueue();
    m_xPeeps->drainMoveQueue();
    m_xSignals->fade(0); // takes layerNum  todo!!!

    saveVideoFrameSync(simStep, generation);
}

//---------------------------------------------------------------------------
void Backend::endOfGeneration(unsigned generation)
{
    auto params = m_xParameterIO->GetParamRef();
    {
        if (params.updateGraphLog && (generation == 1 || ((generation % params.updateGraphLogStride) == 0))) {
#pragma GCC diagnostic ignored "-Wunused-result"
            std::system(params.graphLogUpdateCommand.c_str());
        }
    }
}

//---------------------------------------------------------------------------
void Backend::executeActions(Peep &peep, std::array<float, SensorsActions::Action::NUM_ACTIONS> &actionLevels)
{
    // Only a subset of all possible actions might be enabled (i.e., compiled in).
    // This returns true if the specified action is enabled. See sensors-actions.h
    // for how to enable sensors and actions during compilation.
    auto isEnabled = [](enum SensorsActions::Action action){ return (int)action < (int)SensorsActions::Action::NUM_ACTIONS; };

    // Responsiveness action - convert neuron action level from arbitrary float range
    // to the range 0.0..1.0. If this action neuron is enabled but not driven, will
    // default to mid-level 0.5.
    if (isEnabled(SensorsActions::Action::SET_RESPONSIVENESS)) {
        float level = actionLevels[SensorsActions::Action::SET_RESPONSIVENESS]; // default 0.0
        level = (std::tanh(level) + 1.0) / 2.0; // convert to 0.0..1.0
        peep.responsiveness = level;
    }

    // For the rest of the action outputs, we'll apply an adjusted responsiveness
    // factor (see responseCurve() for more info). Range 0.0..1.0.
    float responsivenessAdjusted = AlgorithmHelpers::responseCurve(peep.responsiveness, m_xParameterIO->GetParamRef());

    // Oscillator period action - convert action level nonlinearly to
    // 2..4*p.stepsPerGeneration. If this action neuron is enabled but not driven,
    // will default to 1.5 + e^(3.5) = a period of 34 simSteps.
    if (isEnabled(SensorsActions::Action::SET_OSCILLATOR_PERIOD)) {
        auto periodf = actionLevels[SensorsActions::Action::SET_OSCILLATOR_PERIOD];
        float newPeriodf01 = (std::tanh(periodf) + 1.0) / 2.0; // convert to 0.0..1.0
        unsigned newPeriod = 1 + (int)(1.5 + std::exp(7.0 * newPeriodf01));
        assert(newPeriod >= 2 && newPeriod <= 2048);
        peep.oscPeriod = newPeriod;
    }

    // Set longProbeDistance - convert action level to 1..maxLongProbeDistance.
    // If this action neuron is enabled but not driven, will default to
    // mid-level period of 17 simSteps.
    if (isEnabled(SensorsActions::Action::SET_LONGPROBE_DIST)) {
        constexpr unsigned maxLongProbeDistance = 32;
        float level = actionLevels[SensorsActions::Action::SET_LONGPROBE_DIST];
        level = (std::tanh(level) + 1.0) / 2.0; // convert to 0.0..1.0
        level = 1 + level * maxLongProbeDistance;
        peep.longProbeDist = (unsigned)level;
    }

    // Emit signal0 - if this action value is below a threshold, nothing emitted.
    // Otherwise convert the action value to a probability of emitting one unit of
    // signal (pheromone).
    // Pheromones may be emitted immediately (see signals.cpp). If this action neuron
    // is enabled but not driven, nothing will be emitted.
    if (isEnabled(SensorsActions::Action::EMIT_SIGNAL0)) {
        constexpr float emitThreshold = 0.5;  // 0.0..1.0; 0.5 is midlevel
        float level = actionLevels[SensorsActions::Action::EMIT_SIGNAL0];
        level = (std::tanh(level) + 1.0) / 2.0; // convert to 0.0..1.0
        level *= responsivenessAdjusted;
        if (level > emitThreshold && AlgorithmHelpers::prob2bool(level, *m_xRandomGenerator.get())) {
            m_xSignals->increment(0, peep.loc);
        }
    }

    // Kill forward -- if this action value is > threshold, value is converted to probability
    // of an attempted murder. Probabilities under the threshold are considered 0.0.
    // If this action neuron is enabled but not driven, the neighbors are safe.
    if (isEnabled(SensorsActions::Action::KILL_FORWARD) && m_xParameterIO->GetParamRef().killEnable) {
        constexpr float killThreshold = 0.5;  // 0.0..1.0; 0.5 is midlevel
        float level = actionLevels[SensorsActions::Action::KILL_FORWARD];
        level = (std::tanh(level) + 1.0) / 2.0; // convert to 0.0..1.0
        level *= responsivenessAdjusted;
        if (level > killThreshold && AlgorithmHelpers::prob2bool((level - SensorsActions::ACTION_MIN) / SensorsActions::ACTION_RANGE, *m_xRandomGenerator.get())) {
            Coord otherLoc = peep.loc + peep.lastMoveDir;
            if (m_xGrid->isInBounds(otherLoc) && m_xGrid->isOccupiedAt(otherLoc)) {
                Peep &peep2 = m_xPeeps->getPeep(otherLoc);
                if (peep2.alive) {
                    assert((peep.loc - peep2.loc).length() == 1);
                    m_xPeeps->queueForDeath(peep2);
                }
            }
        }
    }

    // ------------- Movement action neurons ---------------

    // There are multiple action neurons for movement. Each type of movement neuron
    // urges the individual to move in some specific direction. We sum up all the
    // X and Y components of all the movement urges, then pass the X and Y sums through
    // a transfer function (tanh()) to get a range -1.0..1.0. The absolute values of the
    // X and Y values are passed through prob2bool() to convert to -1, 0, or 1, then
    // multiplied by the component's signum. This results in the x and y components of
    // a normalized movement offset. I.e., the probability of movement in either
    // dimension is the absolute value of tanh of the action level X,Y components and
    // the direction is the sign of the X, Y components. For example, for a particular
    // action neuron:
    //     X, Y == -5.9, +0.3 as raw action levels received here
    //     X, Y == -0.999, +0.29 after passing raw values through tanh()
    //     Xprob, Yprob == 99.9%, 29% probability of X and Y becoming 1 (or -1)
    //     X, Y == -1, 0 after applying the sign and probability
    //     The agent will then be moved West (an offset of -1, 0) if it's a legal move.

    float level;
    Coord offset;
    Coord lastMoveOffset = peep.lastMoveDir.asNormalizedCoord();

    // moveX,moveY will be the accumulators that will hold the sum of all the
    // urges to move along each axis. (+- floating values of arbitrary range)
    float moveX = isEnabled(SensorsActions::Action::MOVE_X) ? actionLevels[SensorsActions::Action::MOVE_X] : 0.0;
    float moveY = isEnabled(SensorsActions::Action::MOVE_Y) ? actionLevels[SensorsActions::Action::MOVE_Y] : 0.0;

    if (isEnabled(SensorsActions::Action::MOVE_EAST)) moveX += actionLevels[SensorsActions::Action::MOVE_EAST];
    if (isEnabled(SensorsActions::Action::MOVE_WEST)) moveX -= actionLevels[SensorsActions::Action::MOVE_WEST];
    if (isEnabled(SensorsActions::Action::MOVE_NORTH)) moveY += actionLevels[SensorsActions::Action::MOVE_NORTH];
    if (isEnabled(SensorsActions::Action::MOVE_SOUTH)) moveY -= actionLevels[SensorsActions::Action::MOVE_SOUTH];

    if (isEnabled(SensorsActions::Action::MOVE_FORWARD)) {
        level = actionLevels[SensorsActions::Action::MOVE_FORWARD];
        moveX += lastMoveOffset.x * level;
        moveY += lastMoveOffset.y * level;
    }
    if (isEnabled(SensorsActions::Action::MOVE_REVERSE)) {
        level = actionLevels[SensorsActions::Action::MOVE_REVERSE];
        moveX -= lastMoveOffset.x * level;
        moveY -= lastMoveOffset.y * level;
    }
    if (isEnabled(SensorsActions::Action::MOVE_LEFT)) {
        level = actionLevels[SensorsActions::Action::MOVE_LEFT];
        offset = peep.lastMoveDir.rotate90DegCCW().asNormalizedCoord();
        moveX += offset.x * level;
        moveY += offset.y * level;
    }
    if (isEnabled(SensorsActions::Action::MOVE_RIGHT)) {
        level = actionLevels[SensorsActions::Action::MOVE_RIGHT];
        offset = peep.lastMoveDir.rotate90DegCW().asNormalizedCoord();
        moveX += offset.x * level;
        moveY += offset.y * level;
    }
    if (isEnabled(SensorsActions::Action::MOVE_RL)) {
        level = actionLevels[SensorsActions::Action::MOVE_RL];
        offset = peep.lastMoveDir.rotate90DegCW().asNormalizedCoord();
        moveX += offset.x * level;
        moveY += offset.y * level;
    }

    if (isEnabled(SensorsActions::Action::MOVE_RANDOM)) {
        level = actionLevels[SensorsActions::Action::MOVE_RANDOM];
        offset = Dir::random8(*m_xRandomGenerator.get()).asNormalizedCoord();
        moveX += offset.x * level;
        moveY += offset.y * level;
    }

    // Convert the accumulated X, Y sums to the range -1.0..1.0 and scale by the
    // peep's responsiveness (0.0..1.0) (adjusted by a curve)
    moveX = std::tanh(moveX);
    moveY = std::tanh(moveY);
    moveX *= responsivenessAdjusted;
    moveY *= responsivenessAdjusted;

    // The probability of movement along each axis is the absolute value
    int16_t probX = (int16_t)AlgorithmHelpers::prob2bool(std::abs(moveX), *m_xRandomGenerator.get()); // convert abs(level) to 0 or 1
    int16_t probY = (int16_t)AlgorithmHelpers::prob2bool(std::abs(moveY), *m_xRandomGenerator.get()); // convert abs(level) to 0 or 1

    // The direction of movement (if any) along each axis is the sign
    int16_t signumX = moveX < 0.0 ? -1 : 1;
    int16_t signumY = moveY < 0.0 ? -1 : 1;

    // Generate a normalized movement offset, where each component is -1, 0, or 1
    Coord movementOffset = { (int16_t)(probX * signumX), (int16_t)(probY * signumY) };

    // Move there if it's a valid location
    Coord newLoc = peep.loc + movementOffset;
    if (m_xGrid->isInBounds(newLoc) && m_xGrid->isEmptyAt(newLoc)) {
        m_xPeeps->queueForMove(peep, newLoc);
    }
}

//---------------------------------------------------------------------------
//! Converts uint8 to QColor for the UI.
//---------------------------------------------------------------------------
QColor ConvertUint8ToQColor(uint8_t c)
{
    constexpr uint8_t maxColorVal = 0xb0;
    constexpr uint8_t maxLumaVal = 0xb0;
    auto rgbToLuma = [](uint8_t r, uint8_t g, uint8_t b) { return (r + r + r + b + g + g + g + g) / 8; };
    
    uint8_t r = (c);                  // R: 0..255
    uint8_t g = ((c & 0x1f) << 3);    // G: 0..255
    uint8_t b = ((c & 7)    << 5);    // B: 0..255

    // Prevent color mappings to very bright colors (hard to see):
    if (rgbToLuma(r, g, b) > maxLumaVal) {
        if (r > maxColorVal) r %= maxColorVal;
        if (g > maxColorVal) g %= maxColorVal;
        if (b > maxColorVal) b %= maxColorVal;
    }
    return QColor(r, g, b);
}

//---------------------------------------------------------------------------
ImageFrameData Backend::GetImageFrameData()
{
    m_Lock.lockForWrite();
    auto data = m_UIFrameData;
    m_Lock.unlock();
    return data;
}

//---------------------------------------------------------------------------
void Backend::saveVideoFrameSync(unsigned simStep, unsigned generation)
{
    // We cache a local copy of data from params, grid, and peeps because
    // those objects will change by the main thread at the same time our
    // saveFrameThread() is using it to output a video frame.
    m_UIFrameData.simStep = simStep;
    m_UIFrameData.generation = generation;
    m_UIFrameData.barrierLocs.clear();
    m_UIFrameData.signalLayers.clear();
    m_UIFrameData.maxPopulation = m_xParameterIO->GetParamRef().population;

    {
        m_Lock.lockForWrite();
        m_UIFrameData.peepsPositions.clear();
        m_UIFrameData.peepsColors.clear();
        for (uint16_t index = 1; index <= m_xParameterIO->GetParamRef().population; ++index) {
            const Peep &peep = (*m_xPeeps.get())[index];
            if (peep.alive) {
                m_UIFrameData.peepsPositions.append(QPoint(peep.loc.x, peep.loc.y));
                m_UIFrameData.peepsColors.append(ConvertUint8ToQColor(Genetics::makeGeneticColor(peep.genome)));
            }
        }
        m_Lock.unlock();
    }

    auto const &barrierLocs = m_xGrid->getBarrierLocations();
    for (Coord loc : barrierLocs) {
        m_UIFrameData.barrierLocs.push_back(loc);
    }
}

//---------------------------------------------------------------------------
eChallenges Backend::GetChallengeId() const
{
    return m_xGenerationGenerator->GetChallengeId();
}

//---------------------------------------------------------------------------
Challenges::iChallenge* Backend::GetChallenge() const
{
    return m_xGenerationGenerator->GetChallenge();
}