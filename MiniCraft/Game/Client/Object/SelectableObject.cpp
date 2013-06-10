#include "stdafx.h"
#include "SelectableObject.h"
#include "World.h"
#include "GUIManager.h"
#include "CommandPanel.h"
#include "InfoPanel.h"
#include "OgreManager.h"
#include "PortraitPanel.h"
#include "GameDataDef.h"
#include "AIComponent.h"
#include "PathComponent.h"
#include "AnimatedComponent.h"
#include "HarvestComponent.h"
#include "Faction.h"
#include "BehaviorComponent.h"
#include "RagdollComponent.h"

/** This is ogre-procedural's temporary mesh buffer.
 * It stores all the info needed to build an Ogre Mesh, yet is intented to be more flexible, since
 * there is no link towards hardware.
 */
class TriangleBuffer
{
public:
	struct Vertex
	{
		FLOAT3 mPosition;
		FLOAT3 mNormal;
		FLOAT2 mUV;
	};
protected:

	std::vector<int> mIndices;

	std::vector<Vertex> mVertices;
	//std::vector<Vertex>::iterator mCurrentVertex;
	int globalOffset;
	int mEstimatedVertexCount;
	int mEstimatedIndexCount;
	Vertex* mCurrentVertex;


	public:
	TriangleBuffer() : globalOffset(0), mEstimatedVertexCount(0), mEstimatedIndexCount(0), mCurrentVertex(0)
	{}

	/// Gets a modifiable reference to vertices
	std::vector<Vertex>& getVertices()
	{
		return mVertices;
	}

	/// Gets a non-modifiable reference to vertices
	const std::vector<Vertex>& getVertices() const
	{
		return mVertices;
	}

	/// Gets a modifiable reference to vertices
	std::vector<int>& getIndices()
	{
		return mIndices;
	}

	/// Gets a non-modifiable reference to indices
	const std::vector<int>& getIndices() const
	{
		return mIndices;
	}

	/**
	 * Rebase index offset : call that function before you add a new mesh to the triangle buffer
	 */
	void rebaseOffset()
	{
		globalOffset = mVertices.size();
	}

	/**
	 * Builds an Ogre Mesh from this buffer.
	 */
	Ogre::MeshPtr transformToMesh(const std::string& name,
		const Ogre::String& group = Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME)
	{
		Ogre::SceneManager* sceneMgr = Ogre::Root::getSingleton().getSceneManagerIterator().begin()->second;
		Ogre::ManualObject * manual = sceneMgr->createManualObject();
		manual->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_TRIANGLE_LIST);

		for (std::vector<Vertex>::iterator it = mVertices.begin(); it != mVertices.end();it++)
		{
			manual->position(it->mPosition);
			manual->textureCoord(it->mUV);
			manual->normal(it->mNormal);
		}
		for (std::vector<int>::iterator it = mIndices.begin(); it!=mIndices.end();it++)
		{
			manual->index(*it);
		}
		manual->end();
		Ogre::MeshPtr mesh = manual->convertToMesh(name, group);

		unsigned short src, dest;
		if (!mesh->suggestTangentVectorBuildParams(Ogre::VES_TANGENT, src, dest))
		{
			mesh->buildTangentVectors(Ogre::VES_TANGENT, src, dest);
		}

		sceneMgr->destroyManualObject(manual);

		return mesh;
	}

	/** Adds a new vertex to the buffer */
	inline TriangleBuffer& position(const FLOAT3& pos)
	{
		Vertex v;
		v.mPosition = pos;
		mVertices.push_back(v);
		mCurrentVertex = &mVertices.back();
		return *this;
	}

	/** Adds a new vertex to the buffer */
	inline TriangleBuffer& position(float x, float y, float z)
	{
		Vertex v;
		v.mPosition = FLOAT3(x,y,z);
		mVertices.push_back(v);
		mCurrentVertex = &mVertices.back();
		return *this;
	}

	/** Sets the normal of the current vertex */
	inline TriangleBuffer& normal(const FLOAT3& normal)
	{
		mCurrentVertex->mNormal = normal;
		return *this;
	}

	/** Sets the texture coordinates of the current vertex */
	inline TriangleBuffer& textureCoord(float u, float v)
	{
		mCurrentVertex->mUV = FLOAT2(u,v);
		return *this;
	}

	/** Sets the texture coordinates of the current vertex */
	inline TriangleBuffer& textureCoord(const FLOAT2& vec)
	{
		mCurrentVertex->mUV = vec;
		return *this;
	}

	/**
	 * Adds an index to the index buffer.
	 * Index is relative to the latest rebaseOffset().
	 */
	inline TriangleBuffer& index(int i)
	{
		mIndices.push_back(globalOffset+i);
		return *this;
	}

	/**
	 * Adds a triangle to the index buffer.
	 * Index is relative to the latest rebaseOffset().
	 */
	inline TriangleBuffer& triangle(int i1, int i2, int i3)
	{
		mIndices.push_back(globalOffset+i1);
		mIndices.push_back(globalOffset+i2);
		mIndices.push_back(globalOffset+i3);
		return *this;
	}

	/// Applies a matrix to transform all vertices inside the triangle buffer
	TriangleBuffer& applyTransform(const Ogre::Matrix4& matrix)
	{
		for (std::vector<Vertex>::iterator it = mVertices.begin(); it!=mVertices.end(); it++)
		{
			it->mPosition = matrix * it->mPosition;
			it->mNormal = matrix * it->mNormal;
			it->mNormal.normalise();
		}
		return *this;
	}

	/// Applies the translation immediately to all the points contained in that triangle buffer
	/// @arg amount translation vector
	TriangleBuffer& translate(const FLOAT3& amount)
	{
		for (std::vector<Vertex>::iterator it = mVertices.begin(); it!=mVertices.end(); it++)
		{
			it->mPosition += amount;
		}
		return *this;
	}

	/// Applies the translation immediately to all the points contained in that triangle buffer
	TriangleBuffer& translate(float x, float y, float z)
	{
		return translate(FLOAT3(x, y, z));
	}

	/// Applies the rotation immediately to all the points contained in that triangle buffer
	/// @arg quat the rotation quaternion to apply
	TriangleBuffer& rotate(Ogre::Quaternion quat)
	{
		for (std::vector<Vertex>::iterator it = mVertices.begin(); it!=mVertices.end(); it++)
		{
			it->mPosition = quat * it->mPosition;
			it->mNormal = quat * it->mNormal;
			it->mNormal.normalise();
		}
		return *this;
	}

	/// Applies an immediate scale operation to that triangle buffer
	/// @arg scale Scale vector
	TriangleBuffer& scale(const FLOAT3& scale)
	{
		for (std::vector<Vertex>::iterator it = mVertices.begin(); it!=mVertices.end(); it++)
		{
			it->mPosition = scale * it->mPosition;
		}
		return *this;
	}

	/// Applies an immediate scale operation to that triangle buffer
	/// @arg x X scale component
	/// @arg y Y scale component
	/// @arg z Z scale component
	TriangleBuffer& scale(float x, float y, float z)
	{
		return scale(FLOAT3(x,y,z));
	}

	/// Applies normal inversion on the triangle buffer
	TriangleBuffer& invertNormals()
	{
		for (std::vector<Vertex>::iterator it = mVertices.begin(); it!=mVertices.end();it++)
		{
			it->mNormal = -it->mNormal;
		}
		for (unsigned int i=0; i < mIndices.size(); ++i)
		{
			if (i%3==1)
			{
				std::swap(mIndices[i], mIndices[i-1]);
			}
		}
		return *this;
	}

	/**
	 * Gives an estimation of the number of vertices need for this triangle buffer.
	 * If this function is called several times, it means an extra vertices count, not an absolute measure.
	 */
	void estimateVertexCount(unsigned int vertexCount)
	{
		mEstimatedVertexCount += vertexCount;
		mVertices.reserve(mEstimatedVertexCount);
	}

	/**
	 * Gives an estimation of the number of indices needed for this triangle buffer.
	 * If this function is called several times, it means an extra indices count, not an absolute measure.
	 */
	void estimateIndexCount(unsigned int indexCount)
	{
		mEstimatedIndexCount += indexCount;
		mIndices.reserve(mEstimatedIndexCount);
	}

	/**
	 * For debugging purposes, outputs the content of this buffer to a YAML styled file.
	 */
	void _dumpContentsToFile(const std::string& fileName)
	{
		std::ofstream outFile;
		outFile.open(fileName.c_str());

		outFile<< "Number of vertices : "<< Ogre::StringConverter::toString(mVertices.size()) <<std::endl;
		outFile<< "Estimated number of vertices : "<< Ogre::StringConverter::toString(mEstimatedVertexCount) <<std::endl;
		outFile<< "Vertices :"<<std::endl;
		for (std::vector<Vertex>::iterator it = mVertices.begin(); it!=mVertices.end();it++)
		{
			outFile<<" - {";
			outFile<<" Position: ["<<Ogre::StringConverter::toString(it->mPosition.x)<<", "<<Ogre::StringConverter::toString(it->mPosition.y)<<", "<<Ogre::StringConverter::toString(it->mPosition.z)<<"]";
			outFile<<", Normal: ["<<Ogre::StringConverter::toString(it->mNormal.x)<<", "<<Ogre::StringConverter::toString(it->mNormal.y)<<", "<<Ogre::StringConverter::toString(it->mNormal.z)<<"]";
			outFile<<", UV: ["<<Ogre::StringConverter::toString(it->mUV.x)<<", "<<Ogre::StringConverter::toString(it->mUV.y)<<"]";
			outFile<<"}"<<std::endl;
		}
		outFile<< "Number of indices : "<< Ogre::StringConverter::toString(mIndices.size()) <<std::endl;
		outFile<< "Estimated number of indices : "<< Ogre::StringConverter::toString(mEstimatedIndexCount) <<std::endl;
		outFile<< "Indices :"<< std::endl;
		for (size_t i = 0; i<mIndices.size()/3; i++)
		{
			outFile<<" - ["<<Ogre::StringConverter::toString(mIndices[i*3])<<", "<<Ogre::StringConverter::toString(mIndices[i*3+1])<<", "<<Ogre::StringConverter::toString(mIndices[i*3+2])<<"]"<<std::endl;
		}

		outFile.close();
	}
};
/////////////////////////////////////////////////////////////////////
std::unordered_map<STRING, Ogre::MeshPtr> SelectableObject::m_selCircleCache;

SelectableObject::SelectableObject()
:RenderableObject()
,m_pSelCircleNode(nullptr)
,m_bSelected(false)
,m_pActiveAbility(nullptr)
,m_pAi(nullptr)
,m_pPath(nullptr)
,m_pAnim(nullptr)
,m_pGather(nullptr)
,m_pBehavior(nullptr)
,m_pRagdoll(nullptr)
{
	
	memset(m_pAbilitySlots, 0, MAX_ABILITY_SLOT * sizeof(SAbilityData*));
}

SelectableObject::~SelectableObject()
{
}

void SelectableObject::SetSelected( bool bSelected )
{
	if(m_bSelected == bSelected)
		return;

	assert(IsRenderableReady());

	m_bSelected = bSelected;
	_OnSelected(bSelected);

	//已创建选中框
	//效率考虑,不会反复创建销毁它
	if (m_pSelCircleNode)
	{
		m_pSelCircleNode->setVisible(bSelected);
		return;
	}

	const Ogre::MeshPtr& objMesh = m_pEntity->getMesh();
	auto iter = m_selCircleCache.find(objMesh->getName());
	if (iter == m_selCircleCache.end())
	{
		//创建该单位的选中框
		Ogre::MeshPtr mesh = _CreateSelectionCircleMesh(objMesh);
		iter = m_selCircleCache.insert(std::make_pair(objMesh->getName(), mesh)).first;
	}

	//创建场景节点
	Ogre::Entity* pEntity = RenderManager.m_pSceneMgr->createEntity((iter->second));
	m_pSelCircleNode = m_pSceneNode->createChildSceneNode();
	m_pSelCircleNode->attachObject(pEntity);
	m_pSelCircleNode->setVisible(bSelected);

	//设置材质
	eObjectType type = GetType();
	if(type == eObjectType_Unit || type == eObjectType_Building)
		pEntity->setMaterialName("SelectionCircleAlly");
	else if(type == eObjectType_Resource)
		pEntity->setMaterialName("SelectionCircleNeutral");
}

Ogre::MeshPtr SelectableObject::_CreateSelectionCircleMesh(const Ogre::MeshPtr& objMesh)
{
	int mNumSegBase = 16;
	int mNumSegHeight = 1;
	float mOuterRadius = objMesh->getBounds().getHalfSize().x;
	float mInnerRadius = mOuterRadius * 0.9f;
	float mHeight = 0.1f;

	TriangleBuffer buffer;
	buffer.rebaseOffset();
	buffer.estimateVertexCount((mNumSegHeight+1)*(mNumSegBase+1)*2+(mNumSegBase+1)*4);
	buffer.estimateIndexCount(6*(mNumSegBase+1)*mNumSegHeight*2+6*mNumSegBase*2);

	float deltaAngle = (Ogre::Math::TWO_PI / mNumSegBase);
	float deltaHeight = mHeight/(float)mNumSegHeight;
	int offset = 0;

	for (int i = 0; i <=mNumSegHeight; i++)
		for (int j = 0; j<=mNumSegBase; j++)
		{
			float x0 = mOuterRadius * cosf(j*deltaAngle);
			float z0 = mOuterRadius * sinf(j*deltaAngle);
			buffer.position(FLOAT3(x0, i*deltaHeight, z0));

			if (i != mNumSegHeight) 
			{
				buffer.index(offset + mNumSegBase + 1);
				buffer.index(offset);
				buffer.index(offset + mNumSegBase);
				buffer.index(offset + mNumSegBase + 1);
				buffer.index(offset + 1);
				buffer.index(offset);
			}
			offset ++;
		}

		for (int i = 0; i <=mNumSegHeight; i++)
			for (int j = 0; j<=mNumSegBase; j++)
			{
				float x0 = mInnerRadius * cosf(j*deltaAngle);
				float z0 = mInnerRadius * sinf(j*deltaAngle);
				buffer.position(FLOAT3(x0, i*deltaHeight, z0));

				if (i != mNumSegHeight) 
				{
					buffer.index(offset + mNumSegBase + 1);
					buffer.index(offset + mNumSegBase);
					buffer.index(offset);
					buffer.index(offset + mNumSegBase + 1);
					buffer.index(offset);
					buffer.index(offset + 1);
				}
				offset ++;
			}


			//low cap
			for (int j=0;j<=mNumSegBase;j++)
			{
				float x0 = mInnerRadius * cosf(j*deltaAngle);
				float z0 = mInnerRadius * sinf(j*deltaAngle);

				buffer.position(FLOAT3(x0, 0.0f, z0));

				x0 = mOuterRadius * cosf(j*deltaAngle);
				z0 = mOuterRadius * sinf(j*deltaAngle);

				buffer.position(FLOAT3(x0, 0.0f, z0));

				if (j!=mNumSegBase)
				{
					buffer.index(offset);
					buffer.index(offset+1);
					buffer.index(offset+3);
					buffer.index(offset+2);
					buffer.index(offset);
					buffer.index(offset+3);
				}
				offset+=2;
			}


			//high cap
			for (int j=0;j<=mNumSegBase;j++)
			{
				float x0 = mInnerRadius * cosf(j*deltaAngle);
				float z0 = mInnerRadius * sinf(j*deltaAngle);

				buffer.position(FLOAT3(x0, mHeight, z0));

				x0 = mOuterRadius * cosf(j*deltaAngle);
				z0 = mOuterRadius * sinf(j*deltaAngle);

				buffer.position(FLOAT3(x0, mHeight, z0));

				if (j!=mNumSegBase)
				{
					buffer.index(offset+1);
					buffer.index(offset);
					buffer.index(offset+3);
					buffer.index(offset);
					buffer.index(offset+2);
					buffer.index(offset+3);
				}
				offset+=2;
			}

	return buffer.transformToMesh(objMesh->getName()+"_SelectionCircle");
}

void SelectableObject::ReleaseMeshCache()
{
	for(auto iter=m_selCircleCache.begin(); iter!=m_selCircleCache.end(); ++iter)
		(iter->second).setNull();
	m_selCircleCache.clear();
}

void SelectableObject::_OnSelected( bool bSelected )
{
	UiCommandPanel* pCmdPanel = World::GetSingleton().GetCommandPanel();
	UiInfoPanel*	pInfoPanel = World::GetSingleton().GetInfoPanel();
	UiPortraitPanel* pPortraitPanel = World::GetSingleton().GetPortraitPanel();

	//更新命令面板
	if (bSelected)
	{
		const auto& selObjs = World::GetSingleton().GetSelectedObjects();
		//TODO: 多选状态下命令面板应该怎样?
		if(selObjs.size() > 1)
			return;

		//设置UI图标
		for(int i=0; i<MAX_ABILITY_SLOT; ++i)
		{
			if(m_pAbilitySlots[i])
				pCmdPanel->CEGUI_SetAbilitySlot(i, true, m_pAbilitySlots[i]->m_iconName, "full_image");
		}

		pCmdPanel->SetActiveObject(this);
		pInfoPanel->SetActiveObject(this);
		pPortraitPanel->SetActiveObject(this);
	}
	else
	{
		//清空UI图标
		for(int i=0; i<MAX_ABILITY_SLOT; ++i)
		{
			if(m_pAbilitySlots[i])
				pCmdPanel->CEGUI_SetAbilitySlot(i, false);
		}

		pCmdPanel->SetActiveObject(nullptr);
		pInfoPanel->SetActiveObject(nullptr);
		pPortraitPanel->SetActiveObject(nullptr);
	}
}

void SelectableObject::SetAbility( int slotIndex, const SAbilityData* pData )
{
	assert(slotIndex>=0 && slotIndex < MAX_ABILITY_SLOT);
	m_pAbilitySlots[slotIndex] = (SAbilityData*)pData;
}

void SelectableObject::_OnCommandFinished(eCommandType cmd)
{
	GetAi()->_OnCommandFinished();
}

void SelectableObject::CreateRenderInstance()
{
	__super::CreateRenderInstance();

	//Movable对象与逻辑对象绑定,方便取出通信
	m_pEntity->setUserAny(Ogre::Any(this));
}

bool SelectableObject::HasAbility( eCommandType type )
{
	for (int i=0; i<MAX_ABILITY_SLOT; ++i)
	{
		if(m_pAbilitySlots[i] && m_pAbilitySlots[i]->m_type == type)
			return true;
	}

	return false;
}

AiComponent* SelectableObject::GetAi()
{
#ifdef DEBUG
	QueryComponent(this, eComponentType_AI, AiComponent);
#endif
	return m_pAi;
}

PathComponent* SelectableObject::GetPath()
{
#ifdef DEBUG
	return QueryComponent(this, eComponentType_Path, PathComponent);
#endif
	return m_pPath;
}

AnimatedComponent* SelectableObject::GetAnim()
{
#ifdef DEBUG
	QueryComponent(this, eComponentType_Animated, AnimatedComponent);
#endif
	return m_pAnim;
}

HarvestComponent* SelectableObject::GetGather()
{
#ifdef DEBUG
	QueryComponent(this, eComponentType_Harvest, HarvestComponent);
#endif
	return m_pGather;
}

BehaviorComponent* SelectableObject::GetBehavior()
{
#ifdef DEBUG
	QueryComponent(this, eComponentType_Behevior, BehaviorComponent);
#endif
	return m_pBehavior;
}

RagdollComponent* SelectableObject::GetRagdoll()
{
#ifdef DEBUG
	QueryComponent(this, eComponentType_Ragoll, RagdollComponent);
#endif
	return m_pRagdoll;
}

void SelectableObject::InitTeamColor(const COLOR& color)
{
	//队伍颜色与Renderable绑定
	size_t num = m_pEntity->getNumSubEntities();
	assert(num);
	for (size_t i=0; i<num; ++i)
		m_pEntity->getSubEntity(i)->setUserAny(Ogre::Any(color));
}

void SelectableObject::AddComponent( eComponentType type, Component* pCo )
{
	Object::AddComponent(type, pCo);
	switch (type)
	{
	case eComponentType_AI:			m_pAi		= QueryComponent(this, eComponentType_AI, AiComponent); break;
	case eComponentType_Path:		m_pPath		= QueryComponent(this, eComponentType_Path, PathComponent); break;
	case eComponentType_Animated:	m_pAnim		= QueryComponent(this, eComponentType_Animated, AnimatedComponent); break;
	case eComponentType_Harvest:	m_pGather	= QueryComponent(this, eComponentType_Harvest, HarvestComponent); break;
	case eComponentType_Behevior:	m_pBehavior = QueryComponent(this, eComponentType_Behevior, BehaviorComponent); break;
	case eComponentType_Ragoll:		m_pRagdoll	= QueryComponent(this, eComponentType_Ragoll, RagdollComponent); break;
	}
}



