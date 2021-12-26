#include "SysStateMachine.h"

#include <cassert>

//-------------------------------------------------------------------------
void SysStateMachine::CheckParamAction()
{
  switch (m_ParameterAction)
  {
  case eParameterActions::InitSensorsActions:
  case eParameterActions::InitChallenge:
    m_NewSysState = eSysStates::Transition;
    break;
  case eParameterActions::Idle:
    break;
  case eParameterActions::FinishAction:
    m_SysAction = eSysActions::Reset;
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


    CheckParamAction();
    CheckSysAction();
    ApplyState(reset, checkParameters);
}

//-------------------------------------------------------------------------
void SysStateMachine::ApplyState(std::function<void()> reset, std::function<bool()> checkParameters)
{
    if (m_CurrentSysState == m_NewSysState)
        return;

    switch (m_NewSysState)
    {
    case eSysStates::Stopped:
        m_RunSimStep = false;
        m_RunGeneration = false;
        m_SysAction = eSysActions::Idle;
        m_ParameterAction = eParameterActions::Idle;
        break;
    case eSysStates::Running:
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
        if (m_ParameterAction == eParameterActions::InitSensorsActions ||
            m_ParameterAction == eParameterActions::InitChallenge)
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