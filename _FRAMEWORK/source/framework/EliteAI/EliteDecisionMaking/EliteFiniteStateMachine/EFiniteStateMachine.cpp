//=== General Includes ===
#include "stdafx.h"
#include "EFiniteStateMachine.h"

using namespace Elite;

FiniteStateMachine::FiniteStateMachine(FSMState* startState, Blackboard* pBlackboard)
    : m_pCurrentState(nullptr),
    m_pBlackboard(pBlackboard)
{
    ChangeState(startState);
}

FiniteStateMachine::~FiniteStateMachine()
{
    SAFE_DELETE(m_pBlackboard);
}

void FiniteStateMachine::AddTransition(FSMState* startState, FSMState* toState, FSMCondition* condition)
{
    auto it = m_Transitions.find(startState);
    if (it == m_Transitions.end())
    {
        m_Transitions[startState] = Transitions();
    }
   
    m_Transitions[startState].push_back(std::make_pair(condition, toState));
}

void FiniteStateMachine::Update(float deltaTime)
{
    // FSM TODO:
    //TODO 4: Look if 1 or more condition exists for the current state that we are in
    //Tip: Check the transitions map for a TransitionState pair
    if(!m_Transitions.contains(m_pCurrentState))
    {
        return;
    }

    const Transitions transitions = m_Transitions[m_pCurrentState];
    if(transitions.empty())
    {
        return;
    }

    //TODO 5: if a TransitionState exists
        //TODO 6: Loop over all the TransitionState pairs
    for(const auto& state : transitions)
    {
        //TODO 7: If the Evaluate function of the FSMCondition returns true => transition to the new corresponding state
        if(state.first->Evaluate(m_pBlackboard) == true)
        {
            ChangeState(state.second);
        }
    }

    //TODO 8: Update the current state (if one exists)
    m_pCurrentState->Update(m_pBlackboard, deltaTime);
}

Blackboard* FiniteStateMachine::GetBlackboard() const
{
    return m_pBlackboard;
}

void FiniteStateMachine::ChangeState(FSMState* newState)
{
    // FSM TODO:
    //TODO 1. If currently in a state => make sure the OnExit of that state gets called
    if(m_pCurrentState != nullptr)
    {
        m_pCurrentState->OnExit(m_pBlackboard);
    }

    //TODO 2. Change the current state to the new state
    m_pCurrentState = newState;

    //TODO 3. Call the OnEnter of the new state
    if (m_pCurrentState != nullptr)
    {
        m_pCurrentState->OnEnter(m_pBlackboard);
    }
}
