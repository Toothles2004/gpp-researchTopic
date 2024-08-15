#include "stdafx.h"
#include "App_FlowFieldPathfinding.h"

using namespace Elite;

App_FlowFieldPathfinding::App_FlowFieldPathfinding()
{
}

App_FlowFieldPathfinding::~App_FlowFieldPathfinding()
{
	for (auto& agent : m_pAgents)
	{
		SAFE_DELETE(agent);
	}
}

void App_FlowFieldPathfinding::Start()
{
	DEBUGRENDERER2D->GetActiveCamera()->SetZoom(55.0f);
	DEBUGRENDERER2D->GetActiveCamera()->SetCenter(Elite::Vector2(m_TrimWorldSize / 1.5f, m_TrimWorldSize / 2));

	for (int index{}; index < m_FlockSize; ++index)
	{
		SteeringAgent* agent = new SteeringAgent();
		Vector2 randomPos{ static_cast<float>(rand() % m_WorldWidth), static_cast<float>(rand() % m_WorldHeight)};
		agent->SetPosition(randomPos);
		agent->SetMaxLinearSpeed(10.f);
		agent->SetAutoOrient(true);
		agent->SetMass(1.f);

		m_pAgents.push_back(agent);
	}
	
	m_pFlowField = std::make_unique<FlowField>(m_COLUMNS, m_ROWS, m_CellSize, false, true);

	m_pGraphRenderer = std::make_unique<GraphRenderer>();
	m_pGraphRenderer->SetNumberPrintPrecision(0);

	m_pGraphEditor = std::make_unique<GraphEditor>();
}

void App_FlowFieldPathfinding::Update(float deltaTime)
{
	if (INPUTMANAGER->IsMouseButtonUp(InputMouseButton::eLeft))
	{
		auto const mouseData = INPUTMANAGER->GetMouseData(InputType::eMouseButton, InputMouseButton::eLeft);
		m_MouseTarget.Position = DEBUGRENDERER2D->GetActiveCamera()->ConvertScreenToWorld({ static_cast<float>(mouseData.X), static_cast<float>(mouseData.Y) });
		m_pFlowField->SetTargetPos(m_MouseTarget.Position);
	}

	if (INPUTMANAGER->IsMouseButtonUp(InputMouseButton::eRight))
	{
		auto const mouseData = INPUTMANAGER->GetMouseData(InputType::eMouseButton, InputMouseButton::eRight);
		m_MouseTarget.Position = DEBUGRENDERER2D->GetActiveCamera()->ConvertScreenToWorld({ static_cast<float>(mouseData.X), static_cast<float>(mouseData.Y) });
		m_pFlowField->AddNavigationCollider(m_MouseTarget.Position);
	}

	for (auto& agent : m_pAgents)
	{
		agent->TrimToWorld({ 0, 0 }, { m_TrimWorldSize, m_TrimWorldSize });
		agent->SetLinearVelocity(m_pFlowField->GetVelocityFromWorldPos(agent->GetPosition()) * agent->GetMaxLinearSpeed());
		agent->Update(deltaTime);
	}
}

void App_FlowFieldPathfinding::Render(float deltaTime) const
{
	RenderWorldBounds(m_TrimWorldSize);

	m_pGraphRenderer->RenderGraph(m_pFlowField.get(), true, true, false, false);

	for (auto& agent : m_pAgents)
	{
		agent->Render(deltaTime);
	}
}

