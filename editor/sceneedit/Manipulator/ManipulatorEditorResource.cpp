#include "stdafx.h"
#include "ManipulatorEditorResource.h"
#include "EditorDefine.h"
#include "Utility.h"


void ManipulatorResource::RenderAllMeshIcons( CImageList& retImageList, Ogre::StringVectorPtr& retMeshNames )
{
	using namespace Ogre;

	retImageList.Create(MESH_ICON_SIZE, MESH_ICON_SIZE, ILC_COLOR32, 100, 500);

	retMeshNames = ResourceGroupManager::getSingleton().findResourceNames(
		ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, "*.mesh");

	//没有mesh资源
	if(retMeshNames->empty())
		return;

	//准备工作...
	const PixelFormat imageFormat = PF_A8R8G8B8;
	TexturePtr texture = TextureManager::getSingleton().createManual( "MeshIconRtt", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, 
		TEX_TYPE_2D, 256, 256, 0, imageFormat , TU_RENDERTARGET );

	RenderTexture *rttTex = texture->getBuffer()->getRenderTarget();
	SceneManager *SceneMgr = Root::getSingletonPtr()->createSceneManager("OctreeSceneManager", "MeshIconSceneMgr");

	Light *dirl = SceneMgr->createLight("DisplayLight");
	dirl->setDirection(-1,-1,-1);
	dirl->setDiffuseColour(1,1,1);
	dirl->setType(Light::LT_DIRECTIONAL);

	Camera* RTTCam = SceneMgr->createCamera("MeshIconCamera");
	RTTCam->setNearClipDistance(0.01f);
	RTTCam->setFarClipDistance(0);
	RTTCam->setAspectRatio(1);
	RTTCam->setFOVy(Degree(90));
	RTTCam->setPosition(0,0,1);
	RTTCam->lookAt(0,0,0);

	Viewport *v = rttTex->addViewport( RTTCam );
	v->setClearEveryFrame( true );
	v->setBackgroundColour(ColourValue(0,0,0,0));

	Entity *entity;
	unsigned char dataptr[300 * 300 * 6]; 
	unsigned char *dataptr2;
	PixelBox pb(256,256,1,imageFormat, dataptr);
	//开始渲染
	for(size_t i=0; i<retMeshNames->size(); ++i)
	{
		entity = SceneMgr->createEntity("TempEntity", retMeshNames->at(i));
		SceneMgr->getRootSceneNode()->attachObject(entity);

		Vector3 vSize = entity->getBoundingBox().getHalfSize();
		Vector3 vCenter = entity->getBoundingBox().getCenter(); 
		vSize += Vector3(vSize.z, vSize.z, vSize.z);
		float maxsize = max(max(vSize.x,vSize.y),vSize.z);
		vSize = Vector3(0, 0, maxsize * 1.1f) + vCenter;

		RTTCam->setPosition(vSize.x,vSize.y,vSize.z);
		RTTCam->lookAt(vCenter.x,vCenter.y,vCenter.z);

		try
		{
			rttTex->update();
			rttTex->copyContentsToMemory(pb, RenderTarget::FB_FRONT);

			dataptr2 = new unsigned char[MESH_ICON_SIZE*MESH_ICON_SIZE*PixelUtil::getNumElemBytes(imageFormat)];
			PixelBox pb2(MESH_ICON_SIZE,MESH_ICON_SIZE,1,imageFormat, dataptr2);
			Image::scale(pb,pb2);

			//图像数据加入列表
			CBitmap bm;
			assert(bm.CreateBitmap(MESH_ICON_SIZE, MESH_ICON_SIZE, 1, PixelUtil::getNumElemBits(imageFormat), dataptr2));
			retImageList.Add(&bm, RGB(0,0,0));

			delete []dataptr2;
		}
		catch(...)
		{
			assert(0);
		}

		entity->detachFromParent();
		SceneMgr->destroyEntity(entity);
	}

	//清理工作...
	rttTex->removeAllViewports();
	Root::getSingletonPtr()->destroySceneManager(SceneMgr);
	TextureManager::getSingletonPtr()->unload(texture->getName());
	TextureManager::getSingletonPtr()->remove(texture->getName());
}

void ManipulatorResource::PrepareAllIcons()
{
	using namespace Ogre;

	//准备工作...
	const int iconSize = 64;
	const int origSize = 76;
	StringVectorPtr iconFiles = ResourceGroupManager::getSingleton().findResourceNames("Icon", "*.dds");
	Rectangle2D fullScreenQuad(true);
	fullScreenQuad.setCorners(-1, 1, 1, -1, false);
	fullScreenQuad.setUseIdentityProjection(true);
	fullScreenQuad.setUseIdentityView(true);
	fullScreenQuad.setMaterial("RenderIcon");

	const PixelFormat imageFormat = PF_A8R8G8B8;
	TexturePtr texture = TextureManager::getSingleton().createManual( "IconRtt", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, 
		TEX_TYPE_2D, origSize, origSize, 0, imageFormat , TU_RENDERTARGET );

	RenderTexture *rttTex = texture->getBuffer()->getRenderTarget();
	SceneManager *SceneMgr = Root::getSingletonPtr()->createSceneManager("OctreeSceneManager", "IconSceneMgr");
	Camera* RTTCam = SceneMgr->createCamera("IconCamera");

	Viewport *v = rttTex->addViewport( RTTCam );
	v->setClearEveryFrame( true );
	v->setBackgroundColour(ColourValue(0,0,0,0));

	SceneMgr->getRootSceneNode()->attachObject(&fullScreenQuad);
	(const_cast<AxisAlignedBox&>(fullScreenQuad.getBoundingBox())).setInfinite();

	unsigned char dataptr[origSize * origSize * 4]; 
	unsigned char *dataptr2;
	PixelBox pb(origSize, origSize, 1, imageFormat, dataptr);

	for (size_t i=0; i<iconFiles->size(); ++i)
	{
		const Ogre::String& filename = iconFiles->at(i);
		fullScreenQuad.getMaterial()->getTechnique(0)->getPass(0)->getTextureUnitState(0)->setTextureName(filename);
		//开始渲染
		try
		{
			rttTex->update();
			rttTex->copyContentsToMemory(pb, RenderTarget::FB_FRONT);

			dataptr2 = new unsigned char[iconSize*iconSize*PixelUtil::getNumElemBytes(imageFormat)];
			PixelBox pb2(iconSize,iconSize,1,imageFormat, dataptr2);
			Image::scale(pb,pb2);
			//图像加入容器
			Gdiplus::Bitmap* pIcon = new Gdiplus::Bitmap(iconSize, iconSize, iconSize*4, PixelFormat32bppARGB, dataptr2);
			m_icons.insert(std::make_pair(Utility::EngineToUnicode(filename), pIcon));

			delete []dataptr2;
		}
		catch(...)
		{
			assert(0);
		}
	}
	//清理工作...
	rttTex->removeAllViewports();
	Root::getSingletonPtr()->destroySceneManager(SceneMgr);
	TextureManager::getSingletonPtr()->unload(texture->getName());
	TextureManager::getSingletonPtr()->remove(texture->getName());
}

Gdiplus::Bitmap* ManipulatorResource::GetIcon( const std::wstring& iconName ) const
{
	auto iter = m_icons.find(iconName);
	assert(iter != m_icons.end());
	return iter->second;
}
