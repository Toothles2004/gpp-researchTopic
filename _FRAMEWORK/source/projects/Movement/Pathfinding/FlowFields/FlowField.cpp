#include "stdafx.h"
#include "FlowField.h"

Elite::FlowField::FlowField(int columns, int rows, int cellSize, bool isDirectionalGraph, bool isConnectedDiagonally, float costStraight, float costDiagonal, GraphNodeFactory* factory, ConnectionCostCalculator* pCostCalculator)
	: GridGraph(columns, rows, cellSize, isDirectionalGraph, isConnectedDiagonally, costStraight, costDiagonal, factory, pCostCalculator)
{
	
}

Elite::FlowField::~FlowField()
{
}

void Elite::FlowField::CalculateFlowField()
{
	CalculateDijkstraGrid();

	for(auto& node : m_pNodes)
	{
		if (node->GetDistance() == 0)
		{
			continue;
		}

		int closestDistance = INT_MAX;

		for (auto& neighborId : GetNeighbors(node->GetId(), true))
		{
			auto neighbor = m_pNodes[neighborId];

			if(neighbor->GetDistance() < closestDistance)
			{
				closestDistance = neighbor->GetDistance();
				node->SetVelocity((neighbor->GetPosition() - node->GetPosition()).GetNormalized());
			}
		}
	}
}

Elite::Vector2 Elite::FlowField::GetVelocityFromNode(int nodeIdx) const
{
	Elite::Vector2 result{};

	result = m_pNodes[nodeIdx]->GetVelocity();

	return result;
}

Elite::Vector2 Elite::FlowField::GetVelocityFromWorldPos(const Elite::Vector2& worldPos) const
{
	//1. Get the node index from the world position
	int nodeIdx{ GetNodeIdAtPosition(worldPos) };

	//2. Return the velocity from the node index
	return GetVelocityFromNode(nodeIdx);
}

void Elite::FlowField::SetTargetPos(const Elite::Vector2& targetPos)
{
	m_TargetPos = targetPos;
	CalculateFlowField();
}

void Elite::FlowField::AddNavigationCollider(Elite::Vector2 position)
{
	int nodeIdx{ GetNodeIdAtPosition(position) };
	m_ColliderIndex.push_back(nodeIdx);
	m_pNavigationColliders.push_back(std::make_unique<NavigationColliderElement>(GetNodePos(nodeIdx), static_cast<float>(m_CellSize), static_cast<float>(m_CellSize)));
	m_pNodes[nodeIdx]->SetColor(Color{ 1.f, 0.f, 0.f });
}

std::vector<int> Elite::FlowField::GetNeighbors(int index, bool diagonal) const
{
	std::vector<int> result{};

	//calculate vertical and horizontal increment
	const int columnIncrement = static_cast<int>(m_pNodes.size() / m_NrOfColumns);
	constexpr int rowIncrement = 1;

	//Check if top not out of bounds
	if (index - columnIncrement >= 0) 
	{
		result.push_back(index - columnIncrement);
	}

	//Check if bottom not out of bounds
	if (index + columnIncrement < m_pNodes.size()) 
	{
		result.push_back(index + columnIncrement);
	}

	//Check if left not out of bounds
	if ((index - rowIncrement + 1) % columnIncrement != 0) 
	{
		result.push_back(index - rowIncrement);
	}

	//Check if right not out of bounds
	if ((index + rowIncrement) % columnIncrement != 0) 
	{
		result.push_back(index + rowIncrement);
	}

	if (diagonal)
	{
		//Check if top left not out of bounds
		if ((index - columnIncrement >= 0) && ((index - rowIncrement + 1) % columnIncrement != 0)) 
		{
			if (!((m_pNodes[index - columnIncrement]->GetDistance() == INT_MAX) && (m_pNodes[index - rowIncrement]->GetDistance() == INT_MAX)))
			{
				result.push_back(index - columnIncrement - rowIncrement);
			}
		}

		//Check if top right not out of bounds
		if ((index - columnIncrement >= 0) && ((index + rowIncrement) % columnIncrement != 0)) 
		{
			if (!((m_pNodes[index - columnIncrement]->GetDistance() == INT_MAX) && (m_pNodes[index + rowIncrement]->GetDistance() == INT_MAX)))
			{
				result.push_back(index - columnIncrement + rowIncrement);
			}
		}

		//Check if bottom left not out of bounds
		if ((index + columnIncrement < m_pNodes.size()) && ((index - rowIncrement + 1) % columnIncrement != 0)) 
		{
			if (!((m_pNodes[index + columnIncrement]->GetDistance() == INT_MAX) && (m_pNodes[index - rowIncrement]->GetDistance() == INT_MAX)))
			{
				result.push_back(index + columnIncrement - rowIncrement);
			}
		}

		//Check if bottom right not out of bounds
		if ((index + columnIncrement < m_pNodes.size()) && ((index + rowIncrement) % columnIncrement != 0))
		{
			if (!((m_pNodes[index + columnIncrement]->GetDistance() == INT_MAX) && (m_pNodes[index + rowIncrement]->GetDistance() == INT_MAX)))
			{
				result.push_back(index + columnIncrement + rowIncrement);
			}
		}
	}

	return result;
}

void Elite::FlowField::CalculateDijkstraGrid()
{
	//Generate an empty grid, set all places as weight null, which will stand for unvisited
	for(auto& node : m_pNodes)
	{
		node->SetVisited(false);
		node->SetDistance(0);
		node->SetVelocity({ 0, 0 });
	}

	//Set all places where walls are as being weight INT_MAX, which stands for unreachable
	for (const auto& collider : m_ColliderIndex)
	{
		m_pNodes[collider]->SetVisited(true);
		m_pNodes[collider]->SetDistance(INT_MAX);
		m_pNodes[collider]->SetVelocity({ 0, 0 });
	}

	//flood fill out from the end point
	std::deque<int> toVisit{};
	toVisit.push_back(GetNodeIdAtPosition(m_TargetPos));

	m_pNodes[toVisit.front()]->SetDistance(0);
	m_pNodes[toVisit.front()]->SetVisited(true);
	m_pNodes[toVisit.front()]->SetVelocity({ 0, 0 });

	//for each node we need to visit, starting with the pathEnd
	while(!toVisit.empty())
	{
		const int currentIdx = toVisit.front();

		//get neighbors
		std::vector<int> neighbors = GetNeighbors(currentIdx, false);
		toVisit.pop_front();

		//for each neighbor of node
		for (const auto& neighbor : neighbors)
		{
			if (m_pNodes[neighbor]->GetVisited() == false)
			{
				m_pNodes[neighbor]->SetDistance(m_pNodes[currentIdx]->GetDistance() + 1);
				m_pNodes[neighbor]->SetVisited(true);
				toVisit.push_back(neighbor);
			}
		}
	}
}
