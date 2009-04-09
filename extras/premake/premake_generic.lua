-- Physics Abstraction Layer Premake (c) Adrian Boeing 2007 
-- PAL : http://pal.sourceforge.net/
-- premake : http://premake.sourceforge.net/ 
--====================================

--set the library location (where ode, bullet, tokamak, etc. live)
if (windows) then
	lloc = "$(LIB_DIR)/"
else
	lloc = "../../" --edit this line for your platform!
end

dirTokamak = lloc .. "tokamak/"
dirTrueAxis = lloc .. "TrueAxisPhysicsSDKNonCommercial/"
dirODE = lloc .. "ODE/"
dirNovodex = lloc .. "PhysX/"
dirNewton = lloc .. "NewtonSDK/sdk/"
dirJiggle = lloc .. "jiglib/"
dirBullet = lloc .. "bullet/"
dirIBDS = lloc .. "IBDS/"
dirSPE = lloc .. "SPE_SDK/"
dirOpenTissue = lloc .. "opentissue/"

make_bullet = true
make_ibds = true
make_jiggle = true
make_newton = true
make_novodex = true
make_ode = true
make_opentissue = true
make_spe = true
make_trueaxis = true
make_tokamak = true

use_qhull = true

static_example = false

dofile("premake_template.lua")