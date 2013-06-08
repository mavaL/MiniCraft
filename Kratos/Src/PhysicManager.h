/********************************************************************
	created:	11:4:2013   20:18
	filename	PhysicManager.h
	author:		maval

	purpose:	物理模块管理器
*********************************************************************/

#ifndef PhysicManager_h__
#define PhysicManager_h__

#if USE_PHYSICS == 1

#include "Singleton.h"
#include "KratosPrerequisites.h"
#include <Common/Base/hkBase.h>
#include <Common/Base/Math/Vector/hkVector4.h>
#include <Common/Base/Math/Quaternion/hkQuaternion.h>

class hkpWorld;
class hkVisualDebugger;
class hkpPhysicsContext;
class hkResource;

namespace Kratos
{
	class CPhysicManager : public CSingleton<CPhysicManager>
	{
		DECLEAR_SINGLETON(CPhysicManager);

		CPhysicManager(void);
		~CPhysicManager(void) {}

	public:
		bool	Init();
		void	Update();
		void	Shutdown();

		hkpWorld*				m_pWorld;

	public:
		const STRING&			GetResPath() const { return m_resPath; }
		void					CreateGround();

	private:
		hkpPhysicsContext*		m_context;
		hkVisualDebugger*		m_pVD;
		STRING					m_resPath;
	};

hkVector4		Vec3_Ogre_to_Havok(const FLOAT3& vecOgre);
hkQuaternion	Rot_Ogre_to_Havok(const ORIENT& rotOgre);
FLOAT3			Vec3_Havok_to_Ogre(const hkVector4& vecHk);	
ORIENT			Rot_Havok_to_Ogre(const hkQuaternion& rotHk);
}

#endif

#endif // PhysicManager_h__
