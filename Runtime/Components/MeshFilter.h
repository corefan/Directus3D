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

//= INCLUDES =========================
#include "Component.h"
#include <vector>
#include <memory>
#include "../FileSystem/FileSystem.h"
#include "../Math/BoundingBox.h"
#include "../Graphics/Vertex.h"

//===================================

namespace Directus
{
	class Mesh;
	class D3D11VertexBuffer;
	class D3D11IndexBuffer;
	struct LoadVertices;
	namespace Math
	{
		class Vector3;
		class BoundingBox;
	}

	class DLL_API MeshFilter : public Component
	{
	public:
		enum MeshType { Imported, Cube, Quad };

		MeshFilter();
		~MeshFilter();

		//= ICOMPONENT =============
		virtual void Reset();
		virtual void Start();
		virtual void OnDisable();
		virtual void Remove();
		virtual void Update();
		virtual void Serialize();
		virtual void Deserialize();
		//=========================

		// Sets a mesh from memory
		bool SetMesh(std::weak_ptr<Mesh> mesh);

		// Sets a default mesh (cube, quad)
		bool SetMesh(MeshType defaultMesh);

		// Sets the meshe's buffers
		bool SetBuffers();

		//= BOUNDING BOX =============================
		Math::BoundingBox GetBoundingBox();
		Math::BoundingBox GetBoundingBoxTransformed();
		//============================================

		//= PROPERTIES ===========================================
		std::string GetMeshName();
		const std::weak_ptr<Mesh>& GetMesh() { return m_mesh; }
		bool HasMesh() { return m_mesh.expired() ? false : true; }
		//========================================================

	private:
		bool CreateBuffers();
		static void CreateCube(std::vector<VertexPosTexTBN>& vertices, std::vector<unsigned int>& indices);
		static void CreateQuad(std::vector<VertexPosTexTBN>& vertices, std::vector<unsigned int>& indices);
		std::string GetGameObjectName();

		std::shared_ptr<D3D11VertexBuffer> m_vertexBuffer;
		std::shared_ptr<D3D11IndexBuffer> m_indexBuffer;
		std::weak_ptr<Mesh> m_mesh;
		MeshType m_meshType;
		Math::BoundingBox m_boundingBox;
	};
}