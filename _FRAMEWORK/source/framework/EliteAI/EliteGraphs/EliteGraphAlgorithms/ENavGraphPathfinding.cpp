#include "stdafx.h"

#include "ENavGraphPathfinding.h"
#include "framework/EliteAI/EliteGraphs/EliteGraphAlgorithms/EAStar.h"
#include "framework/EliteAI/EliteGraphs/EliteNavGraph/ENavGraph.h"

using namespace Elite;

std::vector<Vector2> NavMeshPathfinding::FindPath(Vector2 startPos, Vector2 endPos, NavGraph* pNavGraph)
{
	std::vector<Vector2> debugNodePositions{};
	std::vector<Portal> debugPortals{};
	return FindPath(startPos, endPos, pNavGraph, debugNodePositions, debugPortals);
}

std::vector<Vector2> NavMeshPathfinding::FindPath(Vector2 startPos, Vector2 endPos, NavGraph* pNavGraph, std::vector<Vector2>& debugNodePositions, std::vector<Portal>& debugPortals)
{
	//Create the path to return
	std::vector<Vector2> finalPath{};

	//Get the startTriangle and endTriangle
	//If we don't have a valid startTriangle or endTriangle -> return empty path
	//If the startTriangle and endTriangle are the same -> return straight line path
	const Triangle* startTriangle = pNavGraph->GetNavMeshPolygon()->GetTriangleFromPosition(startPos);
	if (startTriangle == nullptr)
	{
		return finalPath;
	}
	const Triangle* endTriangle = pNavGraph->GetNavMeshPolygon()->GetTriangleFromPosition(endPos);
	if (endTriangle == nullptr)
	{
		return finalPath;
	}

	//=> Start looking for a path
	//Clone the graph (returns shared_ptr!)
	std::shared_ptr<NavGraph> graphClone = pNavGraph->Clone();

	//Create extra node for the Start Node (Agent's position) and add it to the graph. 
	//Make connections between the Start Node and the startTriangle nodes.
	const auto startNode = new NavGraphNode(-1,  startPos);
	const int startNodeIdx = graphClone->AddNode(startNode);

	for(const auto& edges: startTriangle->metaData.IndexLines)
	{
		const int id{ graphClone->GetNodeIdFromLineIndex(edges) };

		if(id != -1)
		{
			graphClone->AddConnection(new GraphConnection(startNodeIdx, id, Elite::Distance(startNode->GetPosition(), graphClone->GetNodePos(id))));
		}
	}

	//Create extra node for the End Node (endpos) and add it to the graph. 
	//Make connections between the End Node and the endTriangle nodes.
	const auto endNode = new NavGraphNode(-1, endPos);
	const int endNodeIdx = graphClone->AddNode(endNode);

	for (const auto& edges : endTriangle->metaData.IndexLines)
	{
		const int id{ graphClone->GetNodeIdFromLineIndex(edges) };

		if (id != -1)
		{
			graphClone->AddConnection(new GraphConnection(endNodeIdx, id, Elite::Distance(endNode->GetPosition(), graphClone->GetNodePos(id))));
		}
	}

	//Run AStar on the new graph
	const auto pathfinder = AStar(graphClone.get(), HeuristicFunctions::Euclidean);
	const auto nodes = pathfinder.FindPath(startNode, endNode);

	debugNodePositions.clear();
	for (const auto& path : nodes)
	{
		finalPath.push_back(path->GetPosition());
		debugNodePositions.push_back(path->GetPosition());
	}

	//Run optimiser on new graph, MAKE SURE the AStar path is working properly before starting the following section:
	std::vector<Portal> portals = SSFA::FindPortals(nodes, pNavGraph->GetNavMeshPolygon());
	finalPath = SSFA::OptimizePortals(portals);

	return finalPath;
}


