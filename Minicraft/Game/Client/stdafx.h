// stdafx.h

#include <assert.h>

//Ogre
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
#include <RendererModules\Ogre\CEGUIOgreRenderer.h>
#include <RendererModules\Ogre\CEGUIOgreTexture.h>

//RecastѰ·��
#include "Recast/Recast.h"
#include "DetourCrowd/DetourCrowd.h"
#include "OgreDetourTileCache.h"
#include "OgreDetourCrowd.h"

//ParticleUniverse
#include <ParticleUniverseSystemManager.h>

//STL
#include <string>
#include <vector>
#include <map>
#include <list>
#include <algorithm>

//rapidxml
#include "rapidxml.hpp"       
#include "rapidxml_utils.hpp"
#include "rapidxml_print.hpp"

#include "EventCallbackBase.h"

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

