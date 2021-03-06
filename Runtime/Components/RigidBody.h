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

//= INCLUDES ===============
#include "Component.h"
#include <memory>
#include "../Math/Vector3.h"
//==========================

class btRigidBody;
class btCollisionShape;

namespace Directus
{
	class GameObject;
	namespace Math
	{
		class Quaternion;
	}

	enum ForceMode
	{
		Force,
		Impulse
	};

	class DLL_API RigidBody : public Component
	{
	public:
		RigidBody();
		~RigidBody();

		//= ICOMPONENT ==========================================
		virtual void Reset();
		virtual void Start();
		virtual void OnDisable();
		virtual void Remove();
		virtual void Update();
		virtual void Serialize();
		virtual void Deserialize();
		//=======================================================

		//= MASS ================================================
		float GetMass() { return m_mass; }
		void SetMass(float mass);

		//= DRAG ===============================================
		float GetDrag() { return m_drag; }
		void SetDrag(float drag);

		//= ANGULAR DRAG =======================================
		float GetAngularDrag() { return m_angularDrag; }
		void SetAngularDrag(float angularDrag);

		//= RESTITUTION ========================================
		float GetRestitution() { return m_restitution; }
		void SetRestitution(float restitution);

		//= GRAVITY ============================================
		void SetUseGravity(bool gravity);
		bool GetUseGravity() { return m_useGravity; };
		Math::Vector3 GetGravity() { return m_gravity; }
		void SetGravity(const Math::Vector3& acceleration);
		//======================================================

		//= KINEMATIC ==========================================
		void SetKinematic(bool kinematic);
		bool GetKinematic() { return m_isKinematic; }

		//= VELOCITY/FORCE/TORQUE =======================================
		void SetLinearVelocity(const Math::Vector3& velocity) const;
		void SetAngularVelocity(const Math::Vector3& velocity);
		void ApplyForce(const Math::Vector3& force, ForceMode mode) const;
		void ApplyForceAtPosition(const Math::Vector3& force, Math::Vector3 position, ForceMode mode) const;
		void ApplyTorque(const Math::Vector3& torque, ForceMode mode) const;

		//= POSITION LOCK =================================
		void SetPositionLock(bool lock);
		void SetPositionLock(const Math::Vector3& lock);
		Math::Vector3 GetPositionLock() { return m_positionLock; }
		//=================================================

		//= ROTATION LOCK =================================
		void SetRotationLock(bool lock);
		void SetRotationLock(const Math::Vector3& lock);
		Math::Vector3 GetRotationLock() { return m_rotationLock; }
		//=================================================

		//= POSITION ============================================
		Math::Vector3 GetPosition() const;
		void SetPosition(const Math::Vector3& position);

		//= ROTATION ============================================
		Math::Quaternion GetRotation() const;
		void SetRotation(const Math::Quaternion& rotation);

		//= MISC ================================================
		void SetCollisionShape(std::weak_ptr<btCollisionShape> shape);
		std::weak_ptr<btRigidBody> GetBtRigidBody() { return m_rigidBody; }
		void ClearForces() const;
		Math::Vector3 GetColliderCenter() const;

	private:
		//= HELPER FUNCTIONS ========================================
		void AddBodyToWorld();
		void RemoveBodyFromWorld();
		void UpdateGravity() const;
		void ReleaseBtRigidBody();
		//===========================================================
		bool IsActivated() const;
		void Activate() const;
		void Deactivate() const;

		std::shared_ptr<btRigidBody> m_rigidBody;
		std::weak_ptr<btCollisionShape> m_shape;

		float m_mass;
		float m_drag;
		float m_angularDrag;
		float m_restitution;
		bool m_useGravity;
		bool m_isKinematic;
	public:
		bool m_hasSimulated;
	private:
		Math::Vector3 m_gravity;
		Math::Vector3 m_positionLock;
		Math::Vector3 m_rotationLock;

		bool m_inWorld;
	};
}