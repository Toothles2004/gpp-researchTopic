/*=============================================================================*/
// Copyright 2020-2021 Elite Engine
/*=============================================================================*/
// Behaviors.h: Implementation of certain reusable behaviors for the BT version of the Agario Game
/*=============================================================================*/
#ifndef ELITE_APPLICATION_BEHAVIOR_TREE_BEHAVIORS
#define ELITE_APPLICATION_BEHAVIOR_TREE_BEHAVIORS
//-----------------------------------------------------------------
// Includes & Forward Declarations
//-----------------------------------------------------------------
#include "framework/EliteMath/EMath.h"
#include "framework/EliteAI/EliteDecisionMaking/EliteBehaviorTree/EBehaviorTree.h"
#include "projects/DecisionMaking/SmartAgent.h"
#include "projects/Movement/SteeringBehaviors/Steering/SteeringBehaviors.h"
#include "projects/Movement/SteeringBehaviors/PathFollow/PathFollowSteeringBehavior.h"

//-----------------------------------------------------------------
// Behaviors
//-----------------------------------------------------------------
// BT TODO:

namespace BT_Actions 
{
	Elite::BehaviorState ChangeToPatrol(Elite::Blackboard* pBlackboard)
	{
		SmartAgent* pGuard{};
		PathFollow* pPathFollow{};
		std::vector<Elite::Vector2> path{};
		pBlackboard->GetData("Agent", pGuard);
		pBlackboard->GetData("PatrolBehavior", pPathFollow);
		pBlackboard->GetData("PatrolPath", path);

		assert(pGuard && "No agent! I fail Loudly");
		assert(pPathFollow && "No path! I fail Loudly");

		if(!pGuard || !pPathFollow)
		{
			return Elite::BehaviorState::Failure;
		}

		pGuard->SetSteeringBehavior(pPathFollow);

		if(pPathFollow->HasArrived())
		{
			pPathFollow->SetPath(path);
		}

		return Elite::BehaviorState::Success;
	}

	Elite::BehaviorState ChangeToChase(Elite::Blackboard* pBlackboard)
	{
		SmartAgent* pGuard{};
		Elite::Vector2 target{};
		pBlackboard->GetData("Agent", pGuard);
		pBlackboard->GetData("LastKnownPos", target);

		if(!pGuard)
		{
			return Elite::BehaviorState::Failure;
		}

		const auto seek = pGuard->GetSeekBehavior();
		seek->SetTarget(target);
		pGuard->SetSteeringBehavior(seek);

		return Elite::BehaviorState::Success;
	}

	Elite::BehaviorState ChangeToSearch(Elite::Blackboard* pBlackboard)
	{
		SmartAgent* pGuard{};

		pBlackboard->GetData("Agent", pGuard);

		if(!pGuard)
		{
			return Elite::BehaviorState::Failure;
		}

		pGuard->SetSteeringBehavior(pGuard->GetWanderBehavior());

		return Elite::BehaviorState::Success;
	}
	
}

namespace BT_Conditions
{
	bool HasSeenPlayer(Elite::Blackboard* pBlackboard)
	{
		SmartAgent* pGuard{};
		SteeringAgent* pPlayer{};
		float detectionRadius{};
		pBlackboard->GetData("Agent", pGuard);
		pBlackboard->GetData("TargetAgent", pPlayer);
		pBlackboard->GetData("DetectionRadius", detectionRadius);

		assert(pGuard && "No guard! I fail Loudly");
		assert(pPlayer && "No player! I fail Loudly");

		const bool isInLineOfSight = pGuard->HasLineOfSight(pPlayer->GetPosition());
		const bool isInRadius = DistanceSquared(pGuard->GetPosition(), pPlayer->GetPosition()) <= detectionRadius * detectionRadius;

		if(isInLineOfSight && isInRadius)
		{
			const auto currentTime = std::chrono::duration_cast<std::chrono::seconds>
				(
					std::chrono::system_clock::now().time_since_epoch()
				);
			pBlackboard->ChangeData("LastKnownPos", pPlayer->GetPosition());
			pBlackboard->ChangeData("TimeLastSeen", currentTime);
			pBlackboard->ChangeData("HasReachedLastSeenPoint", false);
			return true;
		}

		return false;
	}

	bool ShouldSearch(Elite::Blackboard* pBlackboard)
	{
		SmartAgent* pGuard{};
		SteeringAgent* pPlayer{};
		float detectionRadius{};

		pBlackboard->GetData("Agent", pGuard);
		pBlackboard->GetData("TargetAgent", pPlayer);
		pBlackboard->GetData("DetectionRadius", detectionRadius);

		assert(pGuard && "No guard! I fail Loudly");
		assert(pPlayer && "No player! I fail Loudly");

		const bool isNotInLineOfSight = !pGuard->HasLineOfSight(pPlayer->GetPosition());
		const bool isNotInRadius = DistanceSquared(pGuard->GetPosition(), pPlayer->GetPosition()) > detectionRadius * detectionRadius;

		return isNotInLineOfSight || isNotInRadius;
	}

	bool IsSearchingTooLong(Elite::Blackboard* pBlackboard)
	{
		const auto currentTime = std::chrono::duration_cast<std::chrono::seconds>
		(
			std::chrono::system_clock::now().time_since_epoch()
		);

		std::chrono::seconds lastTimeSeen{};
		float maxSearchTime{};

		pBlackboard->GetData("TimeLastSeen", lastTimeSeen);
		pBlackboard->GetData("MaxSearchTime", maxSearchTime);

		if((currentTime - lastTimeSeen).count() >= maxSearchTime)
		{
			return true;
		}

		return false;
	}

	bool ReachedSearchPoint(Elite::Blackboard* pBlackboard)
	{
		bool hasReachedSearchPoint{};
		pBlackboard->GetData("HasReachedLastSeenPoint", hasReachedSearchPoint);
		if(hasReachedSearchPoint)
		{
			return true;
		}

		SmartAgent* pGuard{};
		Elite::Vector2 target{};

		pBlackboard->GetData("LastKnownPos", target);
		pBlackboard->GetData("Agent", pGuard);

		assert(pGuard && "No guard! I fail Loudly");

		const float distance = DistanceSquared(pGuard->GetPosition(), target);

		if(distance < (pGuard->GetRadius() * pGuard->GetRadius()))
		{
			pBlackboard->ChangeData("HasReachedLastSeenPoint", true);
			return true;
		}
		
		return false;
	}
}

#endif