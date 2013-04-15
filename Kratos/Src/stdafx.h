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

//RecastÑ°Â·¿â
#include "Recast/Recast.h"
#include "DetourCrowd/DetourCrowd.h"
#include "OgreDetourTileCache.h"
#include "OgreDetourCrowd.h"

//ParticleUniverse
#include <ParticleUniverseSystemManager.h>

//Havok
#include <Common/Base/hkBase.h>
#include <Common/Base/Memory/System/Util/hkMemoryInitUtil.h>
#include <Common/Base/Memory/Allocator/Malloc/hkMallocAllocator.h>
#include <Common/Base/Fwd/hkcstdio.h>
#include <Common/SceneData/Scene/hkxScene.h>
#include <Common/Serialize/Util/hkRootLevelContainer.h>
#include <Common/Visualize/hkDebugDisplay.h>
#include <Common/Visualize/hkVisualDebugger.h>
#include <Common/Base/System/Io/IStream/hkIStream.h>
#include <Common/Serialize/Util/hkSerializeUtil.h>
#include <Common/Serialize/Util/hkLoader.h>
#include <Common/Base/Math/Vector/hkVector4.h>
#include <Common/Base/Math/Quaternion/hkQuaternion.h>

#include <Physics/Dynamics/World/hkpWorld.h>
#include <Physics/Collide/Dispatch/hkpAgentRegisterUtil.h>
#include <Physics/Dynamics/Entity/hkpRigidBody.h>
#include <Physics/Collide/Shape/Convex/Box/hkpBoxShape.h>
#include <Physics/Utilities/Dynamics/Inertia/hkpInertiaTensorComputer.h>
#include <Physics/Dynamics/World/BroadPhaseBorder/hkpBroadPhaseBorder.h>
#include <Physics/Utilities/VisualDebugger/hkpPhysicsContext.h>
#include <Physics/Collide/Filter/Group/hkpGroupFilter.h>

#include <Animation/Animation/hkaAnimationContainer.h>
#include <Animation/Animation/Mapper/hkaSkeletonMapper.h>
#include <Animation/Animation/Playback/Control/Default/hkaDefaultAnimationControl.h>
#include <Animation/Animation/Playback/hkaAnimatedSkeleton.h>
#include <Animation/Animation/Rig/hkaPose.h>
#include <Animation/Animation/Rig/hkaSkeletonUtils.h>


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

