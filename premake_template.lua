--[[
PAL Premake File

v1.4.0 - 13/01/08 - Box2D support
v1.3.1 - 12/01/08 - JigLib lib dir fix
v1.3.0 - 04/01/08 - Added make_library name flags
v1.2.1 - 04/01/08 - Improved linux support for static_example, ODE and Novodex (contributions by  Volker Darius)
v1.2.0 - 31/12/07 - OpenTissue support
v1.1.0 - 30/12/07 - SPE support
v1.0.0 - 28/12/07 - First public release

flags:
static_example - build the static linking example, or dynamic linking example

internal_debug - to enable factory debug info
use_qhull - to enable qhull use for the engines that require it (eg:tokamak)

make_* - to make a physics engine implementation
eg:
make_spe - to make simple physics engine
make_ibds - to make ibds
make_opentissue - to make opentissue
]]--
--====================================



rloc = "../../"
dirBoost = lloc .. "boost"

--Project name (MSVC solution) --
project.name = "pal"
--location where the project make files will be located--
project.path = "build/" .. target
--location where the executable files will be located--
project.bindir = "bin"
project.config["Debug"].bindir   = "bin/debug/" .. target .."/"
project.config["Release"].bindir = "bin/release/" .. target .."/"

project.libdir = "lib" 
project.config["Debug"].libdir   = "lib/debug/" .. target .."/"
project.config["Release"].libdir = "lib/release/" .. target .."/"

function configureSDLGL(package)
	if (OS == "windows") then
        	tinsert(package.links, { "opengl32", "glu32", "SDLmain", "SDL" })
	else
        	tinsert(package.links, { "GL", "GLU", "SDL" })
	end
end

function MergeTable(dest, x ) 
	if (type(x)) == table then
		for key,value in pairs(x) do
			table.insert(dest,value)
		end
	else
		table.insert(dest,x)
	end
end

function CopyDLL(directory, dllname)
	os.copyfile(directory .. dllname, project.config["Debug"].bindir .. dllname)
	os.copyfile(directory .. dllname, project.config["Release"].bindir .. dllname)
end


--packageinfo class
PackageInfo = {}
PackageInfo.__index = PackageInfo

function PackageInfo.create()
   local p = {}             -- our new object
   setmetatable(p,PackageInfo)  -- make Account handle lookup
   p.config = {}
   p.config["Debug"] = {}
   p.config["Release"] = {}
   return p
end

function PackageInfo:buildpackage()
	
	if (self.name == nil) then
		print ("ERROR: NO NAME!")
		return
	end
	if (self.includepaths == nil) then
		print ("ERROR:" .. self.name .. " does not contain a include path")
		return
	end
	if (self.libpaths == nil) then
		print ("ERROR:" .. self.name .. " does not contain a lib path")
		return
	end
	if (self.files == nil) then
		print ("ERROR:" .. self.name .. " does not contain files")
		return
	end
	
	
	package = newpackage()
	package.name = self.name
	
	package.path = "build/" .. target
	package.kind = "dll"
	package.language = "c++"
	package.includepaths = { 
		rloc .. "framework",
		rloc .. "pal",
		self.includepaths
	}
	
	--package.buildflags = { "dylib" } --for macosX(?)
	if (self.buildflags ~= nil) then
			tinsert(package.buildflags, self.buildflags)
	end
	package.config["Release"].buildflags = { "optimize" }
	
	package.links = { "libpal" } 
	if (self.links ~= nil) then
			tinsert(package.links,self.links)
	end
	
	package.libpaths = self.libpaths
	if (self.config["Debug"].libpaths ~= nil) then
		package.config["Debug"].libpaths = self.config["Debug"].libpaths
	end
	if (self.config["Release"].libpaths ~= nil) then
		package.config["Release"].libpaths = self.config["Release"].libpaths
	end
	package.files = self.files
	if (windows) then
		export_tag = string.upper(package.name .. "_exports")
		package.defines = { "WIN32","_WINDOWS","_USRDLL",export_tag,"DLL_GROUP_IMPLEMENTATION",self.defines}
	else
		package.defines = { "DLL_GROUP_IMPLEMENTATION",self.defines}
	end
	if (target == "vs2005") then
		tinsert(package.defines,{"_CRT_SECURE_NO_DEPRECATE"})
	end
	
	package.config["Release"].defines = {"NDEBUG"}
	if (internal_debug) then
		package.config["Debug"].defines = {"INTERNAL_DEBUG"}
	end
	
	if (linux) then
	if (target == "gnu") then
		tinsert(package.linkoptions,{"-Wl,-E"})
	end
	end
--[[	if (linux) then
	if (target == "gnu") then
		tinsert(package.linkoptions,{"-ldl"})
	end
	end]]--
end

--==============================================
--Package : libpal --
package = newpackage()
package.name = "libpal"
package.path = "build/" .. target
package.kind = "lib"
package.language = "c++"
package.includepaths = { rloc .. "framework" }
package.files = { 
	matchfiles(rloc .. "pal/*.h", rloc.."pal/*.cpp"),
	matchfiles(rloc .. "framework/*.h", rloc.."framework/*.cpp"),
	rloc .. "pal_i/hull.cpp", 
	rloc .. "pal_i/hull.h"
	}
package.excludes = {
  rloc .. "pal/palXmlFactory.cpp",
  rloc .. "pal/palXmlFactory.h"
--  rloc .. "framework/errorlog.cpp",
--  rloc .. "framework/errorlog.h"
}
if (windows) then
	package.defines = { "WIN32", "_LIB" }
end
package.config["Release"].defines = {"NDEBUG"}
if (internal_debug) then
		package.config["Debug"].defines = {"INTERNAL_DEBUG"}
end
if (target == "vs2005") then
	tinsert(package.defines,{"_CRT_SECURE_NO_DEPRECATE"})
end
package.config["Release"].buildflags = { "optimize" }
	
if (windows) then
	cmdcopy = "copy "
	if (target~="gnu") then
		libname = "libpal.lib"
	else
		libname = "liblibpal.a"
	end
else
	cmdcopy = "cp "
	libname = "liblibpal.a"
end


cpd = cmdcopy .. rloc .. "lib/debug/" .. target .. "/" .. libname .. " " .. rloc .. "lib/debug"
cpr = cmdcopy .. rloc .. "lib/release/" .. target .. "/" .. libname .. " " .. rloc .. "lib/release"
if (windows) then
	cpd = string.gsub(cpd,"/","\\")
	cpr = string.gsub(cpr,"/","\\")
else
	cpd = cpd .. "/."
	cpr = cpr .. "/."
end
package.config["Debug"].postbuildcommands    = {cpd}
package.config["Release"].postbuildcommands    = {cpr}

--==============================================
--Package : common test_lib --

	package = newpackage()
	package.name = "libtest"
	package.path = "build/" .. target
	package.kind = "lib"
	package.language = "c++"
	package.includepaths = { lloc .. "SDL/include" }
	package.files = { 
		matchfiles(rloc .. "sdlgl/*.h", rloc .. "sdlgl/*.cpp"),
		matchfiles(rloc .. "test_lib/*.h", rloc.."test_lib/*.cpp"),
		rloc .. "example/graphics.cpp",rloc .. "example/graphics.h"
		}
	if (windows) then
		package.defines = { "WIN32", "_LIB" }
	end
	package.config["Release"].defines = {"NDEBUG"}
	if (internal_debug) then
			package.config["Debug"].defines = {"INTERNAL_DEBUG"}
	end

--==============================================
--Package : pal demo --
if (target~="vs6") then --not for vs6
	if (windows) then
	package = newpackage()
	package.links = { "libpal","libtest","User32"}
	configureSDLGL(package)
	package.name = "paldemo"
	package.path = "build/" .. target
	package.kind = "winexe"
	package.language = "c++"
	package.includepaths = { lloc .. "SDL/include" }
	package.libpaths = {lloc .. "SDL/lib/"  }
	package.files = { 
		matchfiles(rloc .. "paldemo/*.h", rloc .. "paldemo/*.cpp"),
		rloc .. "paldemo/res.rc",
		}
	package.defines = {"WIN32";"_WINDOWS"}
	package.config["Release"].defines = {"NDEBUG"}
	if (internal_debug) then
		package.config["Debug"].defines = {"INTERNAL_DEBUG"}
		package.kind = "exe"
	end
	if (target =="gnu") then
		package.linkoptions = { "-lmingw32 -mwindows" }
	end
	end
end

pList = {} --contains all pal implementations
--==============================================

--Package : libpal_box2D --
if (make_box2d) then
	pBox2D = PackageInfo.create()
	pBox2D.name = "libpal_box2d"
	pBox2D.includepaths = {
		dirBox2D .. "Include"
	}
	pBox2D.files = { 
		matchfiles(rloc .. "pal_i/box2d*.h", rloc.."pal_i/box2d*.cpp"),
	}
	pBox2D.libpaths = {dirBox2D .. "Library"  }
end

--Package : libpal_bullet --
if (make_bullet) then
	pBullet = PackageInfo.create()
	pBullet.name = "libpal_bullet"
	pBullet.includepaths = {
		dirBullet .. "src"
	}
	pBullet.files = { 
		matchfiles(rloc .. "pal_i/bullet*.h", rloc.."pal_i/bullet*.cpp"),
	}
	pBullet.libpaths = {dirBullet .. "/lib/"  }
	if (windows) then	
	else
		pBullet.links = { "bulletmath", "bulletdynamics",  "bulletcollision" }
	end
end
--package.config["Debug"].libpaths = {lloc .. "bullet/out/debug8/libs"  }--
--package.config["Release"].libpaths = {lloc .. "bullet/out/release8/libs"  }--

--Package : libpal_ibds --
if (make_ibds) then
	pIBDS = PackageInfo.create()
	pIBDS.name = "libpal_ibds"
	pIBDS.includepaths = dirIBDS
	pIBDS.files = {
		matchfiles(rloc .. "pal_i/ibds*.h", rloc.."pal_i/ibds*.cpp"),
	}
	pIBDS.libpaths = {dirIBDS .. "lib" }
end

--Package : libpal_jiggle --
if (make_jiggle) then
	pJiggle = PackageInfo.create()
	pJiggle.name = "libpal_jiggle"
	pJiggle.includepaths = { 
		dirJiggle .. "include"
	 }
	pJiggle.libpaths = {dirJiggle .. "lib"  }
	pJiggle.files = { 
		matchfiles(rloc .. "pal_i/jiggle*.h", rloc.."pal_i/jiggle*.cpp")
	}
end

--Package : libpal_newton --
if (make_newton) then
	pNewton = PackageInfo.create()
	pNewton.name = "libpal_newton"
	pNewton.includepaths = dirNewton
	pNewton.files =  { 
		  matchfiles(rloc .. "pal_i/newton*.h", rloc.."pal_i/newton*.cpp")
		}
	if (windows) then
		pNewton.libpaths =  {dirNewton .. "dll"  }
	else
		pNewton.libpaths =  {dirNewton }
		pNewton.links = {"newton32"}
	end
end

--Package : libpal_novodex--
if (make_novodex) then
	pNovodex = PackageInfo.create()
	pNovodex.name = "libpal_novodex"
	pNovodex.includepaths = { 
		dirNovodex .. "SDKs/Physics/include",
		dirNovodex .. "SDKs/Foundation/include",
		dirNovodex .. "SDKs/PhysXLoader/include",
		dirNovodex .. "SDKs/Cooking/Include",			
		dirNovodex .. "SDKs/NxCharacter/include",		
		dirNovodex .. "SDKs/NxExtensions/include"
	 }
	pNovodex.files = { 
		  matchfiles(rloc .. "pal_i/novodex*.h", rloc.."pal_i/novodex*.cpp"),
		  rloc .. "pal_i/Stream.cpp", 
		  rloc .. "pal_i/Stream.h"
		}
	if (windows) then
		pNovodex.libpaths = {dirNovodex .. "SDKs/lib/win32"  }
	else
		pNovodex.libpaths = {dirNovodex .. "latest"  }
		
		pNovodex.links = {
			"pthread",	
			"PhysXCore", 
			"NxCharacter",
			"NxCooking",
			"PhysXLoader"
		}
		
		pNovodex.defines = 	{"CORELIB","NX32","NX_DISABLE_HARDWARE","NX_USE_FLUID_API"}
		if (linux) then
			tinsert(pNovodex.defines,"LINUX")
		end
	end
end

--Package : libpal_ode --
if (make_ode) then
	pODE = PackageInfo.create()
	pODE.name = "libpal_ode"

	pODE.files = { 
		  matchfiles(rloc .. "pal_i/ode*.h", rloc.."pal_i/ode*.cpp"),
		  rloc .. "pal_i/hull.cpp", 
		  rloc .. "pal_i/hull.h"
		}
	if (windows) then
		pODE.includepaths = dirODE .. "include"
		pODE.libpaths = {dirODE .. "lib"  }
		pODE.config["Debug"].libpaths = {dirODE ..  "lib/debugdll"  }
		pODE.config["Release"].libpaths = {dirODE ..  "lib/releasedll"  }
	else
		pODE.includepaths = dirODE
		pODE.libpaths = dirODE
		pODE.links = { "ode" }
	end
end

--Package : libpal_opentissue --
if (make_opentissue) then
	pOpenTissue = PackageInfo.create()
	pOpenTissue.name = "libpal_opentissue"
	pOpenTissue.includepaths = {dirOpenTissue, dirBoost, dirOpenTissue .. "dependencies", dirOpenTissue .. "externals/include"}
	if (windows) then
		tinsert(pOpenTissue.includepaths, dirOpenTissue .. "externals/include/windows")
	end
	pOpenTissue.files = {
		matchfiles(rloc .. "pal_i/opentissue*.h", rloc.."pal_i/opentissue*.cpp"),
	}
	if (windows) then
		pOpenTissue.libpaths = {dirOpenTissue .. "externals/lib/windows" }
	end
end

--Package : libpal_spe --
if (make_spe) then
	pSPE = PackageInfo.create()
	pSPE.name = "libpal_spe"
	pSPE.includepaths = {dirSPE .. "include"}
	pSPE.files = {
		matchfiles(rloc .. "pal_i/spe*.h", rloc.."pal_i/spe*.cpp"),
	}
	pSPE.libpaths = {dirSPE .. "lib" }
end

--Package : libpal_tokamak --
if (make_tokamak) then
	pTokamak = PackageInfo.create()
	pTokamak.name = "libpal_tokamak"
	if (use_qhull) then
		pTokamak.includepaths = { 
			dirTokamak .. "include",
			lloc .. "qhull/src"
		}
		pTokamak.files = { 
			matchfiles(rloc .. "pal_i/tokamak*.h", rloc.."pal_i/tokamak*.cpp"),
			rloc .. "pal_i/mFILE.c", 
			rloc .. "pal_i/mFILE.h", 
			rloc .. "pal_i/vlen.c", 
			rloc .. "pal_i/vlen.h"
			}
		pTokamak.libpaths = {
			dirTokamak .. "lib",
			lloc .. "qhull/lib"
			}
		pTokamak.defines = {"USE_QHULL","TOKAMAK_USE_DLL"}	
	else
		pTokamak.includepaths = { 
			dirTokamak .. "include"
		}
		pTokamak.files = { 
			matchfiles(rloc .. "pal_i/tokamak*.h", rloc.."pal_i/tokamak*.cpp")
			}
		pTokamak.libpaths = {
			dirTokamak .. "lib"
			}
		if (windows) then
			pTokamak.defines = {"TOKAMAK_USE_DLL"}
		end
	end
end --mt

--Package : libpal_trueaxis --
if (make_trueaxis) then
	pTrueAxis = PackageInfo.create()
	pTrueAxis.name = "libpal_trueaxis"
	pTrueAxis.includepaths = { 
		dirTrueAxis .. "TA"
	 }
	pTrueAxis.files = { 
		  matchfiles(rloc .. "pal_i/trueaxis*.h", rloc.."pal_i/trueaxis*.cpp")
		}
	pTrueAxis.libpaths = {dirTrueAxis .. "TA/Projects"}
	pTrueAxis.config["Debug"].libpaths = {dirTrueAxis .. "TA/Projects/Debug"  }
	pTrueAxis.config["Release"].libpaths = {dirTrueAxis .. "TA/Projects/Release"  }
end

--==============================================
if (make_box2d) then
	table.insert(pList,pBox2D)
	pBox2D:buildpackage()
end
if (make_bullet) then
	table.insert(pList,pBullet)
	pBullet:buildpackage()
end
if (make_ibds) then
	table.insert(pList,pIBDS)
	pIBDS:buildpackage()
end
if (make_jiggle) then
	table.insert(pList,pJiggle)
	pJiggle:buildpackage()
end
if (make_newton) then
	table.insert(pList,pNewton)
	pNewton:buildpackage()
end
if (make_novodex) then
	if (target~="vs6") then
	table.insert(pList,pNovodex)
	pNovodex:buildpackage()
	end
end
if (make_ode) then
	table.insert(pList,pODE)
	pODE:buildpackage()
end
if (make_opentissue) then
	table.insert(pList,pOpenTissue)
	pOpenTissue:buildpackage()
end
if (make_spe) then
	table.insert(pList,pSPE)
	pSPE:buildpackage()
end
if (make_tokamak) then
	table.insert(pList,pTokamak)
	pTokamak:buildpackage()
end
if (make_trueaxis) then
	if (target~="vs6") then
		table.insert(pList,pTrueAxis)
		pTrueAxis:buildpackage()
	end
end

if (windows) then --temporary: disable DLL on non-windows
	os.mkdir("bin/debug/" .. target)
	os.mkdir("bin/release/" .. target)
	--sdl
	CopyDLL(lloc .. "SDL/lib/","SDL.dll")
	--physics engine libraries
	if (make_newton) then
			CopyDLL(dirNewton .. "dll/","Newton.dll")
	end
	if (make_novodex) then
		CopyDLL(dirNovodex .. "bin/win32/","NxCooking.dll")
	end
	if (make_spe) then
		CopyDLL(dirSPE .. "bin/","SPE.dll")
	end
	if (make_ode) then
		--ode has seperate dll dirs
		os.copyfile(dirODE .. "lib/debugdll/ode.dll", project.config["Debug"].bindir .. "/ode.dll")
		os.copyfile(dirODE .. "lib/releasedll/ode.dll", project.config["Release"].bindir .. "/ode.dll")
	end
	if (make_tokamak) then
		CopyDLL(dirTokamak .. "lib/","tokamakdll.dll")
	end
end


--==============================================]]
--disabled pal example for now
--[[
if (windows) then
--Package : pal example --
package = newpackage()
if (windows) then
	package.links = { "libpal","libtest","User32" }
else
	package.links = { "libpal","libtest"}
end
configureSDLGL(package)
package.name = "palexample"
package.path = "build/" .. target
package.kind = "exe"
package.language = "c++"
package.includepaths = { 
	lloc .. "SDL/include"}
package.libpaths = {
	lloc .. "SDL/lib/"
}
if (windows) then
package.files = { 
	rloc .. "example/example.cpp",
	rloc .. "example/resource.rc"
	}
else
package.files = { 
	rloc .. "example/example.cpp"
	}
end
package.defines = {"WIN32";"_WINDOWS"}

for key,value in pairs(pList) do
	MergeTable(package.includepaths,value.includepaths)
	MergeTable(package.libpaths,value.libpaths)
	MergeTable(package.files,value.files)
	if (value.config["Debug"].libpaths ~= nil) then
		MergeTable(package.config["Debug"].libpaths,value.config["Debug"].libpaths)
	end
	if (value.config["Release"].libpaths ~= nil) then
		MergeTable(package.config["Release"].libpaths,value.config["Release"].libpaths)
	end
end

if (target =="gnu") then
	package.linkoptions = { "-lmingw32 -mwindows" }
end
end --if windows
]]--
--==============================================
--Package : pal beginner example --

package = newpackage()
if (windows) then
	package.links = { "libpal","User32" }
else
	package.links = { "libpal"}
end

package.name = "palbeginner"
package.path = "build/" .. target
package.kind = "exe"
package.language = "c++"

if (windows) then
	package.defines = {"WIN32";"_WINDOWS"}
end
if (internal_debug) then
		package.config["Debug"].defines = {"INTERNAL_DEBUG"}
end

if (static_example) then --static linking
	package.files = { 
		rloc .. "example/begin.cpp",
		}

	--change to the name of the package used for static linking lib:
	value = pTokamak
	if (value.links ~= nil) then
		MergeTable(package.links,value.links)
	end
	if (value.defines ~= nil) then
		MergeTable(package.defines,value.defines)
	end

	MergeTable(package.includepaths,value.includepaths)
	MergeTable(package.libpaths,value.libpaths)
	MergeTable(package.files,value.files)
	if (value.config["Debug"].libpaths ~= nil) then
		MergeTable(package.config["Debug"].libpaths,value.config["Debug"].libpaths)
	end
	if (value.config["Release"].libpaths ~= nil) then
		MergeTable(package.config["Release"].libpaths,value.config["Release"].libpaths)
	end
else --dynamic linking
	package.files = { 
		rloc .. "example/begin_dynamic.cpp",
		}
end

if (windows) then
	if (target =="gnu") then
		package.linkoptions = { "-lmingw32 -mwindows" }
	end
else
	if (target =="gnu") then
		package.linkoptions = { "-ldl" }
	end
end

--====================================
-- Physics Abstraction Layer Premake (c) Adrian Boeing 2007 
-- PAL : http://pal.sourceforge.net/
-- premake : http://premake.sourceforge.net/ 
--[[
Copyright (c) 2004-2008, Adrian Boeing.
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
Neither the name of PAL nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
]]--
--====================================