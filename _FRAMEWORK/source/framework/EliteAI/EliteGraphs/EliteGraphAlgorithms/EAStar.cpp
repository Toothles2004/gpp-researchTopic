#include "stdafx.h"
#include "EAStar.h"

using namespace Elite;
AStar::AStar(Graph* pGraph, Heuristic hFunction)
	: m_pGraph(pGraph)
	, m_HeuristicFunction(hFunction)
{
}

std::vector<GraphNode*>AStar::FindPath(GraphNode* pStartNode, const GraphNode* pEndNode) const
{
	std::vector<GraphNode*> path{};
	std::list<NodeRecord> openList;
	std::list<NodeRecord> closedList;
	NodeRecord currentNodeRecord{ pStartNode, nullptr, 0, GetHeuristicCost(pStartNode, pEndNode)};

	openList.push_back(currentNodeRecord);

	while(!openList.empty())
	{
		//what horror is this
		auto currentNodeRecordIt = std::min_element(openList.begin(), openList.end());
		currentNodeRecord = *currentNodeRecordIt;
		//currentNodeRecord = *std::min_element(openList.begin(), openList.end());

		if(currentNodeRecord.pNode == pEndNode)
		{
			break;
		}
		for( GraphConnection* connection : m_pGraph->GetConnectionsFromNode(currentNodeRecord.pNode))
		{
			bool inList{ false };

			GraphNode* pNextNode = m_pGraph->GetNode(connection->GetToNodeId());
			float cost = currentNodeRecord.costSoFar + connection->GetCost();

			for(const NodeRecord& nodeRecord : closedList)
			{
				if(nodeRecord.pNode == pNextNode)
				{
					if(nodeRecord.costSoFar < cost)
					{
						inList = true;
						break;
					}
					closedList.remove(nodeRecord);
					break;
				}
			}
			if(inList)
			{
				continue;
			}

			for (const NodeRecord& nodeRecord : openList)
			{
				if (nodeRecord.pNode == pNextNode)
				{
					if (nodeRecord.costSoFar < cost)
					{
						inList = true;
						break;
					}
					openList.remove(nodeRecord);
					break;
				}
			}
			if (inList)
			{
				continue;
			}

			openList.push_back(NodeRecord{ pNextNode, connection, cost, GetHeuristicCost(pNextNode, pEndNode) + cost });
		}
		
		openList.remove(currentNodeRecord);
		closedList.push_back(currentNodeRecord);
	}
	
	while(currentNodeRecord.pNode != pStartNode)
	{
		path.push_back(currentNodeRecord.pNode);

		for(const NodeRecord& nodeRecord : closedList)
		{
			if(nodeRecord.pNode->GetId() == currentNodeRecord.pConnection->GetFromNodeId())
			{
				currentNodeRecord = nodeRecord;
				break;
			}
		}
	}

	path.push_back(pStartNode);
	std::ranges::reverse(path);
	
	return path;
}

float AStar::GetHeuristicCost(const GraphNode* pStartNode, const GraphNode* pEndNode) const
{
	const Vector2 toDestination = m_pGraph->GetNodePos(pEndNode->GetId()) - m_pGraph->GetNodePos(pStartNode->GetId());
	return m_HeuristicFunction(abs(toDestination.x), abs(toDestination.y));
}