#include "stdafx.h"

#include "EInfluenceMap.h"

using namespace Elite;

InfluenceMap::InfluenceMap(int columns, int rows, float cellSize) :
	m_NumColumns{ columns },
	m_NumRows{ rows },
	m_CellSize{ cellSize }
{
	m_InfluenceBackBuffer.resize(columns * rows, 0.f);
	m_InfluenceFrontBuffer.resize(columns * rows, 0.f);
}

InfluenceMap::~InfluenceMap()
{
}

void InfluenceMap::SetInfluenceAtPosition(Elite::Vector2& pos, float influence, bool additive)
{
	auto idx = GetIndexAtPosition(pos);
	if (additive)
	{
		m_InfluenceFrontBuffer[idx] += influence;
	}
	else
	{
		m_InfluenceFrontBuffer[idx] = influence;
	}
}

float InfluenceMap::GetInfluenceAtPosition(Elite::Vector2& pos) const
{
	if (!IsInBounds(pos))return -1.f;

	auto idx = GetIndexAtPosition(pos);
	return m_InfluenceFrontBuffer[idx];
}


void InfluenceMap::Update(float deltaTime)
{
	//Propagation interval
	m_TimeSinceLastPropagation += deltaTime;
	if (m_TimeSinceLastPropagation < m_PropagationInterval)
		return;
	m_TimeSinceLastPropagation = 0.f;

	//Frontbuffer and Backbuffer have same size
	//Always read from frontbuffer and write to backbuffer!!

	//Index based for-loop over all cells
	//for each idx, calculate the maxInfluence of the neighbors (3x3 grid around cell)
	for(int index{}; index < m_InfluenceFrontBuffer.size(); ++index)
	{
		//	- Get neighboring indices (TIP: use GetNeighboringIndices method)
		//  - For each neighbor idx
		for( const int neighbor : GetNeighboringIndices(index))
		{
			//- if index == -1 => invalid index (edge of map)
			if(neighbor == -1)
			{
				continue;
			}

			//- sample the influence on that index (frontbuffer)
			const float sampledInfluence{ m_InfluenceFrontBuffer[neighbor] };

			//- calculate the cost to that neighbor (m_influenceCosts array)
			const float cost{ m_influenceCosts[neighbor]};

			//- calculate the influence of that neighbor:
			//  influence = sampled_influence * expf( -cost * Decay)
			float influence = sampledInfluence * expf(-cost * m_Decay);

			//- keep the maxInfluence (based on highest absolute value!!)
			const float maxInfluence = abs(influence) < m_MaxAbsInfluence ? influence : m_MaxAbsInfluence;

			//- maxInfluence is interpolated with current influence based on momentum:
			//	newInfluence = Lerp(maxInfluence, oldInfluence, Momentum)
			const float newInfluence = Lerp(maxInfluence, sampledInfluence, m_Momentum);

			//- write newInfluence to the BackBuffer
			m_InfluenceBackBuffer[neighbor] = newInfluence;
		}
	}

	SwapBuffers();
}

Color InfluenceMap::GetNodeColorFromInfluence(float influence) const
{
	const float half = .5f;

	Color nodeColor{};
	float relativeInfluence = abs(influence) / m_MaxAbsInfluence;

	if (influence < 0)
	{
		nodeColor = Elite::Color{
		Lerp(m_NeutralColor.r, m_NegativeColor.r, relativeInfluence),
		Lerp(m_NeutralColor.g, m_NegativeColor.g, relativeInfluence),
		Lerp(m_NeutralColor.b, m_NegativeColor.b, relativeInfluence)
		};
	}
	else
	{
		nodeColor = Elite::Color{
		Lerp(m_NeutralColor.r, m_PositiveColor.r, relativeInfluence),
		Lerp(m_NeutralColor.g, m_PositiveColor.g, relativeInfluence),
		Lerp(m_NeutralColor.b, m_PositiveColor.b, relativeInfluence)
		};
	}
	return nodeColor;
}
bool InfluenceMap::IsInBounds(Vector2& position) const
{
	int r, c;

	c = int(floorf(position.x / m_CellSize));
	r = int(floorf(position.y / m_CellSize));
	return c >= 0 && c < m_NumColumns && r >= 0 && r < m_NumRows;
}
int InfluenceMap::GetIndexAtPosition(Vector2& position) const
{
	int r, c;

	c = int(floorf(position.x / m_CellSize));
	r = int(floorf(position.y / m_CellSize));

	c = Elite::Clamp(c, 0, m_NumColumns - 1);
	r = Elite::Clamp(r, 0, m_NumRows - 1);

	return r * m_NumColumns + c;
}

std::vector<int> Elite::InfluenceMap::GetNeighboringIndices(int nodeIdx) const
{
	std::vector<int> indices{};
	indices.resize(9);

	int row = nodeIdx / m_NumColumns;
	int col = nodeIdx % m_NumColumns;

	int idx = 0;
	for (int r = -1; r < 2; ++r)
	{
		int currRow = row + r;
		if (currRow < 0 || currRow >= m_NumRows)
		{
			indices[idx++] = -1;
			indices[idx++] = -1;
			indices[idx++] = -1;
			continue;
		}

		for (int c = -1; c < 2; ++c)
		{
			int currCol = col + c;

			if (c == 0 && r == 0)
			{
				indices[idx++] = -1;
				continue;
			}

			if (currCol < 0 || currCol >= m_NumColumns)
				indices[idx++] = -1;
			else
				indices[idx++] = currRow * m_NumColumns + currCol;
		}
	}
	return indices;
}
void InfluenceMap::Render() const
{
	float depth = DEBUGRENDERER2D->NextDepthSlice();

	float halfSize = m_CellSize * .5f;
	int idx;
	Color col;
	Vector2 pos;
	Vector2 verts[4];
	for (int r = 0; r < m_NumRows; ++r)
	{
		pos.y = r * m_CellSize;
		for (int c = 0; c < m_NumColumns; ++c)
		{
			idx = r * m_NumColumns + c;
			col = GetNodeColorFromInfluence(m_InfluenceFrontBuffer[idx]);
			pos.x = c * m_CellSize;

			verts[0] = Vector2(pos.x, pos.y);
			verts[1] = Vector2(pos.x, pos.y + m_CellSize);
			verts[2] = Vector2(pos.x + m_CellSize, pos.y + m_CellSize);
			verts[3] = Vector2(pos.x + m_CellSize, pos.y);
			DEBUGRENDERER2D->DrawSolidPolygon(&verts[0], 4, col, depth);
			//DEBUGRENDERER2D->DrawPolygon(&verts[0], 4, { .2f,.2f,.2f }, depth);

		};

	}
}

void InfluenceMap::SwapBuffers()
{
	m_InfluenceBackBuffer.swap(m_InfluenceFrontBuffer);
}