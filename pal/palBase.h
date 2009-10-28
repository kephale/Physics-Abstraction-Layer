#ifndef PALBASE_H
#define PALBASE_H
//(c) Adrian Boeing 2007, see liscence.txt (BSD liscence)
/*! \file palBase.h
	\brief
		PAL - Physics Abstraction Layer.
		Base functionality
	\author
		Adrian Boeing
	\version
	<pre>
		Version 0.1   : 11/12/07 - Original
	</pre>
	\todo
*/

#include "../framework/errorlog.h"

#include "../framework/factoryconfig.h"
//Define the base class from which all PAL objects inherit
typedef myFactoryObject palFactoryObject;

class palFactory;
/*
class palFactoryObject : public palFactoryObjectBase {
public:
	palFactoryObject() {
		m_pParent = NULL;
	}
	void SetParent(palFactoryObject *parent) {
		m_pParent = parent;
	}
	palFactoryObject
protected:
	palFactoryObject *m_pParent;
}
*/
#include "palMath.h"

typedef int palGroup;

///A mask used for requesting a list of groups
typedef unsigned long palGroupFlags;

#endif
