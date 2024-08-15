#include "stdafx.h"
#include "Flock.h"

#include "../SteeringAgent.h"
#include "../Steering/SteeringBehaviors.h"
#include "projects/Movement/SteeringBehaviors/CombinedSteering/CombinedSteeringBehaviors.h"
#include "projects/Movement/SteeringBehaviors/SpacePartitioning/SpacePartitioning.h"

using namespace Elite;
//Constructor & Destructor
Flock::Flock(
	int flockSize /*= 50*/,
	float worldSize /*= 100.f*/,
	SteeringAgent* pAgentToEvade /*= nullptr*/,
	bool trimWorld /*= false*/)

	: m_WorldSize{ worldSize }
	, m_FlockSize{ flockSize }
	, m_TrimWorld{ trimWorld }
	, m_pAgentToEvade{ pAgentToEvade }
	, m_NeighborhoodRadius{ 15 }
	, m_NrOfNeighbors{ 0 }
{
	const int rows{ 20 };
	const int cols{ 20 };
	const int maxEntities{ m_FlockSize };

	m_CellSpace = new CellSpace(m_WorldSize, m_WorldSize, rows, cols, maxEntities);

	m_pSeparationBehavior = new Separation(this);
	m_pCohesionBehavior = new Cohesion(this);
	m_pVelMatchBehavior = new VelocityMatch(this);
	m_pSeekBehavior = new Seek();
	m_pWanderBehavior = new Wander();
	m_pEvadeBehavior = new Evade();

	m_pEvadeBehavior->SetEvadingRange(15.f);

	m_pBlendedSteering = new BlendedSteering
	({
		  { m_pSeparationBehavior, 0.5f },
		{ m_pCohesionBehavior, 0.5f},
		{ m_pVelMatchBehavior, 0.5f},
		{ m_pSeekBehavior, 0.5f},
		{ m_pWanderBehavior, 0.5f},
	});

	m_pPrioritySteering = new PrioritySteering
	({
		m_pEvadeBehavior,
		m_pBlendedSteering
	});

	m_Agents.resize(m_FlockSize);
	m_pAgentsOldPos.reserve(m_FlockSize);
	m_pAgentsOldPos.resize(m_FlockSize);
	//TODO: initialize the flock and the memory pool
	for (int index{}; index < m_FlockSize; ++index)
	{
		//m_Agents.emplace_back(new SteeringAgent());
		const Elite::Vector2 randPos = randomVector2(m_WorldSize).GetAbs();

		m_Agents[index] = new SteeringAgent();
		m_Agents[index]->SetMass(0.f);
		m_Agents[index]->SetPosition(randPos);
		m_Agents[index]->SetRotation(randomFloat());
		m_Agents[index]->SetMaxLinearSpeed(20.f);
		m_Agents[index]->SetAutoOrient(true);
		m_Agents[index]->SetSteeringBehavior(m_pPrioritySteering);

		m_CellSpace->AddAgent(m_Agents[index]);
		m_pAgentsOldPos[index] = randPos;
	}

	m_pAgentToEvade = new SteeringAgent();
	m_pAgentToEvade->SetMass(5.f);
	m_pAgentToEvade->SetMaxLinearSpeed(15.f);
	m_pAgentToEvade->SetPosition({ m_WorldSize / 2.f, m_WorldSize / 2.f });
	m_pAgentToEvade->SetRotation(randomFloat());
	m_pAgentToEvade->SetSteeringBehavior(m_pWanderBehavior);
	m_pAgentToEvade->SetBodyColor(Color{ 1.f, 0.f, 0.f });
	m_pAgentToEvade->SetAutoOrient(true);

	m_Neighbors.resize(m_FlockSize - 1);


	//std::fill(m_pAgentsOldPos.begin(), m_pAgentsOldPos.end(), Elite::Vector2{0.f, 0.f});
}

Flock::~Flock()
{
	//TODO: Cleanup any additional data

	SAFE_DELETE(m_pBlendedSteering);
	SAFE_DELETE(m_pPrioritySteering);

	SAFE_DELETE(m_pSeparationBehavior);
	SAFE_DELETE(m_pCohesionBehavior);
	SAFE_DELETE(m_pVelMatchBehavior);
	SAFE_DELETE(m_pSeekBehavior);
	SAFE_DELETE(m_pWanderBehavior);
	SAFE_DELETE(m_pEvadeBehavior);

	SAFE_DELETE(m_pAgentToEvade);
	SAFE_DELETE(m_CellSpace);

	for(auto pAgent: m_Agents)
	{
		SAFE_DELETE(pAgent);
	}
	m_Agents.clear();
	m_Neighbors.clear();
}

void Flock::Update(float deltaT)
{
	// TODO: update the flock
	// for every agent:
		// register the neighbors for this agent (-> fill the memory pool with the neighbors for the currently evaluated agent)
		// update the agent (-> the steeringbehaviors use the neighbors in the memory pool)
		// trim the agent to the world
	TargetData target
	{
		m_pAgentToEvade->GetPosition(),
		m_pAgentToEvade->GetRotation(),
		m_pAgentToEvade->GetLinearVelocity(),
		m_pAgentToEvade->GetAngularVelocity()
	};

	m_pEvadeBehavior->SetTarget(target);

	for(int index{}; index < m_FlockSize; ++index)
	{
		RegisterNeighbors(m_Agents[index]);
		m_Agents[index]->Update(deltaT);
		m_Agents[index]->TrimToWorld(m_WorldSize);

		if (m_SpatialPartitioning)
		{
			m_CellSpace->AgentPositionChanged(m_Agents[index], m_pAgentsOldPos[index]);
			m_pAgentsOldPos[index] = m_Agents[index]->GetPosition();
		}
	}

	m_pAgentToEvade->Update(deltaT);
	m_pAgentToEvade->TrimToWorld(m_WorldSize);
}

void Flock::Render(float deltaT)
{
	// TODO: Render all the agents in the flock
	for(const auto& pAgent : m_Agents)
	{
		if(pAgent != nullptr)
		{
			pAgent->Render(deltaT);
		}
	}
	if(m_pAgentToEvade)
	{
		m_pAgentToEvade->Render(deltaT);
	}
	
	if (m_RenderNeighborhood) //TODO: switch with imGUI checkbox
		RenderNeighborhood();
	else
	{
		m_Agents[0]->SetDebugRenderingEnabled(false);
	}

	if (m_SpatialPartitioning)
		m_CellSpace->RenderCells();
}

void Flock::UpdateAndRenderUI()
{
	//Setup
	int menuWidth = 235;
	int const width = DEBUGRENDERER2D->GetActiveCamera()->GetWidth();
	int const height = DEBUGRENDERER2D->GetActiveCamera()->GetHeight();
	bool windowActive = true;
	ImGui::SetNextWindowPos(ImVec2((float)width - menuWidth - 10, 10));
	ImGui::SetNextWindowSize(ImVec2((float)menuWidth, (float)height - 20));
	ImGui::Begin("Gameplay Programming", &windowActive, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
	ImGui::PushAllowKeyboardFocus(false);

	//Elements
	ImGui::Text("CONTROLS");
	ImGui::Indent();
	ImGui::Text("LMB: place target");
	ImGui::Text("RMB: move cam.");
	ImGui::Text("Scrollwheel: zoom cam.");
	ImGui::Unindent();

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();
	ImGui::Spacing();

	ImGui::Text("STATS");
	ImGui::Indent();
	ImGui::Text("%.3f ms/frame", 1000.0f / ImGui::GetIO().Framerate);
	ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);
	ImGui::Unindent();

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	ImGui::Text("Flocking");
	ImGui::Spacing();

	//TODO: implement ImGUI checkboxes for debug rendering here
	ImGui::Checkbox("Debug rendering", &m_RenderNeighborhood);
	ImGui::Text("Behavior Weights");
	ImGui::Spacing();
	ImGui::Checkbox("Spatial partitioning", &m_SpatialPartitioning);
	ImGui::Spacing();

	//TODO: implement ImGUI sliders for steering behavior weights here
	ImGui::SliderFloat("Separation", &m_pBlendedSteering->GetWeightedBehaviorsRef()[0].weight, 0.f, 1.f, "%.2");
	ImGui::SliderFloat("Cohesion", &m_pBlendedSteering->GetWeightedBehaviorsRef()[1].weight, 0.f, 1.f, "%.2");
	ImGui::SliderFloat("Velocity", &m_pBlendedSteering->GetWeightedBehaviorsRef()[2].weight, 0.f, 1.f, "%.2");
	ImGui::SliderFloat("Seek", &m_pBlendedSteering->GetWeightedBehaviorsRef()[3].weight, 0.f, 1.f, "%.2");
	ImGui::SliderFloat("Wander", &m_pBlendedSteering->GetWeightedBehaviorsRef()[4].weight, 0.f, 1.f, "%.2");

	//End
	ImGui::PopAllowKeyboardFocus();
	ImGui::End();
	
}

void Flock::RenderNeighborhood()
{
	// TODO: Implement
	// Register the neighbors for the first agent in the flock
	// DebugRender the neighbors in the memory pool
	RegisterNeighbors(m_Agents[0]);
	m_Agents[0]->SetDebugRenderingEnabled(true);

	DEBUGRENDERER2D->DrawCircle(m_Agents[0]->GetPosition(), m_NeighborhoodRadius, Color{ 0.f, 1.f, 1.f }, DEBUGRENDERER2D->NextDepthSlice());
	for(int index{}; index < m_NrOfNeighbors; ++index)
	{
		DEBUGRENDERER2D->DrawSolidCircle
		(
			m_Neighbors[index]->GetPosition(), 
			m_Neighbors[index]->GetRadius(), 
			m_Neighbors[index]->GetLinearVelocity().GetNormalized(), 
			Color{ 0.f, 1.f, 0.f }
		);
	}
}


void Flock::RegisterNeighbors(SteeringAgent* pAgent)
{
	if(m_SpatialPartitioning)
	{
		m_CellSpace->RegisterNeighbors(pAgent, m_NeighborhoodRadius);
		m_NrOfNeighbors = m_CellSpace->GetNrOfNeighbors();
		m_Neighbors = m_CellSpace->GetNeighbors();
		return;
	}

	m_NrOfNeighbors = 0;

	// TODO: Implement
	for(const auto& agent : m_Agents)
	{
		if (&agent == &pAgent) continue;
		const auto distanceSqrt = pAgent->GetPosition().DistanceSquared(agent->GetPosition());
		if(distanceSqrt < m_NeighborhoodRadius * m_NeighborhoodRadius)
		{
			m_Neighbors[m_NrOfNeighbors] = agent;
			++m_NrOfNeighbors;
		}
	}
}

int Flock::GetNrOfNeighbors() const
{
	if(m_SpatialPartitioning)
	{
		return m_CellSpace->GetNrOfNeighbors();
	}
	return m_NrOfNeighbors;
}

const std::vector<SteeringAgent*>& Flock::GetNeighbors() const
{
	if (m_SpatialPartitioning)
	{
		return m_CellSpace->GetNeighbors();
	}
	return m_Neighbors;
}

Vector2 Flock::GetAverageNeighborPos() const
{
	Vector2 avgPosition = Elite::ZeroVector2;

	// TODO: Implement
	for (int index{}; index < m_NrOfNeighbors; ++index)
	{
		avgPosition += m_Neighbors[index]->GetPosition();
	}

	avgPosition /= static_cast<float>(m_NrOfNeighbors);

	return avgPosition;
}

Vector2 Flock::GetAverageNeighborVelocity() const
{
	Vector2 avgVelocity = Elite::ZeroVector2;
	
	// TODO: Implement
	for (int index{}; index < m_NrOfNeighbors; ++index)
	{
		avgVelocity += m_Neighbors[index]->GetLinearVelocity();
	}

	avgVelocity /= static_cast<float>(m_NrOfNeighbors);

	return avgVelocity;
}
void Flock::SetTarget_Seek(const TargetData& target)
{
	// TODO: Implement
	m_pSeekBehavior->SetTarget(target);
}

float* Flock::GetWeight(ISteeringBehavior* pBehavior) 
{
	if (m_pBlendedSteering)
	{
		auto& weightedBehaviors = m_pBlendedSteering->GetWeightedBehaviorsRef();
		auto it = find_if(weightedBehaviors.begin(),
			weightedBehaviors.end(),
			[pBehavior](BlendedSteering::WeightedBehavior el)
			{
				return el.pBehavior == pBehavior;
			}
		);

		if(it!= weightedBehaviors.end())
			return &it->weight;
	}

	return nullptr;
}
