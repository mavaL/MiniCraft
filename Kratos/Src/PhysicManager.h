/********************************************************************
	created:	11:4:2013   20:18
	filename	PhysicManager.h
	author:		maval

	purpose:	物理模块管理器
*********************************************************************/

#ifndef PhysicManager_h__
#define PhysicManager_h__

#include "Singleton.h"
#include "KratosPrerequisites.h"

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

#define		Vec3_Ogre_to_Havok(vecOgre) std::move(hkVector4(vecOgre.x, vecOgre.y, vecOgre.z))
#define		Rot_Ogre_to_Havok(rotOgre)	std::move(hkQuaternion(rotOgre.x, rotOgre.y, rotOgre.z, rotOgre.w))
#define		Vec3_Havok_to_Ogre(vecHk)	std::move(FLOAT3(vecHk.v[0], vecHk.v[1], vecHk.v[2]))
#define		Rot_Havok_to_Ogre(rotHk)	std::move(ORIENT(rotHk.v[0], rotHk.v[1], rotHk.v[2], rotHk.v[3]))
}

#endif // PhysicManager_h__
