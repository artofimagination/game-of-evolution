#include "SysStateMachine.h"

#include <cassert>

//-------------------------------------------------------------------------
void SysStateMachine::CheckParamAction()
{
  // If there is an actual parameter change request (so no Idle or FinishAction, or Failing)
  // Then we put the system in a transitional state and keep it there as long as an FinishAction is received.
  switch (m_ParameterAction)
  {
  case eParameterActions::InitSensorsActions:
  case eParameterActions::InitChallenge:
  case eParameterActions::ClearAnalyticsCounts:
    // When FinishAction is received, we want to know what action we are finishing,
    // let's store the active action.
    m_ParameterActionInProcess = m_ParameterAction;
    m_NewSysState = eSysStates::Transition;
    break;
  case eParameterActions::Idle:
    break;
  // Some action require reset, some don't
  case eParameterActions::FinishAction:
    if (m_ParameterActionInProcess == eParameterActions::InitSensorsActions ||
        m_ParameterActionInProcess == eParameterActions::InitChallenge)
        m_SysAction = eSysActions::Reset;
    else if (m_ParameterActionInProcess == eParameterActions::ClearAnalyticsCounts)
        m_SysAction = eSysActions::Run;
    m_ParameterActionInProcess = eParameterActions::Idle;
    break;
  default:
    m_NewSysState = eSysStates::Failing;
    m_Error += "\nUnknown parameter action";
    break;
  }
}

//-------------------------------------------------------------------------
void SysStateMachine::CheckSysAction()
{
    switch (m_SysAction)
    {
    case eSysActions::Run:
        if (m_ParameterAction == eParameterActions::Idle)
            m_NewSysState = eSysStates::Running;
        break;
    case eSysActions::Stop:
        m_NewSysState = eSysStates::Stopped;
        break;
    case eSysActions::Reset:
        m_NewSysState = eSysStates::Transition;
        break;
    case eSysActions::Idle:
        break;
    default:
        m_NewSysState = eSysStates::Failing;
        m_Error += "\nUnknown sys action";
        break;
    }
}

//-------------------------------------------------------------------------
void SysStateMachine::Evaluate(
    std::function<bool()> checkParameters,
    std::function<void()> reset)
{
    if (m_SysAction == eSysActions::Idle && 
        m_ParameterAction == eParameterActions::Idle &&
        m_NewSysState == m_CurrentSysState)
        return;

    // Let's check first what active parameter action there is.
    CheckParamAction();
    // Meanwhile we can also check for some system actions too.
    CheckSysAction();
    // Alright all actions are checked lets apply the new system state based on the above checks.
    ApplyState(reset, checkParameters);
}

//-------------------------------------------------------------------------
void SysStateMachine::ApplyState(std::function<void()> reset, std::function<bool()> checkParameters)
{
    switch (m_NewSysState)
    {
    case eSysStates::Stopped:
        // Alright, we need to stop simulating, clear all actions in execution and stop the simulation.
        m_RunSimStep = false;
        m_RunGeneration = false;
        m_SysAction = eSysActions::Idle;
        m_ParameterAction = eParameterActions::Idle;
        break;
    case eSysStates::Running:
        // Time to roll, so if all the parameters are meeting the simulation conditions
        // Let's start simulating, otherwise sit tight and wait.
        if (!checkParameters())
        {
            m_NewSysState = eSysStates::Stopped;
            ApplyState(nullptr, checkParameters);
            break;
        }
        m_RunSimStep = true;
        m_RunGeneration = true;
        m_SysAction = eSysActions::Idle;
        m_ParameterAction = eParameterActions::Idle;
        break;
    case eSysStates::Transition:
        // Okay, we have to reset, lets stop everything first to be safe,
        // then reset our simulation.
        if (m_SysAction == eSysActions::Reset)
        {
            if (!reset)
            {
                m_NewSysState = eSysStates::Failing;
                m_Error += "\nreset() is null, corrupt state.";
            }
            else
            { 
                m_NewSysState = eSysStates::Stopped;
                ApplyState(nullptr, checkParameters);
                reset();
                m_NewSysState = eSysStates::Running;
                ApplyState(nullptr, checkParameters);
            }
        }
        // The user wants to stop, lets stop everything then (including parameter actions in execution)
        else if (m_SysAction == eSysActions::Stop)
        {
            m_NewSysState = eSysStates::Stopped;
            ApplyState(nullptr, checkParameters);
        }

        // we have some parameter actions in making, lets stop the simulation and decide
        // whether to restart or continue when the action is completed.
        if (m_ParameterAction == eParameterActions::InitSensorsActions ||
            m_ParameterAction == eParameterActions::InitChallenge ||
            m_ParameterAction == eParameterActions::ClearAnalyticsCounts)
        {
            m_NewSysState = eSysStates::Stopped;
            ApplyState(nullptr, checkParameters);
        }

        break;
    case eSysStates::Failing:
    default:
        assert(!m_Error.c_str());
        break;
    }

    m_CurrentSysState = m_NewSysState;
}