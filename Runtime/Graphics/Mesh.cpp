/*
Copyright(c) 2016-2017 Panos Karabelas

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
copies of the Software, and to permit persons to whom the Software is furnished
to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#pragma once

//= INCLUDES ========================
#include "../Graphics/Mesh.h"
#include "../Logging/Log.h"
#include "../Core/GUIDGenerator.h"
#include "../FileSystem/FileSystem.h"
#include "../IO/Serializer.h"
//===================================

//= NAMESPACES ================
using namespace std;
using namespace Directus::Math;
//=============================

namespace Directus
{
	Mesh::Mesh()
	{
		// Mesh	
		m_id = GENERATE_GUID;
		m_name = DATA_NOT_ASSIGNED;
		m_vertexCount = 0;
		m_indexCount = 0;
		m_triangleCount = 0;
		m_boundingBox = BoundingBox();
		m_onUpdate = nullptr;
	}

	Mesh::~Mesh()
	{
		m_vertices.clear();
		m_indices.clear();
		m_name.clear();
		m_vertexCount = 0;
		m_indexCount = 0;
		m_triangleCount = 0;
	}

	//= IO =========================================================================
	void Mesh::Serialize()
	{
		Serializer::WriteSTR(m_id);
		Serializer::WriteSTR(m_gameObjID);
		Serializer::WriteSTR(m_modelID);
		Serializer::WriteSTR(m_name);
		Serializer::WriteInt(m_vertexCount);
		Serializer::WriteInt(m_indexCount);
		Serializer::WriteInt(m_triangleCount);

		for (const auto& vertex : m_vertices)
		{
			SaveVertex(vertex);
		}

		for (const auto& index : m_indices)
		{
			Serializer::WriteInt(index);
		}
	}

	void Mesh::Deserialize()
	{
		m_id = Serializer::ReadSTR();
		m_gameObjID = Serializer::ReadSTR();
		m_modelID = Serializer::ReadSTR();
		m_name = Serializer::ReadSTR();
		m_vertexCount = Serializer::ReadInt();
		m_indexCount = Serializer::ReadInt();
		m_triangleCount = Serializer::ReadInt();

		for (unsigned int i = 0; i < m_vertexCount; i++)
		{
			m_vertices.push_back(VertexPosTexNorTan());
			LoadVertex(m_vertices.back());
		}

		for (unsigned int i = 0; i < m_indexCount; i++)
		{
			m_indices.push_back(Serializer::ReadInt());
		}

		m_boundingBox.ComputeFromMesh(this);
	}

	void Mesh::SetVertices(const vector<VertexPosTexNorTan>& vertices)
	{
		m_vertices = vertices;
		m_vertexCount = (unsigned int)vertices.size();
	}

	void Mesh::SetIndices(const vector<unsigned>& indices)
	{
		m_indices = indices;
		m_indexCount = (unsigned int)indices.size();
		m_triangleCount = m_indexCount / 3;
	}

	//==============================================================================

	//= PROCESSING =================================================================
	void Mesh::Update()
	{
		m_boundingBox.ComputeFromMesh(this);

		if (m_onUpdate)
		{
			m_onUpdate();
		}
	}

	// This is attached to CreateBuffers() which is part of the MeshFilter component.
	// Whenever something changes, the buffers are auto-updated.
	void Mesh::SubscribeToUpdate(function<void()> function)
	{
		m_onUpdate = function;

		if (m_onUpdate)
		{
			m_onUpdate();
		}
	}

	void Mesh::SetScale(float scale)
	{
		SetScale(this, scale);
		Update();
	}
	//==============================================================================

	//= IO =========================================================================
	void Mesh::SaveVertex(const VertexPosTexNorTan& vertex)
	{
		Serializer::WriteFloat(vertex.position.x);
		Serializer::WriteFloat(vertex.position.y);
		Serializer::WriteFloat(vertex.position.z);

		Serializer::WriteFloat(vertex.uv.x);
		Serializer::WriteFloat(vertex.uv.y);

		Serializer::WriteFloat(vertex.normal.x);
		Serializer::WriteFloat(vertex.normal.y);
		Serializer::WriteFloat(vertex.normal.z);

		Serializer::WriteFloat(vertex.tangent.x);
		Serializer::WriteFloat(vertex.tangent.y);
		Serializer::WriteFloat(vertex.tangent.z);
	}

	void Mesh::LoadVertex(VertexPosTexNorTan& vertex)
	{
		vertex.position.x = Serializer::ReadFloat();
		vertex.position.y = Serializer::ReadFloat();
		vertex.position.z = Serializer::ReadFloat();

		vertex.uv.x = Serializer::ReadFloat();
		vertex.uv.y = Serializer::ReadFloat();

		vertex.normal.x = Serializer::ReadFloat();
		vertex.normal.y = Serializer::ReadFloat();
		vertex.normal.z = Serializer::ReadFloat();

		vertex.tangent.x = Serializer::ReadFloat();
		vertex.tangent.y = Serializer::ReadFloat();
		vertex.tangent.z = Serializer::ReadFloat();
	}
	//==============================================================================

	//= HELPER FUNCTIONS ===========================================================
	void Mesh::SetScale(Mesh* meshData, float scale)
	{
		for (unsigned int i = 0; i < meshData->GetVertexCount(); i++)
		{
			meshData->GetVertices()[i].position *= scale;
		}
	}
	//==============================================================================
}