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

//= INCLUDES ================================================
#include "RigidBody.h"
#include "BulletDynamics/Dynamics/btRigidBody.h"
#include <BulletCollision/CollisionShapes/btCollisionShape.h>
#include <BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h>
#include <LinearMath/btMotionState.h>
#include "Transform.h"
#include "Collider.h"
#include "../Core/Engine.h"
#include "../Core/GameObject.h"
#include "../Physics/Physics.h"
#include "../Physics/BulletPhysicsHelper.h"
#include "../Math/Quaternion.h"
#include "../IO/StreamIO.h"
//===========================================================

//= NAMESPACES ================
using namespace Directus::Math;
using namespace std;
//=============================

namespace Directus
{
	static const float DEFAULT_MASS = 0.0f;
	static const float DEFAULT_RESTITUTION = 0.0f;
	static const float DEFAULT_FRICTION = 0.5f;
	static const float DEFAULT_ANGULAR_FRICTION = 0.0f;

	class MotionState : public btMotionState
	{
		RigidBody* m_rigidBody;
	public:
		MotionState(RigidBody* rigidBody) { m_rigidBody = rigidBody; }

		// Update from engine, ENGINE -> BULLET
		void btMotionState::getWorldTransform(btTransform& worldTransform) const override
		{
			Vector3 enginePosition = m_rigidBody->g_transform->GetPosition();
			Quaternion engineRotation = m_rigidBody->g_transform->GetRotation();
			worldTransform.setOrigin(ToBtVector3(enginePosition + engineRotation * m_rigidBody->GetColliderCenter()));
			worldTransform.setRotation(ToBtQuaternion(engineRotation));

			m_rigidBody->m_hasSimulated = true;
		}

		// Update from bullet, BULLET -> ENGINE
		void btMotionState::setWorldTransform(const btTransform& worldTransform) override
		{
			Quaternion bulletRot = ToQuaternion(worldTransform.getRotation());
			Vector3 bulletPos = ToVector3(worldTransform.getOrigin()) - bulletRot * m_rigidBody->GetColliderCenter();
			m_rigidBody->g_transform->SetPosition(bulletPos);
			m_rigidBody->g_transform->SetRotation(bulletRot);

			m_rigidBody->m_hasSimulated = true;
		}
	};

	RigidBody::RigidBody()
	{
		m_inWorld = false;
		m_mass = DEFAULT_MASS;
		m_restitution = DEFAULT_RESTITUTION;
		m_drag = DEFAULT_FRICTION;
		m_angularDrag = DEFAULT_ANGULAR_FRICTION;
		m_useGravity = true;
		m_isKinematic = false;
		m_hasSimulated = false;
		m_positionLock = Vector3::Zero;
		m_rotationLock = Vector3::Zero;
	}

	RigidBody::~RigidBody()
	{
		ReleaseBtRigidBody();
	}

	//= ICOMPONENT ==========================================================
	void RigidBody::Reset()
	{
		AddBodyToWorld();
	}

	void RigidBody::Start()
	{

	}

	void RigidBody::OnDisable()
	{

	}

	void RigidBody::Remove()
	{

	}

	void RigidBody::Update()
	{
		// To make the body able to get positioned directly by the use without worrying about Bullet 
		// reseting it's state, we secretly set is as kinematic when the engine is not simulating (e.g. Editor Mode)
		if (!m_rigidBody)
			return;

		EngineMode engineMode = g_context->GetSubsystem<Engine>()->GetMode();

		// Editor -> Kinematic (so the user can move it around)
		if (engineMode == Editor && !m_rigidBody->isKinematicObject())
		{
			AddBodyToWorld();
		}

		// Game -> Dynamic (so bullet starts simulating)
		if (engineMode == Game && !m_isKinematic && m_rigidBody->isKinematicObject())
		{
			AddBodyToWorld();
		}
	}

	void RigidBody::Serialize()
	{
		StreamIO::WriteFloat(m_mass);
		StreamIO::WriteFloat(m_drag);
		StreamIO::WriteFloat(m_angularDrag);
		StreamIO::WriteFloat(m_restitution);
		StreamIO::WriteBool(m_useGravity);
		StreamIO::WriteVector3(m_gravity);
		StreamIO::WriteBool(m_isKinematic);
		StreamIO::WriteVector3(m_positionLock);
		StreamIO::WriteVector3(m_rotationLock);
	}

	void RigidBody::Deserialize()
	{
		m_mass = StreamIO::ReadFloat();
		m_drag = StreamIO::ReadFloat();
		m_angularDrag = StreamIO::ReadFloat();
		m_restitution = StreamIO::ReadFloat();
		m_useGravity = StreamIO::ReadBool();
		m_gravity = StreamIO::ReadVector3();
		m_isKinematic = StreamIO::ReadBool();
		m_positionLock = StreamIO::ReadVector3();
		m_rotationLock = StreamIO::ReadVector3();

		AddBodyToWorld();
	}

	// = PROPERTIES =========================================================
	void RigidBody::SetMass(float mass)
	{
		m_mass = max(mass, 0.0f);
		AddBodyToWorld();
	}

	void RigidBody::SetDrag(float drag)
	{
		m_drag = drag;
		AddBodyToWorld();
	}

	void RigidBody::SetAngularDrag(float angularDrag)
	{
		m_angularDrag = angularDrag;
		AddBodyToWorld();
	}

	void RigidBody::SetRestitution(float restitution)
	{
		m_restitution = restitution;
		AddBodyToWorld();
	}

	void RigidBody::SetUseGravity(bool gravity)
	{
		if (gravity == m_useGravity)
			return;

		m_useGravity = gravity;
		AddBodyToWorld();
	}

	void RigidBody::SetGravity(const Vector3& acceleration)
	{
		m_gravity = acceleration;
		AddBodyToWorld();
	}

	void RigidBody::SetKinematic(bool kinematic)
	{
		if (kinematic == m_isKinematic)
			return;

		m_isKinematic = kinematic;
		AddBodyToWorld();
	}

	//= FORCE/TORQUE ========================================================
	void RigidBody::SetLinearVelocity(const Vector3& velocity) const
	{
		if (!m_rigidBody)
			return;

		m_rigidBody->setLinearVelocity(ToBtVector3(velocity));
		if (velocity != Vector3::Zero)
		{
			Activate();
		}
	}

	void RigidBody::SetAngularVelocity(const Vector3& velocity)
	{
		if (!m_rigidBody)
			return;

		m_rigidBody->setAngularVelocity(ToBtVector3(velocity));
		if (velocity != Vector3::Zero)
		{
			Activate();
		}
	}

	void RigidBody::ApplyForce(const Vector3& force, ForceMode mode) const
	{
		Activate();

		if (mode == Force)
		{
			m_rigidBody->applyCentralForce(ToBtVector3(force));
		}
		else if (mode == Impulse)
		{
			m_rigidBody->applyCentralImpulse(ToBtVector3(force));
		}
	}

	void RigidBody::ApplyForceAtPosition(const Vector3& force, Vector3 position, ForceMode mode) const
	{
		Activate();

		if (mode == Force)
		{
			m_rigidBody->applyForce(ToBtVector3(force), ToBtVector3(position));
		}
		else if (mode == Impulse)
		{
			m_rigidBody->applyImpulse(ToBtVector3(force), ToBtVector3(position));
		}
	}

	void RigidBody::ApplyTorque(const Vector3& torque, ForceMode mode) const
	{
		Activate();

		if (mode == Force)
		{
			m_rigidBody->applyTorque(ToBtVector3(torque));
		}
		else if (mode == Impulse)
		{
			m_rigidBody->applyTorqueImpulse(ToBtVector3(torque));
		}
	}

	//= CONSTRAINTS =========================================================
	void RigidBody::SetPositionLock(bool lock)
	{
		if (lock)
		{
			SetPositionLock(Vector3::One);
		}
		else
		{
			SetPositionLock(Vector3::Zero);
		}
	}

	void RigidBody::SetPositionLock(const Vector3& lock)
	{
		m_positionLock = lock;

		Vector3 translationFreedom = Vector3(!lock.x, !lock.y, !lock.z);
		m_rigidBody->setLinearFactor(ToBtVector3(translationFreedom));
	}

	void RigidBody::SetRotationLock(bool lock)
	{
		if (lock)
			SetRotationLock(Vector3::One);
		else
			SetRotationLock(Vector3::Zero);
	}

	void RigidBody::SetRotationLock(const Vector3& lock)
	{
		m_rotationLock = lock;

		Vector3 rotationFreedom = Vector3(!lock.x, !lock.y, !lock.z);
		m_rigidBody->setAngularFactor(ToBtVector3(rotationFreedom));
	}

	//= POSITION ============================================================
	Vector3 RigidBody::GetPosition() const
	{
		return m_rigidBody ? ToVector3(m_rigidBody->getWorldTransform().getOrigin()) : Vector3::Zero;
	}

	void RigidBody::SetPosition(const Vector3& position)
	{
		if (!m_rigidBody)
			return;

		m_rigidBody->getWorldTransform().setOrigin(ToBtVector3(position + ToQuaternion(m_rigidBody->getWorldTransform().getRotation()) * GetColliderCenter()));
	}

	//= ROTATION ============================================================
	Quaternion RigidBody::GetRotation() const
	{
		return m_rigidBody ? ToQuaternion(m_rigidBody->getWorldTransform().getRotation()) : Quaternion::Identity;
	}

	void RigidBody::SetRotation(const Quaternion& rotation)
	{
		if (!m_rigidBody)
			return;

		m_rigidBody->getWorldTransform().setRotation(ToBtQuaternion(rotation));
	}

	//= MISC ====================================================================
	void RigidBody::SetCollisionShape(weak_ptr<btCollisionShape> shape)
	{
		m_shape = shape;
		AddBodyToWorld();
	}

	void RigidBody::ClearForces() const
	{
		if (!m_rigidBody)
			return;

		m_rigidBody->clearForces();
	}

	Vector3 RigidBody::GetColliderCenter() const
	{
		if (g_gameObject.expired())
		{
			return Vector3::Zero;
		}

		Collider* collider = g_gameObject.lock()->GetComponent<Collider>();
		return collider ? collider->GetCenter() : Vector3::Zero;
	}
	//===========================================================================

	//= HELPER FUNCTIONS ========================================================
	void RigidBody::AddBodyToWorld()
	{
		btVector3 inertia(0, 0, 0);

		if (m_mass < 0.0f)
		{
			m_mass = 0.0f;
		}

		// in case there is an existing rigidBody, remove it
		if (m_rigidBody)
		{
			inertia = m_rigidBody->getLocalInertia(); // save the inertia
			ReleaseBtRigidBody();
		}

		// Calculate local inertia
		if (!m_shape.expired())
			m_shape._Get()->calculateLocalInertia(m_mass, inertia);

		// Motion state
		MotionState* motionState = new MotionState(this);

		// Construction Info
		btRigidBody::btRigidBodyConstructionInfo constructionInfo(m_mass, motionState, !m_shape.expired() ? m_shape._Get() : nullptr, inertia);
		constructionInfo.m_mass = m_mass;
		constructionInfo.m_friction = m_drag;
		constructionInfo.m_rollingFriction = m_angularDrag;
		constructionInfo.m_restitution = m_restitution;
		constructionInfo.m_startWorldTransform;
		constructionInfo.m_collisionShape = !m_shape.expired() ? m_shape._Get() : nullptr;
		constructionInfo.m_localInertia = inertia;
		constructionInfo.m_motionState = motionState;

		// RigidBody
		m_rigidBody = make_shared<btRigidBody>(constructionInfo);

		UpdateGravity();

		//= COLLISION FLAGS ====================================================================
		int flags = m_rigidBody->getCollisionFlags();

		// Editor -> Kinematic (so the user can move it around)
		bool originalKinematicState = m_isKinematic;
		if (g_context->GetSubsystem<Engine>()->GetMode() == Editor)
			m_isKinematic = true;

		if (m_isKinematic)
		{
			flags |= btCollisionObject::CF_KINEMATIC_OBJECT;
		}
		else
		{
			flags &= ~btCollisionObject::CF_KINEMATIC_OBJECT;
		}

		m_rigidBody->setCollisionFlags(flags);
		m_rigidBody->forceActivationState(m_isKinematic ? DISABLE_DEACTIVATION : ISLAND_SLEEPING);

		m_isKinematic = originalKinematicState;
		//======================================================================================
		m_rigidBody->setDeactivationTime(2000);

		SetPosition(g_transform->GetPosition());
		SetRotation(g_transform->GetRotation());

		// Constraints
		SetPositionLock(m_positionLock);
		SetRotationLock(m_rotationLock);

		// PHYSICS WORLD - ADD
		g_context->GetSubsystem<Physics>()->GetWorld()->addRigidBody(m_rigidBody.get());

		if (m_mass > 0.0f)
		{
			Activate();
		}
		else
		{
			SetLinearVelocity(Vector3::Zero);
			SetAngularVelocity(Vector3::Zero);
		}

		m_hasSimulated = false;
		m_inWorld = true;
	}

	void RigidBody::RemoveBodyFromWorld()
	{
		if (!m_rigidBody)
			return;

		if (m_inWorld)
		{
			g_context->GetSubsystem<Physics>()->GetWorld()->removeRigidBody(m_rigidBody.get());
			m_inWorld = false;
		}
	}

	void RigidBody::UpdateGravity() const
	{
		if (!m_rigidBody)
			return;

		btDiscreteDynamicsWorld* world = g_context->GetSubsystem<Physics>()->GetWorld();

		int flags = m_rigidBody->getFlags();
		if (m_useGravity)
		{
			flags &= ~BT_DISABLE_WORLD_GRAVITY;
		}
		else
		{
			flags |= BT_DISABLE_WORLD_GRAVITY;
		}
		m_rigidBody->setFlags(flags);

		if (m_useGravity)
		{
			m_rigidBody->setGravity(world->getGravity());
		}
		else
		{
			m_rigidBody->setGravity(btVector3(0.0f, 0.0f, 0.0f));
		}
	}

	void RigidBody::ReleaseBtRigidBody()
	{
		if (!m_rigidBody)
			return;

		g_context->GetSubsystem<Physics>()->GetWorld()->removeRigidBody(m_rigidBody.get());
		delete m_rigidBody->getMotionState();
	}

	bool RigidBody::IsActivated() const
	{
		return m_rigidBody->isActive();
	}

	void RigidBody::Activate() const
	{
		if (!m_rigidBody)
			return;

		if (m_mass > 0.0f)
		{
			m_rigidBody->activate(true);
		}
	}

	void RigidBody::Deactivate() const
	{
		if (!m_rigidBody)
			return;

		m_rigidBody->setActivationState(WANTS_DEACTIVATION);
	}
	//===========================================================================
}