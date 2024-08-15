/*=============================================================================*/
// Copyright 2020-2021 Elite Engine
/*=============================================================================*/
// StatesAndTransitions.h: Implementation of the state/transition classes
/*=============================================================================*/
#ifndef ELITE_APPLICATION_FSM_STATES_TRANSITIONS
#define ELITE_APPLICATION_FSM_STATES_TRANSITIONS

#include "projects/DecisionMaking/SmartAgent.h"
#include "projects/Movement/SteeringBehaviors/Steering/SteeringBehaviors.h"
#include "framework/EliteAI/EliteData/EBlackboard.h"

namespace FSMState
{
	using namespace Elite;
	// FSM TODO:

	//------------
	//---STATES---
	//------------
	class PatrolState : public FSMState
	{
	public:
		PatrolState() {}
		~PatrolState() = default;

		void OnEnter(Blackboard* pBlackboard) override;
		void Update(Blackboard* pBlackboard, float deltaTime) override;
	};

	class ChaseState : public FSMState
	{
	public:
		ChaseState() {}
		~ChaseState() = default;

		void OnEnter(Blackboard* pBlackboard) override;
		void Update(Blackboard* pBlackboard, float deltaTime) override;
	};

	class SearchState : public FSMState
	{
	public:
		SearchState() {}
		~SearchState() = default;

		void OnEnter(Blackboard* pBlackboard) override;
		void Update(Blackboard* pBlackboard, float deltaTime) override;
	};

	//-----------------
	//---TRANSITIONS---
	//-----------------

	class IsTargetVisible : public FSMCondition
	{
	public:
		IsTargetVisible() {};
		~IsTargetVisible() = default;

		bool Evaluate(Blackboard* pBlackboard) const override;
	};

	class IsTargetNotVisible : public FSMCondition
	{
	public:
		IsTargetNotVisible() {};
		~IsTargetNotVisible() = default;

		bool Evaluate(Blackboard* pBlackboard) const override;
	};

	class IsSearchingTooLong : public FSMCondition
	{
	public:
		IsSearchingTooLong() {};
		~IsSearchingTooLong() = default;

		bool Evaluate(Blackboard* pBlackboard) const override;
	};

}
#endif