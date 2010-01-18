#ifndef GLOBAL_STRINGS_H
#define GLOBAL_STRINGS_H

/*
 *  strings.h
 *  PALBenchmark
 *
 *  Created by Brock Woolf on 15/01/10.
 *  Copyright 2010 Blastcube. All rights reserved.
 *
 */

//-------------------------------------------------------------------------------------
//	Strings
//-------------------------------------------------------------------------------------
#ifdef __APPLE__
std::string cubeMeshFile = "/media/cube.3ds";
std::string splashScreen1 = "/media/Benchmark DemoBETA.png";
std::string dropImage = "/media/drop.png";
std::string integratorErrorImage = "/media/integratorerror.png";
std::string boxStackImage = "/media/boxstack.png";
std::string stackEffortImage = "/media/stackeffort.png";
std::string sphereStackImage = "/media/spherestack.png";
std::string pyramidStackImage = "/media/pyramidstack.png";
std::string constraintImage = "/media/constraint.png";
std::string constraintErrorImage = "/media/constrainterror.png";
std::string constraintTimeImage = "/media/constrainttime.png";
std::string restitutionImage = "/media/restitution.png";
std::string staticFrictionImage = "/media/staticfriction.png";
std::string collision100hzImage = "/media/collision100hz.png";
std::string collision10hzImage = "/media/collision10hz.png";
std::string palUrlImage = "/media/palurl.png";
std::string IOUpImage = "/media/io_up.jpg";
std::string IODownImage = "/media/io_dn.jpg";
std::string IOLeftImage = "/media/io_lf.jpg";
std::string IORightImage = "/media/io_rt.jpg";
std::string IOFooterImage = "/media/io_ft.jpg";
std::string IOBackImage = "/media/io_bk.jpg";
std::string sphereMeshFile = "/media/sphere.3ds";
std::string capsuleMeshFile = "/media/capsule.3ds";
std::string idealImageFile = "/media/ideal.3ds";
std::string spotImageTexture = "/media/spot.tga";
#else
std::string cubeMeshFile = "../media/cube.3ds";
std::string splashScreen1 = "../media/Benchmark DemoBETA.png";
std::string dropImage = "../media/drop.png";
std::string integratorErrorImage = "../media/integratorerror.png";
std::string boxStackImage = "../media/boxstack.png";
std::string stackEffortImage = "../media/stackeffort.png";
std::string sphereStackImage = "../media/spherestack.png";
std::string pyramidStackImage = "../media/pyramidstack.png";
std::string constraintImage = "../media/constraint.png";
std::string constraintErrorImage = "../media/constrainterror.png";
std::string constraintTimeImage = "../media/constrainttime.png";
std::string restitutionImage = "../media/restitution.png";
std::string staticFrictionImage = "../media/staticfriction.png";
std::string collision100hzImage = "../media/collision100hz.png";
std::string collision10hzImage = "../media/collision10hz.png";
std::string palUrlImage = "../media/palurl.png";
std::string IOUpImage = "../media/io_up.jpg";
std::string IODownImage = "../media/io_dn.jpg";
std::string IOLeftImage = "../media/io_lf.jpg";
std::string IORightImage = "../media/io_rt.jpg";
std::string IOFooterImage = "../media/io_ft.jpg";
std::string IOBackImage = "../media/io_bk.jpg";
std::string sphereMeshFile = "/media/sphere.3ds";
std::string capsuleMeshFile = "../media/capsule.3ds";
std::string idealImageFile = "../media/ideal.3ds";
std::string spotImageTexture = "../media/spot.tga";
#endif
//-------------------------------------------------------------------------------------
#endif