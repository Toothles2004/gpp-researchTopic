#pragma once
#include "framework/EliteAI/EliteGraphs/EliteGridGraph/EGridGraph.h"
#include "projects/Shared/NavigationColliderElement.h"

namespace Elite
{
	class FlowField : public GridGraph
	{
	public:
		FlowField(int columns, int rows, int cellSize, bool isDirectionalGraph, bool isConnectedDiagonally, float costStraight = 1.f, float costDiagonal = 1.5, GraphNodeFactory* factory = nullptr, ConnectionCostCalculator* pCostCalculator = nullptr);
		~FlowField();

		Elite::Vector2 GetVelocityFromNode(int nodeIdx) const;
		Elite::Vector2 GetVelocityFromWorldPos(const Elite::Vector2& worldPos) const;
		void SetTargetPos(const Elite::Vector2& targetPos);
		void AddNavigationCollider(Elite::Vector2 position);

	private:

		void CalculateFlowField();
		void CalculateDijkstraGrid();
		std::vector<int> GetNeighbors(int idx, bool Diagonal) const;

		Elite::Vector2 m_TargetPos;
		std::vector<std::unique_ptr<NavigationColliderElement>> m_pNavigationColliders;
		std::vector<int> m_ColliderIndex;
	};

}
