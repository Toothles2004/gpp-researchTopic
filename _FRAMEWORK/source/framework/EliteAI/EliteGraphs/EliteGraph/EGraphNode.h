#pragma once
#include "EGraphEnums.h"
#include "../EliteGraphUtilities/EGraphVisuals.h"
namespace Elite
{
	class GraphNode
	{

	public:
		GraphNode(const Elite::Vector2& pos = Elite::ZeroVector2);
		virtual ~GraphNode() = default;

		int GetId() const { return m_Id; }

		Elite::Vector2 GetPosition() const { return m_Position; }
		void SetPosition(const Elite::Vector2& newPos) { m_Position = newPos; }

		Elite::Color GetColor() const { return m_Color; }
		void SetColor(const Elite::Color& color) { m_Color = color; }

		int GetDistance() const { return m_Distance; }
		void SetDistance(int distance) { m_Distance = distance; }

		bool GetVisited() const { return m_Visited; }
		void SetVisited(bool visited) { m_Visited = visited; }

		Vector2 GetVelocity() const { return m_Velocity; }
		void SetVelocity(const Vector2& velocity) { m_Velocity = velocity; }

	protected:
		int m_Id;
		Elite::Vector2 m_Position;
		Elite::Color m_Color;
		int m_Distance = 0;
		bool m_Visited = false;
		Vector2 m_Velocity = {0, 0};

		friend class Graph;
		void SetId(int id) { m_Id = id; }

	};



}