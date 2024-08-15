#pragma once
#include <stack>
#include "../EliteGraph/EGraph.h"
#include "../EliteGraph/EGraphConnection.h"
#include "../EliteGraph/EGraphNode.h"
namespace Elite
{
	enum class Eulerianity
	{
		notEulerian,
		semiEulerian,
		eulerian,
	};

	class EulerianPath
	{
	public:

		EulerianPath(Graph* pGraph);

		Eulerianity IsEulerian() const;
		std::vector<GraphNode*> FindPath(Eulerianity& eulerianity) const;

	private:
		void VisitAllNodesDFS(const std::vector<GraphNode*>& pNodes, std::vector<bool>& visited, int startIndex) const;
		bool IsConnected() const;

		Graph* m_pGraph;
	};

	inline EulerianPath::EulerianPath(Graph* pGraph)
		: m_pGraph(pGraph)
	{
	}

	inline Eulerianity EulerianPath::IsEulerian() const
	{

		// If the graph is not connected, there can be no Eulerian Trail
		if(!IsConnected())
		{
			return Eulerianity::notEulerian;
		}

		// Count nodes with odd degree 
		auto nodes = m_pGraph->GetAllNodes();
		int oddCount{};

		for(auto& node : nodes)
		{
			auto connections = m_pGraph->GetConnectionsFromNode(node);
			int amountConnections{ static_cast<int>(connections.size()) };

			//if (amountConnections%2 == 1)
			if(amountConnections & 1)     
			{
				++oddCount;
			}
		}

		// A connected graph with more than 2 nodes with an odd degree (an odd amount of connections) is not Eulerian
		if(oddCount > 2)
		{
			return Eulerianity::notEulerian;
		}

		// A connected graph with exactly 2 nodes with an odd degree is Semi-Eulerian (unless there are only 2 nodes)
		// An Euler trail can be made, but only starting and ending in these 2 nodes
		if (oddCount == 2)
		{
			return Eulerianity::semiEulerian;
		}


		// A connected graph with no odd nodes is Eulerian
		return Eulerianity::eulerian;

	}

	inline std::vector<GraphNode*> EulerianPath::FindPath(Eulerianity& eulerianity) const
	{
		// Get a copy of the graph because this algorithm involves removing edges
		auto graphCopy = m_pGraph->Clone();
		auto path = std::vector<GraphNode*>();
		int nrOfNodes = graphCopy->GetAmountOfNodes();

		GraphNode* currentNode{};

		// Check if there can be an Euler path
		switch (eulerianity)
		{
		case Eulerianity::eulerian:
			currentNode = graphCopy->GetNode(0);
			break;
		case Eulerianity::semiEulerian:
			for(const auto& node : graphCopy->GetAllNodes())
			{
				if(static_cast<int>(graphCopy->GetConnectionsFromNode(node).size()) & 1)
				{
					currentNode = node;
					break;
				}
			}
			break;
			// If this graph is not eulerian, return the empty path
		case Eulerianity::notEulerian:
			return path;
			break;

		}

		// Find a valid starting index for the algorithm

		// Start algorithm loop
		std::stack<int> nodeStack;

		while(!graphCopy->GetConnectionsFromNode(currentNode).empty() || !nodeStack.empty())
		{
			const std::vector<GraphConnection*> neighbors = graphCopy->GetConnectionsFromNode(currentNode);
			if(!neighbors.empty())
			{
				nodeStack.push(currentNode->GetId());
				currentNode = m_pGraph->GetNode(neighbors[0]->GetToNodeId());
				graphCopy->RemoveConnection(neighbors[0]->GetFromNodeId(), neighbors[0]->GetToNodeId());
			}
			else
			{
				path.push_back(currentNode);
				currentNode = m_pGraph->GetNode(nodeStack.top());
				nodeStack.pop();
			}
		}


		std::reverse(path.begin(), path.end()); // reverses order of the path
		return path;
	}


	inline void EulerianPath::VisitAllNodesDFS(const std::vector<GraphNode*>& pNodes, std::vector<bool>& visited, int startIndex ) const
	{
		// mark the visited node
		visited[startIndex] = true;
		
		for(const auto& connection : m_pGraph->GetConnectionsFromNode(startIndex))
		{
			for(int index{}; index < pNodes.size(); ++index)
			{
				if(pNodes[index]->GetId() == connection->GetToNodeId())
				{
					// recursively visit any valid connected nodes that were not visited before
					if (!visited[index])
					{
						VisitAllNodesDFS(pNodes, visited, index);
					}
				}
			}
		}
	}

	inline bool EulerianPath::IsConnected() const
	{
		auto nodes = m_pGraph->GetAllNodes();
		if (nodes.empty())
			return false;

		GraphNode* startingNode{ nodes[0] };

		// start a depth-first-search traversal from the node that has at least one connection
		std::vector<bool> visited{};
		visited.resize(nodes.size());
		VisitAllNodesDFS(nodes, visited, 0);

		// if a node was never visited, this graph is not connected
		for (const bool visit : visited)
		{
			if(visit == false)
			{
				return false;
			}
		}
		return true;
	}

}