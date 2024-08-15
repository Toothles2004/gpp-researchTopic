//Precompiled Header [ALWAYS ON TOP IN CPP]
#include "stdafx.h"

//Includes
#include "SteeringBehaviors.h"
#include "../SteeringAgent.h"
#include "../Obstacle.h"
#include "framework\EliteMath\EMatrix2x3.h"
#include <limits>

using namespace Elite;

//SEEK
//****
SteeringOutput Seek::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering = {};
	
	Vector2 currentPos = pAgent->GetPosition();
	Vector2 targetPos = m_Target.Position;

	Vector2 dir = targetPos - currentPos;
	dir.Normalize();

	steering.LinearVelocity = dir * pAgent->GetMaxLinearSpeed();

	if (pAgent->GetDebugRenderingEnabled())
	{
		DEBUGRENDERER2D->DrawDirection(currentPos, steering.LinearVelocity, 5.f, { 1.f, 0.f, 0.f });
	}

	return steering;
}

//Flee
//****
SteeringOutput Flee::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering = {};

	Vector2 currentPos = pAgent->GetPosition();
	Vector2 targetPos = m_Target.Position;

	Vector2 dir = targetPos - currentPos;
	dir.Normalize();

	steering.LinearVelocity = -dir * pAgent->GetMaxLinearSpeed();

	if (pAgent->GetDebugRenderingEnabled())
	{
		DEBUGRENDERER2D->DrawDirection(currentPos, steering.LinearVelocity, 5.f, { 1.f, 0.f, 0.f });
	}

	return steering;
}

//Arrive
//****
SteeringOutput Arrive::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering = {};

	Vector2 currentPos = pAgent->GetPosition();
	Vector2 targetPos = m_Target.Position;

	Vector2 toTarget = targetPos - currentPos;

	float speed = pAgent->GetMaxLinearSpeed();

	float distance = toTarget.Magnitude();
	float a = distance - m_TargetRadius;
	float b = m_SlowRadius - m_TargetRadius;

	if (distance < m_SlowRadius)
	{
		speed *= (a / b);
	}

	steering.LinearVelocity = toTarget.GetNormalized() * speed;

	if (pAgent->GetDebugRenderingEnabled())
	{
		DEBUGRENDERER2D->DrawDirection(currentPos, steering.LinearVelocity, 5.f, { 1.f, 0.f, 0.f });
		DEBUGRENDERER2D->DrawCircle(currentPos, m_SlowRadius, { 0.f, 0.f, 1.f }, DEBUGRENDERER2D->NextDepthSlice());
		DEBUGRENDERER2D->DrawCircle(currentPos, m_TargetRadius, { 0.f, 1.f, 0.f }, DEBUGRENDERER2D->NextDepthSlice());

	}

	return steering;
}

SteeringOutput Face::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	Vector2 currentPos = pAgent->GetPosition();
	Vector2 targetPos = m_Target.Position;

	Vector2 toTarget = targetPos - currentPos;

	SteeringOutput  steering{};
	pAgent->SetAutoOrient(false);
	float angle = VectorToOrientation(toTarget) - pAgent->GetRotation();

	if (angle <= -static_cast<float>(E_PI)) 
	{
		angle += 2 * static_cast<float>(E_PI);
	}
	if (angle > static_cast<float>(E_PI)) 
	{
		angle -= 2 * static_cast<float>(E_PI);
	}
	
	if (angle > 0)
	{
		steering.AngularVelocity = pAgent->GetMaxAngularSpeed();
	}
	else
	{
		steering.AngularVelocity = -pAgent->GetMaxAngularSpeed();
	}

	return steering;
}

SteeringOutput Evade::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	Vector2 toTarget = m_Target.Position - pAgent->GetPosition();

	float distanceSq = toTarget.MagnitudeSquared();
	if (m_EvadeRad != 0)
	{
		if (distanceSq > m_EvadeRad * m_EvadeRad)
		{
			SteeringOutput output{};
			output.IsValid = false;

			if (pAgent->GetDebugRenderingEnabled())
			{
				DEBUGRENDERER2D->DrawCircle(pAgent->GetPosition(), m_EvadeRad, { 1.f, 0.f, 0.f }, DEBUGRENDERER2D->NextDepthSlice());
			}

			return output;
		}
	}

	SteeringOutput steering = { Pursuit::CalculateSteering(deltaT, pAgent)};

	steering.LinearVelocity *= -1;

	return steering;
}

SteeringOutput Pursuit::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	SteeringOutput steering = {};

	TargetData target{ m_Target };

	const float distance{ pAgent->GetPosition().Distance(m_Target.Position)};
	target.Position += (m_Target.LinearVelocity.GetNormalized() * distance);

	steering.LinearVelocity = target.Position - pAgent->GetPosition();
	steering.LinearVelocity.Normalize() ;
	steering.LinearVelocity *= pAgent->GetMaxLinearSpeed();

	if (pAgent->GetDebugRenderingEnabled())
	{
		DEBUGRENDERER2D->DrawDirection(pAgent->GetPosition(), steering.LinearVelocity, 5.f, {0.f, 0.f, 1.f});
		DEBUGRENDERER2D->DrawCircle(target.Position, 3.f, { 1.f, 0.f, 0.f }, DEBUGRENDERER2D->NextDepthSlice());
	}

	return steering;
}

//SteeringOutput Hide::CalculateSteering(float deltaT, SteeringAgent* pAgent)
//{
//
//
//	return;
//}
//
//SteeringOutput AvoidObstacle::CalculateSteering(float deltaT, SteeringAgent* pAgent)
//{
//
//
//	return;
//}
 
SteeringOutput Wander::CalculateSteering(float deltaT, SteeringAgent* pAgent)
{
	m_WanderAngle += (Elite::randomFloat(0, 1) * (m_MaxAngleChange)) - (m_MaxAngleChange / 2.f);
	Elite::ClampRef(m_WanderAngle, Elite::ToRadians(-90), Elite::ToRadians(90));

	const Elite::Vector2 wanderCircleCenter{ pAgent->GetPosition() + pAgent->GetLinearVelocity().GetNormalized() * m_OffsetDistance };
	const Elite::Vector2 vecFromCenter{ cosf(m_WanderAngle) * m_Radius,sinf(m_WanderAngle) * m_Radius };

	m_Target.Position = (wanderCircleCenter + vecFromCenter);

	if (pAgent->GetDebugRenderingEnabled())
	{
		DEBUGRENDERER2D->DrawCircle(wanderCircleCenter, m_Radius, { 0.f, 0.f, 1.f, 0.5f }, 0.40f);
		DEBUGRENDERER2D->DrawSolidCircle(m_Target.Position, 0.1f, {}, { 0.f, 1.f, 0.f }, 0.40f);
		DEBUGRENDERER2D->DrawDirection(pAgent->GetPosition(), pAgent->GetLinearVelocity(), m_OffsetDistance, {0.f, 0.f, 1.f, 0.5f}, 0.40f);
		DEBUGRENDERER2D->DrawDirection(pAgent->GetPosition(), pAgent->GetLinearVelocity(), pAgent->GetLinearVelocity().Magnitude(), { 1.f, 0.f, 1.f, 0.5f }, 0.40f);
	}
	
	return Seek::CalculateSteering(deltaT, pAgent);
}

//AvoidObstacle::AvoidObstacle(std::vector<Obstacle*> obstacles)
//	: m_Obstacles{ obstacles }
//{
//}

//Hide::Hide(std::vector<Obstacle*> obstacles)
//	: m_Obstacles{ obstacles }
//{
//}
