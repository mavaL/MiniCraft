// stdafx.h

#include <assert.h>

//Ogre
#include <Ogre.h>
#include <Terrain/OgreTerrain.h>
#include <Terrain/OgreTerrainGroup.h>
#include <Terrain/OgreTerrainQuadTreeNode.h>
#include <Terrain/OgreTerrainMaterialGeneratorA.h>

//OIS
#include <OISEvents.h>
#include <OISInputManager.h>
#include <OISKeyboard.h>
#include <OISMouse.h>

//CEGUI
#include <CEGUI.h>

//RecastÑ°Â·¿â
#include "Recast/Recast.h"
#include "DetourCrowd/DetourCrowd.h"
#include "OgreDetourTileCache.h"
#include "OgreDetourCrowd.h"

//STL
#include <string>
#include <vector>
#include <map>
#include <list>
#include <algorithm>

//rapidxml
#include "rapidxml/rapidxml.hpp"       
#include "rapidxml/rapidxml_utils.hpp"
#include "rapidxml/rapidxml_print.hpp"

#include <Windows.h>


#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p) { if(p) { p->Release(); p=NULL; } }
#endif

#ifndef SAFE_DELETE
#define SAFE_DELETE(p) { if(p) { delete p; p=NULL; } }
#endif

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(a) { if(p) { delete []p; p=NULL; } }
#endif

