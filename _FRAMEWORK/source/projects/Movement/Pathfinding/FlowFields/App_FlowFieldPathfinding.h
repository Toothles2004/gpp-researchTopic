#pragma once
#include "framework/EliteInterfaces/EIApp.h"
#include "projects/Movement/SteeringBehaviors/SteeringHelpers.h"
#include "framework/EliteAI/EliteGraphs/EliteGraphUtilities/EGraphEditor.h"
#include "framework/EliteAI/EliteGraphs/EliteGraphUtilities/EGraphRenderer.h"
#include "projects/Movement/Pathfinding/FlowFields/FlowField.h"
#include "projects/Movement/SteeringBehaviors/SteeringAgent.h"

//Forward declarations
class PathFollow;

class App_FlowFieldPathfinding final : public IApp
{
public:
	//Constructor & Destructor
	App_FlowFieldPathfinding();
	virtual ~App_FlowFieldPathfinding() override;

	//App Functions
	void Start() override;
	void Update(float deltaTime) override;
	void Render(float deltaTime) const override;

private:
	//Datamembers
	// --Agents--
	TargetData m_MouseTarget = {};
	bool m_UseMouseTarget = true;

	//Grid datamembers
	static const int m_COLUMNS = 20;
	static const int m_ROWS = 20;
	const int m_CellSize = 6;

	const int m_WorldWidth = m_COLUMNS * m_CellSize;
	const int m_WorldHeight = m_ROWS * m_CellSize;

	float m_TrimWorldSize = m_CellSize * m_COLUMNS;
	int m_FlockSize = 100;

	//std::vector<std::unique_ptr<SteeringAgent>> m_pAgents{};
	std::vector<SteeringAgent*> m_pAgents{};
	std::unique_ptr<Elite::GraphEditor> m_pGraphEditor{};
	std::unique_ptr<Elite::GraphRenderer> m_pGraphRenderer{};
	std::unique_ptr<Elite::FlowField> m_pFlowField{};

	//C++ make the class non-copyable
	App_FlowFieldPathfinding(const App_FlowFieldPathfinding&) = delete;
	App_FlowFieldPathfinding& operator=(const App_FlowFieldPathfinding&) = delete;
};

