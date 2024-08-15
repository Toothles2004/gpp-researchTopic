#include "stdafx.h"
#include "SandboxAgent.h"

using namespace Elite;

SandboxAgent::SandboxAgent(): BaseAgent()
{
	m_Target = GetPosition();
}

void SandboxAgent::Update(float dt)
{
	const float speed{1000.f};

	//TODO: set linear velocity towards m_Target
	Vector2 pos = GetPosition();
	Vector2 dir = m_Target - pos;
	Vector2 vel = dir.GetNormalized() * speed * dt;

	SetLinearVelocity(dir.GetNormalized() * speed * dt);

	DEBUGRENDERER2D->DrawSegment(pos, pos+vel, { 1.f,0.f,0.f });

	//Orientation
	AutoOrient();
}

void SandboxAgent::Render(float dt)
{
	BaseAgent::Render(dt); //Default Agent Rendering
}

void SandboxAgent::AutoOrient()
{
	//Determine angle based on direction
	Vector2 velocity = GetLinearVelocity();
	if (velocity.MagnitudeSquared() > 0)
	{
		SetRotation(VectorToOrientation(velocity));
	}
}