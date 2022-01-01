#include "Backend.h"

#include "Analytics.h"
#include "AlgorithmHelpers.h"
#include "BasicTypes.h"
#include "Challenges/Altruism.h"
#include "Challenges/iChallenges.h"
#include "SensorsActions.h"

#include <QColor>
#include <QPoint>


//---------------------------------------------------------------------------
Backend::Backend()
  : m_xRandomGenerator(std::make_unique<RandomUintGenerator>())
  , m_xParameterIO(std::make_unique<ParameterIO>())
  , m_xGrid(std::make_unique<Grid>(m_xParameterIO->GetParamRef(), *m_xRandomGenerator.get()))
  , m_xSignals(std::make_unique<PheromoneSignals>(m_xParameterIO->GetParamRef()))
  , m_xSensors(std::make_unique<Sensors>())
  , m_xPeeps(std::make_unique<PeepsPool>(*m_xGrid.get()))
  , m_xActions(std::make_unique<Actions>(
      *m_xPeeps.get(),
      *m_xRandomGenerator.get(),
      *m_xSignals.get(),
      *m_xGrid.get(),
      m_xParameterIO->GetParamRef()
  ))
  , m_xChallenge(std::make_unique<Challenges::Altruism>(*m_xRandomGenerator.get(), m_xParameterIO->GetParamRef()))
  , m_xAnalytics(std::make_unique<Analytics>())
  , m_xGenerationGenerator(std::make_unique<GenerationGenerator>(
      *m_xGrid.get(), 
      *m_xPeeps.get(), 
      *m_xAnalytics.get(),
      *m_xSignals.get(), 
      m_xParameterIO->GetParamRef(),
      *m_xRandomGenerator.get(),
      m_BarrierType,
      m_Barriers))
  , m_xSysStateMachine(std::make_unique<SysStateMachine>())
  , m_BarrierType(static_cast<eBarrierType>(m_xParameterIO->GetParamRef().barrierType))
{
    qRegisterMetaType<WorldData>("WorldData");

}

//---------------------------------------------------------------------------
bool Backend::CheckParameters()
{
    auto params = m_xParameterIO->GetParamRef();
    if(!m_xChallenge)
    {
        return false;
    }
    if (m_Generation >= params.maxGenerations)
    {
        return false;
    }
    if(m_xSensors->AvailableSensorTypeCount() == 0)
    {
        return false;
    }
    if(m_xActions->AvailableActionTypeCount() == 0)
    {
        return false;
    }
    
    return true;
}

//---------------------------------------------------------------------------
void Backend::StartSim()
{
    m_xSysStateMachine->UpdateSysAction(SysStateMachine::eSysActions::Run);
}

//---------------------------------------------------------------------------
void Backend::StopSim()
{
    m_xSysStateMachine->UpdateSysAction(SysStateMachine::eSysActions::Stop);
}

//---------------------------------------------------------------------------
void Backend::ResetSim()
{
    m_xSysStateMachine->UpdateSysAction(SysStateMachine::eSysActions::Reset);
}

//---------------------------------------------------------------------------
std::vector<std::string> Backend::GetChallengeNames() const
{
    return Challenges::GetChallengeNames();
}

//---------------------------------------------------------------------------
void Backend::Reset()
{
    if (m_xSensors->AvailableSensorTypeCount() > 0 && m_xActions->AvailableActionTypeCount() > 0)
    {
        auto params = m_xParameterIO->GetParamRef();
        m_xGenerationGenerator->initializeGeneration0(
            static_cast<eBarrierType>(params.barrierType),
            m_xSensors->AvailableSensorTypeCount(),
            m_xActions->AvailableActionTypeCount());
        m_Generation = 0;
        m_xAnalytics->Clear();
    }
}

//---------------------------------------------------------------------------
void Backend::UpdateSensorsActions(const QVariantList& sensors, const QVariantList& actions)
{
    std::vector<Sensors::eType> sensorsVector{};
    for (int i = 0; i < sensors.size(); ++i)
    {
        if (sensors[i].toBool())
            sensorsVector.push_back(static_cast<Sensors::eType>(i));
    }

    std::vector<Actions::eType> actionsVector{};
    for (int i = 0; i < actions.size(); ++i)
    {
        if (actions[i].toBool())
            actionsVector.push_back(static_cast<Actions::eType>(i));
    }

    m_xSysStateMachine->UpdateParameterAction(SysStateMachine::eParameterActions::InitSensorsActions);
    m_xSensors->UpdateAvailableSensorTypes(sensorsVector);
    m_xActions->UpdateAvailableActionTypes(actionsVector);
    m_xSysStateMachine->UpdateParameterAction(SysStateMachine::eParameterActions::FinishAction);
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
    SetChallengeId(static_cast<unsigned>(m_CurrentChallenge));
    m_BarrierType = static_cast<eBarrierType>(parameters.barrierType);

    // Define functions called in the system state machine
    auto reset = [this]() { Backend::Reset();} ;
    auto checkParameters = [this]() { return Backend::CheckParameters(); };

    while (!m_ThreadStop)
    {
        // Evaluate the state machine every cycle, looking for parameter/action changes
        m_xSysStateMachine->Evaluate(checkParameters, reset);
        if (m_xSysStateMachine->GenerationRunning())
        {
            m_xGenerationGenerator->initializeGeneration0(
            static_cast<eBarrierType>(parameters.barrierType),
            m_xSensors->AvailableSensorTypeCount(),
            m_xActions->AvailableActionTypeCount()); // starting population
        }

        while (!m_ThreadStop && m_xSysStateMachine->GenerationRunning()) { // generation loop
            unsigned murderCount = 0; // for reporting purposes
            for (unsigned simStep = 0; simStep < parameters.stepsPerGeneration && m_xSysStateMachine->SimStepRunning(); ++simStep) {
                m_xSysStateMachine->Evaluate(checkParameters, reset);
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
                endOfSimStep(simStep, m_Generation);
            }

            endOfGeneration(m_Generation);
            unsigned numberSurvivors = 
                m_xGenerationGenerator->spawnNewGeneration(
                    m_Generation,
                    murderCount,
                    m_xChallenge.get(),
                    m_xSensors->AvailableSensorTypeCount(),
                    m_xActions->AvailableActionTypeCount());
            // if (numberSurvivors > 0 && (m_Generation % parameters.genomeAnalysisStride == 0)) {
            //     Genetics::displaySampleGenomes(parameters.displaySampleGenomes, *m_xPeeps.get(), parameters);
            // }
            if (numberSurvivors == 0) {
                m_Generation = 0;  // start over
            } else {
                ++m_Generation;
            }
        }
        //Genetics::displaySampleGenomes(3, *m_xPeeps.get(), parameters); // final report, for debugging
    }
}

//---------------------------------------------------------------------------
void Backend::SimStepOnePeep(Peep &peep, unsigned simStep, RandomUintGenerator& random)
{
    ++peep.age; // for this implementation, tracks simStep
    auto actionLevels = peep.feedForward(
        simStep,
        m_xGenerationGenerator->GetOldestAge(),
        *m_xPeeps.get(),
        *m_xSignals.get(),
        *m_xSensors.get(),
        random);
    m_xActions->executeActions(peep, simStep, actionLevels);
}

//---------------------------------------------------------------------------
void Backend::endOfSimStep(unsigned simStep, unsigned generation)
{
    auto params = m_xParameterIO->GetParamRef();
    auto settings = Challenges::Settings();
    settings.simStep = simStep;
    m_xChallenge->EvaluateAtEndOfSimStep(*m_xPeeps.get(), m_xParameterIO->GetParamRef(), *m_xGrid.get(), settings);

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
WorldData Backend::GetWorldData()
{
    m_Lock.lockForWrite();
    auto data = m_WorldData;
    m_Lock.unlock();
    return data;
}

//---------------------------------------------------------------------------
void Backend::saveVideoFrameSync(unsigned simStep, unsigned generation)
{
    // We cache a local copy of data from params, grid, and peeps because
    // those objects will change by the main thread at the same time our
    // saveFrameThread() is using it to output a video frame.
    m_WorldData.simStep = simStep;
    m_WorldData.generation = generation;
    m_WorldData.signalLayers.clear();
    m_WorldData.maxPopulation = m_xParameterIO->GetParamRef().population;

    {
        m_Lock.lockForWrite();
        m_WorldData.peepsPositions.clear();
        m_WorldData.peepsColors.clear();
        for (uint16_t index = 1; index <= m_xParameterIO->GetParamRef().population; ++index) {
            const Peep &peep = (*m_xPeeps.get())[index];
            if (peep.alive) {
                m_WorldData.peepsPositions.append(QPoint(peep.loc.x, peep.loc.y));
                m_WorldData.peepsColors.append(ConvertUint8ToQColor(Genetics::makeGeneticColor(peep.genome)));
            }
        }
        m_Lock.unlock();
    }
}

//---------------------------------------------------------------------------
eChallenges Backend::GetChallengeId() const
{
    return m_CurrentChallenge;
}

//---------------------------------------------------------------------------
void Backend::SetChallengeId(unsigned id)
{
    // Inform the state machine about the parameter action.
    m_xSysStateMachine->UpdateParameterAction(SysStateMachine::eParameterActions::InitChallenge);
    m_CurrentChallenge = static_cast<eChallenges>(id);
    m_xChallenge = 
        std::unique_ptr<Challenges::iChallenge>(Challenges::CreateChallenge(
            m_CurrentChallenge,
            *m_xRandomGenerator.get(),
            *m_xAnalytics.get(),
            m_xParameterIO->GetParamRef()));
    // Inform the state machine about the parameter action completion.
    m_xSysStateMachine->UpdateParameterAction(SysStateMachine::eParameterActions::FinishAction);
}

//---------------------------------------------------------------------------
Challenges::iChallenge* Backend::GetChallenge() const
{
    return m_xChallenge.get();
}

//---------------------------------------------------------------------------
const std::vector<std::unique_ptr<Barriers::iBarrier> >& Backend::GetBarriers() const
{
    return m_Barriers;
}

//---------------------------------------------------------------------------
std::vector<std::string> Backend::GetSensorNames() const
{
    std::vector<std::string> names;
    for(Sensors::eType i = Sensors::eType::LOC_X; i < Sensors::eType::NUM_SENSES; i = static_cast<Sensors::eType>((size_t)i + 1))
    {
        names.push_back(SensorsActions::sensorName(i) + " (" + SensorsActions::sensorShortName(i) + ")");
    }
    return names;
}

//---------------------------------------------------------------------------
std::vector<std::string> Backend::GetActionNames() const
{
    std::vector<std::string> names;
    for(Actions::eType i = Actions::eType::SET_OSCILLATOR_PERIOD; i < Actions::eType::NUM_ACTIONS; i = static_cast<Actions::eType>((size_t)i + 1))
    {
        names.push_back(SensorsActions::actionName(i) + " (" + SensorsActions::actionShortName(i) + ")");
    }
    return names;
}

//---------------------------------------------------------------------------
std::pair<unsigned, std::vector<unsigned> > Backend::GetSurvivors() const
{
    return m_xAnalytics->GetSurvivors();
}

//---------------------------------------------------------------------------
std::pair<unsigned, std::vector<unsigned> > Backend::GetSurvivorsToNextGen() const
{
    return m_xAnalytics->GetSurvivorsNextGen();
}

//---------------------------------------------------------------------------
std::pair<unsigned, std::vector<float> > Backend::GetGeneticDiversity() const
{
    return m_xAnalytics->GetGeneticDiversity();
}

//---------------------------------------------------------------------------
std::vector<std::string> Backend::GetAnalyticsTypes() const
{
    return Analytics::GetAnalyticsNames();
}

//---------------------------------------------------------------------------
std::pair<uint16_t, uint16_t> Backend::GetFrameSize() const
{
    return { m_xParameterIO->GetParamRef().sizeX, m_xParameterIO->GetParamRef().sizeY }; 
};

//---------------------------------------------------------------------------
void Backend::ClearAnalyticsProcessedCount()
{
    m_xSysStateMachine->UpdateParameterAction(SysStateMachine::eParameterActions::ClearAnalyticsCounts);
    m_xAnalytics->ClearProcessedCounts();
    m_xSysStateMachine->UpdateParameterAction(SysStateMachine::eParameterActions::FinishAction);
};

//---------------------------------------------------------------------------
std::pair<unsigned, std::vector<std::vector<unsigned> > > Backend::GetCompletedChallengeTaskCounts() const
{
    return m_xAnalytics->GetCompletedChallengeTaskCounts();
}

//---------------------------------------------------------------------------
std::pair<unsigned, std::vector<float> > Backend::GetAvgAges() const
{
    return m_xAnalytics->GetAvgAges();
}