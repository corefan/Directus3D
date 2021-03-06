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

//= INCLUDES ================
#include "Component.h"
#include "../Graphics/Mesh.h"
#include <memory>
//===========================

class btCollisionShape;

namespace Directus
{
	class RigidBody;
	class MeshFilter;	

	class DLL_API MeshCollider : public Component
	{
	public:
		MeshCollider();
		~MeshCollider();

		//= ICOMPONENT ==============
		virtual void Reset();
		virtual void Start();
		virtual void OnDisable();
		virtual void Remove();
		virtual void Update();
		virtual void Serialize();
		virtual void Deserialize();
		//===========================

		bool GetConvex() { return m_isConvex; }
		void SetConvex(bool isConvex) { m_isConvex = isConvex; }
		std::weak_ptr<Mesh>& GetMesh() { return m_mesh; }
		void SetMesh(std::weak_ptr<Mesh> mesh) { m_mesh = mesh; }
		void Build();

	private:
		//= HELPER FUNCTIONS ======================================================
		void DeleteCollisionShape();
		void SetCollisionShapeToRigidBody(std::weak_ptr<btCollisionShape> shape);
		std::weak_ptr<Mesh> GetMeshFromAttachedMeshFilter();
		//========================================================================

		std::weak_ptr<Mesh> m_mesh;
		unsigned int m_vertexLimit = 100000;
		std::shared_ptr<btCollisionShape> m_collisionShape;
		bool m_isConvex;
	};
}