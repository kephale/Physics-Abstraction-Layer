#include "palCollision.h"
/*
	Abstract:
		PAL - Physics Abstraction Layer.
		Implementation File (collision)

	Author:
		Adrian Boeing
	Revision History:
		Version 0.1   : 05/07/08 - Original
	TODO:
*/

palCollisionDetection::palCollisionDetection(){
}

palContactPoint::palContactPoint()
: m_pBody1(NULL)
, m_pBody2(NULL)
, m_fDistance(0.0f) //!< The distance between closest points. Negative distance indicates interpenetrations
, m_fImpulse(0.0f) //!< The impulse magnitude used to resolve the constraints on the bodies along the normal.
{
}

palContact::palContact() {
}

palRayHit::palRayHit() {
	Clear();
}

void palRayHit::Clear() {
	m_bHit = false;
	m_bHitPosition = false;
	m_bHitNormal = false;
	m_pBody = 0;
	m_pGeom = 0;
	m_fDistance = -1;
}

void palRayHit::SetHitPosition(Float x, Float y, Float z) {
	m_bHitPosition = true;
	m_vHitPosition.x=x;
	m_vHitPosition.y=y;
	m_vHitPosition.z=z;
}
void palRayHit::SetHitNormal(Float x, Float y, Float z) {
	m_bHitNormal = true;
	m_vHitNormal.x = x;
	m_vHitNormal.y = y;
	m_vHitNormal.z = z;
}

palRayHitCallback::palRayHitCallback() {
}

palCollisionDetectionExtended::palCollisionDetectionExtended() {

}
