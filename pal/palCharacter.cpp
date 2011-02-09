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

#include "palCharacter.h"

palCharacterControllerDesc::palCharacterControllerDesc()
: m_pShape(NULL)
, m_fJumpHeight(0.5)
, m_fJumpSpeed(15.0)
, m_fFallSpeed(20.0)
, m_fStepHeight(0.2)
, m_fMaxInclineAngle(45.0)
, m_fGravity(9.8)
{

}

palCharacterController::palCharacterController() {
}

palCharacterController::~palCharacterController() {
}
