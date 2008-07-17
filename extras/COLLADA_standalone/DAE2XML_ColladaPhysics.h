#ifndef DAE2XML_COLLADA_PHYSICS_H

#define DAE2XML_COLLADA_PHYSICS_H


namespace DAE2XML
{

class ColladaPhysics;

ColladaPhysics * loadColladaPhysics(const char *collada_name);
bool             saveNxuStream(ColladaPhysics *cp,const char *nxustream_name);
bool             loadPAL(ColladaPhysics *cp);
void             releaseColladaPhysics(ColladaPhysics *cp);

};


#endif
