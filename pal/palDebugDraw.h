//(c) Adrian Boeing 2009, see liscence.txt (BSD liscence)
/** \file palDebugDraw.h
	\brief
		PAL - Physics Abstraction Layer.
		Debug Drawing utility class.
	\author
		David
	\version
	<pre>
	Revision History:
		Version 0.0.1: 21/12/09 - Initial Version
*/

#ifndef PALDEBUGDRAW_H
#define PALDEBUGDRAW_H

#include <framework/common.h>
#include <pal/palMath.h>

enum DebugDrawPrimitiveType {
	POINTS,
	LINES,
	TRIANGLES,
};

struct palDebugGeometry {
	DebugDrawPrimitiveType m_eType;
	PAL_VECTOR<palVector4> m_vColors;
	PAL_VECTOR<palVector3> m_vVertices;
	PAL_VECTOR<int> m_vIndices;
};

struct palDebugText {
	palVector3 m_vPos;
	PAL_STRING text;
};

class palDebugDraw {
public:
	palDebugDraw() {}
	virtual ~palDebugDraw() {}

	palDebugGeometry m_Lines;
	palDebugGeometry m_Points;
	palDebugGeometry m_Triangles;
	PAL_VECTOR<palDebugText> m_vTextItems;
};

#endif /* PALDEBUGDRAW_H */
