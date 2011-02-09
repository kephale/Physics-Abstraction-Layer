//(c) Alion Science and Technology Inc. 2009, see liscence.txt (BSD liscence)
/** \file palCharacter.h
\brief
PAL - Physics Abstraction Layer.
Character motion model
\author
David Guthrie
\version
<pre>
Version 0.1	: 10/12/09 - Original
</pre>
\todo
*/
#include "bullet_palCharacter.h"
#include "BulletCollision/CollisionDispatch/btGhostObject.h"

palBulletCharacterController::palBulletCharacterController()
: m_pKinematicCharacterController(NULL)
, m_fSkinWidth(0.025f)
{
}

palBulletCharacterController::~palBulletCharacterController() {
	palBulletPhysics* physics = palBulletPhysics::GetInstance();

	if (m_pKinematicCharacterController != NULL)
	{
		btPairCachingGhostObject* pairCachingGhost = m_pKinematicCharacterController->getGhostObject();

		if (pairCachingGhost != NULL)
		{
			if (physics != NULL)
			{
				btDynamicsWorld* world = physics->BulletGetDynamicsWorld();
				world->removeCollisionObject(pairCachingGhost);
				world->removeCharacter(m_pKinematicCharacterController);
			}

			delete pairCachingGhost->getBroadphaseHandle();
			delete pairCachingGhost;
		}

		delete m_pKinematicCharacterController;
		m_pKinematicCharacterController = NULL;
	}
}

bool palBulletCharacterController::Init(const palCharacterControllerDesc& desc) {
	palBulletGeometry* geom = dynamic_cast<palBulletGeometry*>(desc.m_pShape);
	m_pShape = geom;
	bool validData = false;
	if (geom != NULL)
	{
		m_Geometries.push_back(geom);
		SetGeometryBody(geom);

		btCollisionShape* shape = geom->BulletGetCollisionShape();
		if (shape != NULL && shape->isConvex())
		{
			palBulletPhysics* physics = palBulletPhysics::GetInstance();
			btPairCachingGhostObject* pairCachingGhost = new btPairCachingGhostObject;

			// Set the skin width
			pairCachingGhost->setContactProcessingThreshold(btScalar(m_fSkinWidth));

			btDynamicsWorld* world = physics->BulletGetDynamicsWorld();
			pairCachingGhost->setCollisionShape(shape);
			pairCachingGhost->setCollisionFlags(btCollisionObject::CF_CHARACTER_OBJECT );
			world->addCollisionObject(pairCachingGhost, convert_group(desc.m_Group), physics->m_CollisionMasks[desc.m_Group]);

			btConvexShape* convexShape = static_cast<btConvexShape*>(shape);

			unsigned int upAxis = physics->GetUpAxis();

			m_pKinematicCharacterController = new btKinematicCharacterController(
						pairCachingGhost, convexShape, desc.m_fStepHeight, upAxis);

			m_pKinematicCharacterController->setUseGhostSweepTest(true);
			m_pKinematicCharacterController->setJumpSpeed(desc.m_fJumpSpeed);
			m_pKinematicCharacterController->setFallSpeed(desc.m_fFallSpeed);
			m_pKinematicCharacterController->setGravity(desc.m_fGravity);
			m_pKinematicCharacterController->setMaxSlope(btRadians(desc.m_fMaxInclineAngle));
			world->addCharacter(m_pKinematicCharacterController);
			validData = true;
		}
	}
	return validData;
}

void palBulletCharacterController::SetGroup(palGroup group) {
	palBodyBase::SetGroup(group);
	if (m_pKinematicCharacterController != NULL)
	{
		palBulletPhysics* physics = palBulletPhysics::GetInstance();
		btBroadphaseProxy *proxy = m_pKinematicCharacterController->getGhostObject()->getBroadphaseHandle();
		proxy->m_collisionFilterGroup = convert_group(group);
		if (proxy != NULL) {
			proxy->m_collisionFilterMask = physics->m_CollisionMasks[group];
			physics->BulletGetDynamicsWorld()->getBroadphase()->getOverlappingPairCache()->cleanProxyFromPairs(proxy,
						physics->BulletGetDynamicsWorld()->getDispatcher());
			// Reset the collision mask
		}
	}
}

palGroup palBulletCharacterController::GetGroup() const {
	palGroup group = palBodyBase::GetGroup();
	if (m_pKinematicCharacterController != NULL)
	{
		group = convert_to_pal_group(m_pKinematicCharacterController->getGhostObject()->getBroadphaseHandle()->m_collisionFilterGroup);
	}
	return group;
}

void palBulletCharacterController::Move(const palVector3& displacement) {
	if (m_pKinematicCharacterController != NULL) {
		m_pKinematicCharacterController->setWalkDirection(
					btVector3(displacement.x, displacement.y, displacement.z));
	}
}

void palBulletCharacterController::Walk(const palVector3& walkVelocity, Float timeInterval) {
	if (m_pKinematicCharacterController != NULL) {
		m_pKinematicCharacterController->setVelocityForTimeInterval(
					btVector3(walkVelocity.x, walkVelocity.y, walkVelocity.z), timeInterval);
	}
}

void palBulletCharacterController::WalkClear() {
	if (m_pKinematicCharacterController != NULL) {
		m_pKinematicCharacterController->reset();
	}
}

void palBulletCharacterController::Jump() {
	if (m_pKinematicCharacterController != NULL) {
		m_pKinematicCharacterController->jump();
	}
}

void palBulletCharacterController::Warp(const palVector3& worldPos) {
	if (m_pKinematicCharacterController != NULL) {
		m_pKinematicCharacterController->warp(btVector3(worldPos.x, worldPos.y, worldPos.z));
		SetPosition(worldPos.x, worldPos.y, worldPos.z);
	}
}

const palMatrix4x4& palBulletCharacterController::GetLocationMatrix() const {
	if (m_pKinematicCharacterController != NULL) {
		convertBtTransformToPalMat(m_mLoc, m_pKinematicCharacterController->getGhostObject()->getWorldTransform());
	}
	return m_mLoc;
}

Float palBulletCharacterController::GetSkinWidth() const {
	if (m_pKinematicCharacterController != NULL) {
		return (Float) m_pKinematicCharacterController->getGhostObject()->getContactProcessingThreshold();
	}
	return m_fSkinWidth;
}

bool palBulletCharacterController::SetSkinWidth(Float skinWidth) {
	m_fSkinWidth = skinWidth;
	if (m_pKinematicCharacterController != NULL) {
		m_pKinematicCharacterController->getGhostObject()->setContactProcessingThreshold(btScalar(skinWidth));
	}
	return true;
}

void palBulletCharacterController::SetMaxSlope(Float slopeDegrees) {
	if (m_pKinematicCharacterController != NULL) {
		m_pKinematicCharacterController->setMaxSlope(btScalar(slopeDegrees));
	}
}

Float palBulletCharacterController::GetMaxSlope() const {
	return m_pKinematicCharacterController != NULL
		? Float(m_pKinematicCharacterController->getMaxSlope())
		: 0.0f;
}

void palBulletCharacterController::SetGravity(Float gravity) {
	if (m_pKinematicCharacterController != NULL) {
		m_pKinematicCharacterController->setGravity(btScalar(gravity));
	}
}

Float palBulletCharacterController::GetGravity() const {
	return m_pKinematicCharacterController != NULL
		? Float(m_pKinematicCharacterController->getGravity())
		: 0.0f;
}

