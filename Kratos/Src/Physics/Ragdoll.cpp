#include "stdafx.h"
#include "Ragdoll.h"
#include "PhysicManager.h"
#include <Animation/Ragdoll/Controller/PoweredConstraint/hkaRagdollPoweredConstraintController.h>
#include <Animation/Ragdoll/Controller/RigidBody/hkaRagdollRigidBodyController.h>
#include <Animation/Ragdoll/Utils/hkaRagdollUtils.h>
#include <Animation/Ragdoll/Instance/hkaRagdollInstance.h>

namespace Kratos
{
	Ragdoll::Ragdoll( Ogre::Entity* pEnt, Ogre::SceneNode* pNode, const STRING& rigFilename )
	:m_pEntity(pEnt)
	,m_pSceneNode(pNode)
	{
		hkpWorld* hkWorld = PHYSICMANAGER.m_pWorld;
		hkWorld->lock();

		hkaSkeleton* animationSkeleton;

		STRING filepath = PHYSICMANAGER.GetResPath();
		filepath += rigFilename;
		hkIstream stream(filepath.c_str());
		hkSerializeUtil::ErrorDetails loadError;
		hkResource* resource = hkSerializeUtil::load(stream.getStreamReader(), &loadError);
		HK_ASSERT2(0xa6451543, resource != HK_NULL, loadError.defaultMessage.cString());
		hkRootLevelContainer* container = resource->getContents<hkRootLevelContainer>();

		hkaAnimationContainer* ac = reinterpret_cast<hkaAnimationContainer*>( container->findObjectByType( hkaAnimationContainerClass.getName() ));
		HK_ASSERT2(0x27343435, ac && (ac->m_skeletons.getSize() == 2), "No skeleton loaded");

		if (ac->m_skeletons[0]->m_bones.getSize() >= ac->m_skeletons[1]->m_bones.getSize())
			animationSkeleton =  ac->m_skeletons[0] ;
		else
			animationSkeleton = ac->m_skeletons[1];

		m_pHighSkeleton = new hkaAnimatedSkeleton( animationSkeleton );
		m_pRagdollInstance = (hkaRagdollInstance*)(container->findObjectByType(hkaRagdollInstanceClass.getName()));
		m_pRagdollInstance->addToWorld(hkWorld, true);
		const hkaSkeleton* ragdollSkel = m_pRagdollInstance->getSkeleton();

		// Locks all translations, except root.
		hkaSkeletonUtils::lockTranslations(*animationSkeleton, true);
		// Notice: Only "Triangle Pelvis Setup" should unlock these
		const char* boneName[] = { "Ragdoll_HavokBipedRig L Thigh", "Ragdoll_HavokBipedRig R Thigh", "Bip01 L Thigh", "Bip01 R Thigh" };
		const hkaSkeleton* skeleton[] = { ragdollSkel, ragdollSkel, animationSkeleton, animationSkeleton };
		for ( int i = 0; i < 4; i++ )
		{
			const hkInt16 bone = hkaSkeletonUtils::findBoneWithName( *skeleton[ i ], boneName[ i ] );
			HK_ASSERT3( 0x0ceee644, bone >= 0, "Bone " << boneName << " not found." );
			const_cast<hkaSkeleton*>(skeleton[ i ])->m_bones[ bone ].m_lockTranslation = false;
		}

		hkaSkeletonMapper* tempMapper1,*tempMapper2;
		tempMapper1 = (hkaSkeletonMapper*)(container->findObjectByType(hkaSkeletonMapperClass.getName()));
		tempMapper2 = (hkaSkeletonMapper*)(container->findObjectByType(hkaSkeletonMapperClass.getName(),tempMapper1));
		if (tempMapper1->m_mapping.m_skeletonA == animationSkeleton)
		{
			m_pHighToLowMapper = tempMapper1;
			m_pLowToHighMapper = tempMapper2;
		}
		else
		{
			m_pHighToLowMapper = tempMapper2;
			m_pLowToHighMapper = tempMapper1;
		}

		m_pHighPose = new hkaPose(m_pHighSkeleton->getSkeleton());
		m_pRagdollPose = new hkaPose(m_pRagdollInstance->getSkeleton());

		for(int i=0; i<m_pRagdollInstance->getNumBones(); ++i)
		{
			hkpRigidBody* rb = m_pRagdollInstance->getRigidBodyOfBone(i);
			rb->setMotionType(hkpMotion::MOTION_DYNAMIC);
		}

		hkWorld->unlock();
	}

	void Ragdoll::StartRagdoll()
	{
		///同步ragoll的初始化high pose
		Ogre::Skeleton* pSkel = m_pEntity->getSkeleton();
		Ogre::Skeleton::BoneIterator iter = pSkel->getBoneIterator();
		size_t numBones = pSkel->getNumBones();
		iter = pSkel->getBoneIterator();
		hkArray<hkQsTransform> initHighPoseArray(numBones + 1);

		for (size_t i=1; i<=numBones; ++i)
		{
			Ogre::Bone* bone = iter.peekNext();
			initHighPoseArray[i].m_scale		= Vec3_Ogre_to_Havok(bone->getScale());
			initHighPoseArray[i].m_rotation		= Rot_Ogre_to_Havok(bone->getOrientation());
			initHighPoseArray[i].m_translation	= Vec3_Ogre_to_Havok(bone->getPosition());
			iter.moveNext();
		}
		Ogre::Bone* pRoot = pSkel->getRootBone();
		initHighPoseArray[0].m_scale		= Vec3_Ogre_to_Havok(pRoot->getScale());
		initHighPoseArray[0].m_rotation		= Rot_Ogre_to_Havok(pRoot->getOrientation());
		initHighPoseArray[0].m_translation	= Vec3_Ogre_to_Havok(pRoot->getPosition());

		m_pHighPose->setPoseLocalSpace(initHighPoseArray);

		///high pose -> low pose
		m_pRagdollPose->setToReferencePose();
		m_pHighToLowMapper->mapPose(*m_pHighPose, *m_pRagdollPose, hkaSkeletonMapper::CURRENT_POSE );

		///drive the ragdoll
		hkQsTransform worldTrans;
		worldTrans.m_scale = Vec3_Ogre_to_Havok(m_pSceneNode->_getDerivedScale());
		worldTrans.m_rotation = Rot_Ogre_to_Havok(m_pSceneNode->_getDerivedOrientation());
		worldTrans.m_translation = Vec3_Ogre_to_Havok(m_pSceneNode->_getDerivedPosition());
		m_pRagdollInstance->setPoseModelSpace(m_pRagdollPose->getSyncedPoseModelSpace().begin(), worldTrans);
	}

	void Ragdoll::UpdateRagdoll()
	{
 		//low pose -> high pose 
		m_pHighPose->setToReferencePose();
 		m_pRagdollInstance->getPoseModelSpace(m_pRagdollPose->accessUnsyncedPoseModelSpace().begin(), hkQsTransform::getIdentity());
 		m_pRagdollPose->syncAll();
 		m_pLowToHighMapper->mapPose(*m_pRagdollPose, *m_pHighPose, hkaSkeletonMapper::CURRENT_POSE);
 
 		//high pose -> ogre skeleton
 		const hkaSkeleton* hkSkeleton = m_pHighPose->getSkeleton();
 		Ogre::Skeleton* ogreSkel = m_pEntity->getSkeleton();
 
 		const hkArray<hkQsTransform>& localSpaceTransformArray = m_pHighPose->getSyncedPoseLocalSpace();
   		const hkQsTransform& transRoot = localSpaceTransformArray[0];

// 		hkQsTransform worldTrans;
// 		m_pRagdollInstance->getWorldFromBoneTransform(0, worldTrans);
//   		m_pSceneNode->setScale(Vec3_Havok_to_Ogre(worldTrans.m_scale.getQuad()));
//   		m_pSceneNode->setOrientation(Rot_Havok_to_Ogre(worldTrans.m_rotation.m_vec.getQuad()));
//   		m_pSceneNode->setPosition(Vec3_Havok_to_Ogre(worldTrans.m_translation.getQuad()));
 
 		ogreSkel->getRootBone()->setPosition(Vec3_Havok_to_Ogre(transRoot.m_scale.getQuad()));
 		ogreSkel->getRootBone()->setOrientation(Rot_Havok_to_Ogre(transRoot.m_rotation.m_vec.getQuad()));
 		ogreSkel->getRootBone()->setScale(Vec3_Havok_to_Ogre(transRoot.m_translation.getQuad()));
 
 		for (int i = 1;i < hkSkeleton->m_bones.getSize();++i)
 		{
 			const hkaBone& hkBone = hkSkeleton->m_bones[i];
 			const Ogre::String boneName(hkBone.m_name);
 			Ogre::Bone* ogreBone;
 			ogreBone = ogreSkel->getBone(boneName);
 			assert(ogreBone);
 			const hkQsTransform& trans = localSpaceTransformArray[i];
 
 			ogreBone->setPosition(Vec3_Havok_to_Ogre(trans.m_translation.getQuad()));
 			ogreBone->setOrientation(Rot_Havok_to_Ogre(trans.m_rotation.m_vec.getQuad()));
 			ogreBone->setScale(Vec3_Havok_to_Ogre(trans.m_scale.getQuad()));
 		}
 	}

}
