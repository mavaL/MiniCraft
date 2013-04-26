/********************************************************************
	created:	11:4:2013   23:28
	filename	Ragdoll.h
	author:		maval

	purpose:	Ragdoll·â×°
*********************************************************************/

#ifndef Ragdoll_h__
#define Ragdoll_h__

#include "Singleton.h"
#include "KratosPrerequisites.h"

class hkaRagdollInstance;
class hkaSkeletonMapper;
class hkaAnimatedSkeleton;
class hkaPose;

namespace Kratos
{
	class Ragdoll
	{
	public:
		Ragdoll(Ogre::Entity* pEnt, Ogre::SceneNode* pNode, const STRING& rigFilename);
		~Ragdoll() {}

	public:
		void	StartRagdoll();
		void	UpdateRagdoll();
		void	LogSkeletonData();

	private:
		Ogre::Entity*			m_pEntity;
		Ogre::SceneNode*		m_pSceneNode;
		hkaSkeletonMapper*		m_pHighToLowMapper; 
		hkaSkeletonMapper*		m_pLowToHighMapper;
		hkaAnimatedSkeleton*	m_pHighSkeleton;	
		hkaRagdollInstance*		m_pRagdollInstance;
		hkaPose*				m_pHighPose;
		hkaPose*				m_pRagdollPose;
	};

}

#endif // Ragdoll_h__
