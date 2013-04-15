#include "stdafx.h"
#include "PhysicManager.h"
#include "OgreManager.h"

#include <Common/Base/System/Init/PlatformInit.cxx>
#include <Common/Base/keycode.cxx>

// Productfeatures
// We're using only physics - we undef products even if the keycode is present so
// that we don't get the usual initialization for these products.
#undef HK_FEATURE_PRODUCT_AI
#undef HK_FEATURE_PRODUCT_CLOTH
#undef HK_FEATURE_PRODUCT_DESTRUCTION
#undef HK_FEATURE_PRODUCT_BEHAVIOR
#undef HK_FEATURE_PRODUCT_MILSIM

//#define HK_EXCLUDE_FEATURE_SerializeDeprecatedPre700		
//#define HK_EXCLUDE_FEATURE_RegisterVersionPatches			
// Vdb needs the reflected classes
//#define HK_EXCLUDE_FEATURE_RegisterReflectedClasses
#define HK_EXCLUDE_FEATURE_MemoryTracker

#include <Common/Base/Config/hkProductFeatures.cxx>

namespace Kratos
{
	static void HK_CALL errorReport(const char* msg, void* userContext)
	{
		STRING message(msg);
		message += "\n";
		Ogre::LogManager::getSingleton().logMessage(message);
	}

	CPhysicManager::CPhysicManager( void )
	:m_pWorld(nullptr)
	,m_pVD(nullptr)
	,m_context(nullptr)
	,m_resPath(Ogre::StringUtil::BLANK)
	{

	}

	bool CPhysicManager::Init()
	{
		assert(RenderManager.IsInit() && "错误!!图形系统还未初始化!");

		// Need to have memory allocated for the solver. Allocate 1mb for it.
		hkMemoryRouter* memoryRouter = hkMemoryInitUtil::initDefault( hkMallocAllocator::m_defaultMallocAllocator, hkMemorySystem::FrameInfo(1024 * 1024) );
		hkBaseSystem::init( memoryRouter, errorReport );

		float gravity = -9.8f;
		hkpWorldCinfo info;
		info.m_simulationType = hkpWorldCinfo::SIMULATION_TYPE_DISCRETE;
		info.m_gravity = hkVector4(0.0f, gravity, 0.0f);
		info.setBroadPhaseWorldSize(256);
		info.setupSolverInfo(hkpWorldCinfo::SOLVER_TYPE_8ITERS_MEDIUM);
		//info.m_solverIterations = 32;
		info.m_collisionTolerance = 0.01f; 

		m_pWorld = new hkpWorld(info);
		m_pWorld->setBroadPhaseBorder(new hkpBroadPhaseBorder(m_pWorld));
		m_pWorld->lock();
		hkpAgentRegisterUtil::registerAllAgents(m_pWorld->getCollisionDispatcher());
		m_pWorld->unlock();

		m_context = new hkpPhysicsContext;
		m_context->addWorld(m_pWorld); 
		hkpPhysicsContext::registerAllPhysicsProcesses();

		hkArray<hkProcessContext*> contexts;
		contexts.pushBack( m_context );  
		m_pVD = new hkVisualDebugger( contexts );
		m_pVD->serve();

		m_resPath = Ogre::ResourceGroupManager::getSingleton().findResourceLocation("Physics", "*")->at(0) + "\\";

		// Replace filter.
		const int COLLISION_LAYER_RAGDOLL = 0;
		hkpGroupFilter* filter  = new hkpGroupFilter();
		// Obtain a new system group that we will use for all the bodies in the ragdoll
		int ragdollSystemGroup = filter->getNewSystemGroup();
		// Caculate the filter info for the root body (e.g chest) with subsytem ID 0
		hkUint32 chestFilterInfo = hkpGroupFilter::calcFilterInfo(COLLISION_LAYER_RAGDOLL, ragdollSystemGroup, 0, 0);
		// Caculate the filter info for a body directly constrained to the root (e.g. neck) with subsystem ID 1
		hkUint32 neckFilterInfo = hkpGroupFilter::calcFilterInfo(COLLISION_LAYER_RAGDOLL, ragdollSystemGroup, 1, 0);
		// Caculate the filter info for a body constrained to the previous one (e.g head) with subsystem ID 2
		hkUint32 headFilterInfo = hkpGroupFilter::calcFilterInfo(COLLISION_LAYER_RAGDOLL, ragdollSystemGroup, 2, 1);
		// Enable collisions within the ragdoll layer if they were previously disabled
		filter->enableCollisionsBetween(COLLISION_LAYER_RAGDOLL, COLLISION_LAYER_RAGDOLL);
		m_pWorld->setCollisionFilter( filter, true);
		filter->removeReference();

		return true;
	}

	void CPhysicManager::Update()
	{
		m_pWorld->stepDeltaTime(1.0f/60);
		m_pVD->step();
	}

	void CPhysicManager::Shutdown()
	{
		m_pVD->removeReference();
		m_pWorld->removeReference();
		m_context->removeReference();

		hkBaseSystem::quit();
		hkMemoryInitUtil::quit();
	}

	void CPhysicManager::CreateGround()
	{
		hkVector4 halfExtents(64, 5, 64);
		hkpBoxShape* shape = new hkpBoxShape( halfExtents, 0.01f );
		hkpRigidBodyCinfo rigidBodyInfo;
		rigidBodyInfo.m_shape = shape;
		rigidBodyInfo.m_position.set(0, -halfExtents.getQuad().v[1], 0);
		rigidBodyInfo.m_motionType = hkpMotion::MOTION_FIXED;
		hkpRigidBody* rb = new hkpRigidBody(rigidBodyInfo);
		PHYSICMANAGER.m_pWorld->addEntity(rb);
		rb->removeReference();
		shape->removeReference();
	}

}


