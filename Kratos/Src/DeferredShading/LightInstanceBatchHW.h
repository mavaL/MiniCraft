/********************************************************************
	created:	7:6:2013   13:51
	filename	LightInstanceBatchHW.h
	author:		maval

	purpose:	点光HW Instancing
				copy from OgreInstanceBatchHW.h/cpp
*********************************************************************/
#ifndef LightInstanceBatchHW_h__
#define LightInstanceBatchHW_h__

#include <OgreInstanceBatch.h>
#include "KratosPrerequisites.h"

namespace Kratos
{
	class LightInstanceBatchHW : public Ogre::InstanceBatch
	{
		bool	mKeepStatic;

		void setupVertices( const Ogre::SubMesh* baseSubMesh );
		void setupIndices( const Ogre::SubMesh* baseSubMesh );

		void removeBlendData();
		virtual bool checkSubMeshCompatibility( const Ogre::SubMesh* baseSubMesh );

		size_t updateVertexBuffer( Ogre::Camera *currentCamera );

	public:
		LightInstanceBatchHW( Ogre::InstanceManager *creator, Ogre::MeshPtr &meshReference, const Ogre::MaterialPtr &material,
							size_t instancesPerBatch, const Ogre::Mesh::IndexMap *indexToBoneMap,
							const Ogre::String &batchName );
		virtual ~LightInstanceBatchHW();

		/** @see InstanceBatch::calculateMaxNumInstances */
		size_t calculateMaxNumInstances( const Ogre::SubMesh *baseSubMesh, Ogre::uint16 flags ) const;

		/** @see InstanceBatch::buildFrom */
		void buildFrom( const Ogre::SubMesh *baseSubMesh, const Ogre::RenderOperation &renderOperation );

		/** Overloaded so that we don't perform needless updates when in static mode. Also doing that
			could cause glitches with shadow mapping (since Ogre thinks we're small/bigger than we
			really are when displaying, or that we're somewhere else)
        */
		void _boundsDirty(void);

		/** @see InstanceBatch::setStaticAndUpdate. While this flag is true, no individual per-entity
			cull check is made. This means if the camera is looking at only one instance, all instances
			are sent to the vertex shader (unlike when this flag is false). This saves a lot of CPU
			power and a bit of bus bandwidth.
		*/
		void setStaticAndUpdate( bool bStatic );

		bool isStatic() const						{ return mKeepStatic; }

		//Renderable overloads
		void getWorldTransforms( Ogre::Matrix4* xform ) const;
		unsigned short getNumWorldTransforms(void) const;

		/** Overloaded to avoid updating skeletons (which we don't support), check visibility on a
			per unit basis and finally updated the vertex buffer */
		virtual void _updateRenderQueue( Ogre::RenderQueue* queue );

		/** 直接绘制该batch
        */
		void injectRender();

		void updateLightInstanceData(const HashMap<Ogre::InstancedEntity*, Ogre::Light*>& lightMap);
	};
}

#endif // LightInstanceBatchHW_h__
