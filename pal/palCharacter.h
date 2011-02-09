#ifndef PALCHARACTER_H_
#define PALCHARACTER_H_

#include "palFactory.h"
#include "palMath.h"
#include "palBase.h"
#include "palGeometry.h"

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

struct palCharacterControllerDesc {
	palCharacterControllerDesc();

	/** The geometry description to use when creating this controller.
	 * It also gets the initial position from this geometry.
	 */
	palGeometry *m_pShape;

	/// The height the character can jump.
	Float m_fJumpHeight;

	/// The initial upward jumping speed.
	Float m_fJumpSpeed;

	/// The max falling speed (terminal velocity).
	Float m_fFallSpeed;

	/// The height the character can step.
	Float m_fStepHeight;

	/// The max steepness the character can walk up as an angle from 0 to 90 degrees.
	Float m_fMaxInclineAngle;

	/// The gravitational force that should be applied.
	Float m_fGravity;

	/// The initial collision group.
	palGroup m_Group;
};


class palCharacterController : public palBodyBase {
public:
	palCharacterController();
	virtual ~palCharacterController();

	/**
	 * Initializes this character controller with the given description object.
	 * The shape passed in must be non-null. This object will also take ownership of the shape geom memory,
	 * that is it will delete it.  Some engines only support some shapes, such as a capsule and a box,  so
	 * only pass other shapes if you know the engine being used supports them.  Note that not all
	 * parameters on the description may be used by all engines.
	 *
	 * @return true if initialization succeeded or false if an error occurred.
	 */
	virtual bool Init(const palCharacterControllerDesc& desc) = 0;

	/// Moves with a given displacement vector
	virtual void Move(const palVector3& displacement) = 0;

	/// Starts motion along the walkVelocity vector for the specified item interval.
	virtual void Walk(const palVector3& walkVelocity, Float timeInterval) = 0;

	/// Clears a walk call early
	virtual void WalkClear() = 0;

	/// Sets an upward velocity for causing a character to jump.
	//virtual void Jump() = 0;

	/// Forces the underlying body to warp to the given position.
	virtual void Warp(const palVector3& worldPos) = 0;

};

#endif /* PALCHARACTER_H_ */
