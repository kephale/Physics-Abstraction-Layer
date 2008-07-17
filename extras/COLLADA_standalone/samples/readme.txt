DAE2XML written by John W. Ratcliff August 2, 2006
modified by Adrian Boeing 2007 for PAL support

This is the PAL COLLADA Loader.

The COLLADA Loader was written by John Ratcliff.

You can visit his excellent website here:
http://codesuppository.blogspot.com/

John Ratcliff's COLLADA loader was modified by Adrian Boeing to include support for PAL.

You can visit the PAL website here:
http://pal.sourceforge.net/

Usage: DAE2XML <physics engine> <collada_file_name>
eg: DAE2XML Novodex deer_bound.dae
eg: DAE2XML Bullet hornbug.dae

NOTE: 
Not all physics engines support convex objects, and not all engines support generic links.
Of the engines that do, not all are implemented in PAL.

Objects are loaded in a paused mode. Press space bar to enable the simulation.
