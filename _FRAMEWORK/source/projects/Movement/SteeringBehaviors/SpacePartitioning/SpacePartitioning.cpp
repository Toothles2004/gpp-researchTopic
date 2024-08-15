#include "stdafx.h"
#include "SpacePartitioning.h"
#include "projects\Movement\SteeringBehaviors\SteeringAgent.h"

// --- Cell ---
// ------------
Cell::Cell(float left, float bottom, float width, float height)
{
	boundingBox.bottomLeft = Elite::Vector2{ left, bottom };
	boundingBox.width = width;
	boundingBox.height = height;
}

std::vector<Elite::Vector2> Cell::GetRectPoints() const
{
	auto left = boundingBox.bottomLeft.x;
	auto bottom = boundingBox.bottomLeft.y;
	auto width = boundingBox.width;
	auto height = boundingBox.height;

	std::vector<Elite::Vector2> rectPoints =
	{
		{ left , bottom  },
		{ left , bottom + height  },
		{ left + width , bottom + height },
		{ left + width , bottom  },
	};

	return rectPoints;
}

// --- Partitioned Space ---
// -------------------------
CellSpace::CellSpace(float width, float height, int rows, int cols, int maxEntities)
	: m_SpaceWidth(width)
	, m_SpaceHeight(height)
	, m_NrOfRows(rows)
	, m_NrOfCols(cols)
	, m_Neighbors(maxEntities)
	, m_NrOfNeighbors(0)
{
	//Calculate bounds of a cell
	m_CellWidth = width / cols;
	m_CellHeight = height / rows;

	//TODO: create the cells
	int totalCells{ m_NrOfRows * m_NrOfCols };
	m_Cells.reserve(totalCells);
	for(int index{}; index < totalCells; ++index)
	{
		m_Cells.emplace_back(static_cast<float>(index % m_NrOfCols) * m_CellWidth, static_cast<float>(index / m_NrOfRows) * m_CellHeight, m_CellWidth, m_CellHeight);
	}
}

void CellSpace::AddAgent(SteeringAgent* agent)
{
	//Add the agent to the correct cell
	m_Cells[PositionToIndex(agent->GetPosition())].agents.push_back(agent);
}

void CellSpace::AgentPositionChanged(SteeringAgent* agent, Elite::Vector2 oldPos)
{
	// TODO: Check if the agent needs to be moved to another cell.
	// Use the calculated index for oldPos and currentPos for this
	int oldIndex = PositionToIndex(oldPos);
	int newIndex{ PositionToIndex(agent->GetPosition()) };

	if(oldIndex == newIndex)
	{
		return;
	}

	m_Cells[oldIndex].agents.remove(agent);
	AddAgent(agent);
}

void CellSpace::RegisterNeighbors(SteeringAgent* pAgent, float neighborhoodRadius)
{
	//TODO: Register the neighbors for the provided agent
	//Only check the cells that are within the radius of the neighborhood
	m_NrOfNeighbors = 0;

	Elite::Rect agentRect{Elite::Vector2{pAgent->GetPosition().x - (neighborhoodRadius), pAgent->GetPosition().y - neighborhoodRadius},neighborhoodRadius*2,neighborhoodRadius*2 };
	m_AgentRect = agentRect;
	for(int index{}; index < m_Cells.size(); ++index)
	{
		if(Elite::IsOverlapping(m_AgentRect, m_Cells[index].boundingBox))
		{
			for(const auto& agent : m_Cells[index].agents)
			{
				if (&agent == &pAgent) continue;
				const auto distanceSqrt = pAgent->GetPosition().DistanceSquared(agent->GetPosition());
				if (distanceSqrt < neighborhoodRadius * neighborhoodRadius)
				{
					m_Neighbors[m_NrOfNeighbors] = agent;
					++m_NrOfNeighbors;
				}
			}
		}
	}
	
}

void CellSpace::EmptyCells()
{
	for (Cell& c : m_Cells)
		c.agents.clear();
}

void CellSpace::RenderCells() const
{
	//TODO: Render the cells with the number of agents inside of it
	std::vector<Elite::Vector2> rectPoints{};

	for(int index{}; index < static_cast<int>(m_Cells.size()); ++index)
	{
		rectPoints = m_Cells[index].GetRectPoints();
		DEBUGRENDERER2D->DrawPolygon(&rectPoints[0], 4, {0.5f, 0.f, 0.f, 1.f}, 0.4f);
		int nrAgents{ static_cast<int>(m_Cells[index].agents.size()) };
		Elite::Vector2 pos{ m_Cells[index].boundingBox.bottomLeft.x, m_Cells[index].boundingBox.bottomLeft.y + m_CellHeight };
		DEBUGRENDERER2D->DrawString(pos, std::to_string(nrAgents).c_str());
	}

	for(int index{}; index < m_NrOfNeighbors; ++index)
	{
		DEBUGRENDERER2D->DrawCircle(
			m_Neighbors[index]->GetPosition(),
			m_Neighbors[index]->GetRadius() + 1,
			Elite::Color{ 0,0,255 },
			DEBUGRENDERER2D->NextDepthSlice());
	}

	rectPoints[0] = { m_AgentRect.bottomLeft };
	rectPoints[1] = { m_AgentRect.bottomLeft };
	rectPoints[2] = { m_AgentRect.bottomLeft };
	rectPoints[3] = { m_AgentRect.bottomLeft };
	rectPoints[1] += { m_AgentRect.width, 0};
	rectPoints[3] += { 0, m_AgentRect.height};
	rectPoints[2] += { m_AgentRect.width, m_AgentRect.height};

	DEBUGRENDERER2D->DrawPolygon(&rectPoints[0], 4, { 1.f,0.2f,1.f }, DEBUGRENDERER2D->NextDepthSlice());

}

int CellSpace::PositionToIndex(const Elite::Vector2 pos) const
{
	//TODO: Calculate the index of the cell based on the position
	int index = (static_cast<int>(pos.y / m_CellHeight) * m_NrOfCols) + static_cast<int>(pos.x / m_CellWidth);

	while(index < 0)
	{
		index += m_NrOfCols;
	}
	while(index >= m_NrOfRows * m_NrOfCols)
	{
		index -= m_NrOfCols;
	}

	return index;
}