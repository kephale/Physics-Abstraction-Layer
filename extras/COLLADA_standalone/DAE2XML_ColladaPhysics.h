#ifndef DAE2XML_COLLADA_PHYSICS_H

#define DAE2XML_COLLADA_PHYSICS_H

#include "pal/palFactory.h"

namespace DAE2XML
{

class ColladaPhysics;

ColladaPhysics*  loadColladaPhysics(const char *collada_name);
palBodyBase*	 palGetColladaBody(ColladaPhysics *cp, const char *model_name, const char *rigid_body_name);
palLink*		 palGetColladaLink(ColladaPhysics *cp, const char *model_name, const char *constraint_name);
bool             saveNxuStream(ColladaPhysics *cp,const char *nxustream_name);
bool             loadPAL(ColladaPhysics *cp);
void             releaseColladaPhysics(ColladaPhysics *cp);
const std::vector<palBodyBase *>&	palGetAllColladaBodies();
};


#endif
