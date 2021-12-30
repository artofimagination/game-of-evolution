#pragma once

#include <cstdint>
#include <functional>

class Parameters;

//! \class SysStateMachine
//! \brief Runs the core states of the simulator
//!
//! The state is determined incoming system actions, to control the running of the simulation
//! and parameter actions, that change the simulation configuration, hence the system behaviour.
//! It only outputs two states, step is running and generation is running. This controls the simulation loop.
class SysStateMachine
{
public:
    //! System inputs to control the running of the simulation
    enum class eSysActions : uint8_t
    {
        Idle,               ///!< No active action.
        Stop,               ///!< Stop simulation.
        Run,                ///!< Run simulation.
        Reset,              ///!< Reset simulation.
    };

    //! Parameter related actions to control the system state.
    enum class eParameterActions : uint8_t
    {
        Idle,                 ///!< No active action.
        InitSensorsActions,   ///!< Sensors/actions changed.
        InitChallenge,        ///!< Challanege has been changed.
        ClearAnalyticsCounts, ///!< Clears analytics counters.
        FinishAction          ///!< Parameter action has been completed.
    };

    //! Update parameter action.
    void UpdateParameterAction(eParameterActions action) { m_ParameterAction = action; };
    //! Update system action.
    void UpdateSysAction(eSysActions action) { m_SysAction = action; };
    //! Evaluates state machine.
    void Evaluate(
        std::function<bool()> checkParameters,
        std::function<void()> reset);

    //! If returns true the simulation step is running.
    bool SimStepRunning() const { return m_RunSimStep; }
    //! If returns true the generations are running.
    bool GenerationRunning() const { return m_RunGeneration; }

private:
    //! System states that control the simulation running.
    enum class eSysStates : uint8_t
    {
        Stopped,    ///!< The simulation is not running.
        Running,    ///!< The simulation is running.
        Transition, ///!< Transition between two states is happening.
                    ///!< When a complex process is running (reset, update parameters), 
                    ///!< while they are not completed the system stays in transition
        Failing     ///!< System failure, raises error.
    };

    //! Apply new system state.
    void ApplyState(std::function<void()> reset, std::function<bool()> checkParameters);
    //! Evaluate incoming system actions.
    void CheckSysAction();
    //! Evaluate parameter actions.
    void CheckParamAction();

    // Input actions from GUI
    eSysActions       m_SysAction{eSysActions::Idle};               ///!< Stores the current system action.
    eParameterActions m_ParameterAction{eParameterActions::Idle};   ///!< Stores the current parameter action.

    // Internal state flow
    eParameterActions m_ParameterActionInProcess{eParameterActions::Idle};  ///!< Stores a non-idle param action state 
                                                                            ///!< as long as the action is not finished.
    eSysStates        m_CurrentSysState{eSysStates::Stopped};       ///!< Stores the current system state.
    eSysStates        m_NewSysState{eSysStates::Stopped};           ///!< Stores the new requested state.
    std::string       m_Error{};

    // State machine outputs
    bool              m_RunGeneration{false};                       ///!< Stores the state whether the generation is running.
    bool              m_RunSimStep{false};                          ///!< Stores the state whether the simulation step is running.
};