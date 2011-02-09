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

#ifndef BULLET_PALCHARACTER_H_
#define BULLET_PALCHARACTER_H_

#include "bullet_pal.h"
#include <pal/palCharacter.h>
#include "BulletDynamics/Character/btKinematicCharacterController.h"

class palBulletCharacterController : public palCharacterController {
public:
	palBulletCharacterController();
	virtual ~palBulletCharacterController();

	virtual const palMatrix4x4& GetLocationMatrix() const;

	/// Initializes this character controller with the given description object.
	virtual bool Init(const palCharacterControllerDesc& desc);

	/// Sets the collision group for the underlying body.
	virtual void SetGroup(palGroup group);

	/// @return the collision group for the underlying body.
	virtual palGroup GetGroup() const;

	/// Moves with a given displacement vector
	virtual void Move(const palVector3& displacement);

	/// Starts motion along the walkVelocity vector for the specified item interval.
	virtual void Walk(const palVector3& walkVelocity, Float timeInterval);

	/// Clears a walk call early
	virtual void WalkClear();

	/// Sets an upward velocity.
	virtual void Jump();

	/// Forces the underlying body to warp to the given position.
	virtual void Warp(const palVector3& worldPos);

	virtual Float GetSkinWidth() const;

	virtual bool SetSkinWidth(Float skinWidth);

	/// Sets the the maximum slope angle over which the character can traverse.
	virtual void SetMaxSlope(Float slopeDegrees);

	virtual Float GetMaxSlope() const;

	/// Sets the gravitational force that should be applied to the character.
	virtual void SetGravity(Float gravity);

	virtual Float GetGravity() const;

protected:
	btKinematicCharacterController* m_pKinematicCharacterController;
	palBulletGeometry* m_pShape;
	Float m_fSkinWidth;
	FACTORY_CLASS(palBulletCharacterController,palCharacterController,Bullet,1)
};

#endif /* BULLET_PALCHARACTER_H_ */
