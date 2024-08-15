#include "stdafx.h"
#include "StatesAndTransitions.h"

#include "projects/Shared/NavigationColliderElement.h"
#include "projects/Movement/SteeringBehaviors/PathFollow/PathFollowSteeringBehavior.h"
using namespace FSMState;
// FSM TODO:

//------------
//---STATES---
//------------
void PatrolState::OnEnter(Blackboard* pBlackboard)
{
	SmartAgent* pGuard{};
	PathFollow* pPathFollowBehavior{};
	std::vector<Elite::Vector2> path{};

	pBlackboard->GetData("Guard", pGuard);
	pBlackboard->GetData("PathFollow", pPathFollowBehavior);
	pBlackboard->GetData("PatrolPath", path);

	pPathFollowBehavior->SetPath(path);
	pGuard->SetSteeringBehavior(pPathFollowBehavior);
}

void PatrolState::Update(Blackboard* pBlackboard, float deltaTime)
{
	PathFollow* pPathFollowBehavior{};
	pBlackboard->GetData("PathFollow", pPathFollowBehavior);

	if(pPathFollowBehavior->HasArrived())
	{
		OnEnter(pBlackboard);
	}
}

void ChaseState::OnEnter(Blackboard* pBlackboard)
{
	SmartAgent* pGuard{};
	SteeringAgent* pPlayer{};

	pBlackboard->GetData("Guard", pGuard);
	pBlackboard->GetData("Agent", pPlayer);

	pGuard->SetSteeringBehavior(pGuard->GetSeekBehavior());
}

void ChaseState::Update(Blackboard* pBlackboard, float deltaTime)
{
	SmartAgent* pGuard{};
	SteeringAgent* pPlayer{};

	pBlackboard->GetData("Guard", pGuard);
	pBlackboard->GetData("Agent", pPlayer);

	pGuard->GetSeekBehavior()->SetTarget(pPlayer->GetPosition());
}

void SearchState::OnEnter(Blackboard* pBlackboard)
{
	SmartAgent* pGuard{};

	pBlackboard->GetData("Guard", pGuard);

	pGuard->SetSteeringBehavior(pGuard->GetWanderBehavior());
}

void SearchState::Update(Blackboard* pBlackboard, float deltaTime)
{
	float timerSearching{};

	pBlackboard->GetData("TimerSearching", timerSearching);
	
	timerSearching += deltaTime;
	pBlackboard->ChangeData("TimerSearching", timerSearching);

}

//-----------------
//---TRANSITIONS---
//-----------------
bool IsTargetVisible::Evaluate(Blackboard* pBlackboard) const
{
	SmartAgent* pGuard{};
	SteeringAgent* pPlayer{};
	float detectionRadius{};

	pBlackboard->GetData("Guard", pGuard);
	pBlackboard->GetData("Agent", pPlayer);
	pBlackboard->GetData("DetectionRadius", detectionRadius);

	const float distance = pGuard->GetPosition().DistanceSquared(pPlayer->GetPosition());

	return (pGuard->HasLineOfSight(pPlayer->GetPosition())) && distance <= (detectionRadius * detectionRadius);
}

bool IsTargetNotVisible::Evaluate(Blackboard* pBlackboard) const
{
	SmartAgent* pGuard{};
	SteeringAgent* pPlayer{};
	float detectionRadius{};

	pBlackboard->GetData("Guard", pGuard);
	pBlackboard->GetData("Agent", pPlayer);
	pBlackboard->GetData("DetectionRadius", detectionRadius);

	const float distance = pGuard->GetPosition().DistanceSquared(pPlayer->GetPosition());

	return (!pGuard->HasLineOfSight(pPlayer->GetPosition())) || (distance > (detectionRadius * detectionRadius));
}

bool IsSearchingTooLong::Evaluate(Blackboard* pBlackboard) const
{
	SmartAgent* pGuard{};
	float maxSearchTime{};
	float timerSearching{};

	pBlackboard->GetData("Guard", pGuard);
	pBlackboard->GetData("MaxSearchTime", maxSearchTime);
	pBlackboard->GetData("TimerSearching", timerSearching);

	if(timerSearching >= maxSearchTime)
	{
		timerSearching = 0.0f;
		pBlackboard->ChangeData("TimerSearching", timerSearching);
		return true;
	}

	return false;
}

