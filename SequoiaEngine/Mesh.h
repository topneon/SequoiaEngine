#pragma once
#include "framework.h"

namespace seq {
	typedef DirectX::XMFLOAT3 Vertex;
	typedef uint32_t Indice;
	typedef DirectX::XMINT3 Triangle;
	typedef DirectX::XMFLOAT2 Coordinate;

	struct MeshData {
		Vertex* vertices;
		Indice* indices;
		// Triangle* triangles;
		Coordinate* coordinates;
	};

	class Mesh
	{
		MeshData data;
	public:
		// get
		Vertex* GetVertices();
		Indice* GetIndices();
		Triangle* GetTriangles();
		Coordinate* GetCoordinates();

		Vertex GetVertex(uint32_t i);
		Indice GetIndice(uint32_t i);
		Triangle GetTriangle(uint32_t i);
		Coordinate GetCoordinate(uint32_t i);

		// set
	};
}