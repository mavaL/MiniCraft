/********************************************************************
	created:	24:1:2013   0:34
	filename: 	F:\MiniCraft\MiniCraft\Game\Client\Object\Building.h
	author:		maval
	
	purpose:	½¨ÖþÀà
*********************************************************************/

#ifndef Building_h__
#define Building_h__

#include "SelectableObject.h"
#include "GameDataDef.h"

class Building : public SelectableObject
{
public:
	Building();
	~Building() {}

	static const STRING sNamePrefix;

public:
	virtual eObjectType GetType() const { return eObjectType_Building; }
	virtual const STRING& GetNamePrefix() const {  return sNamePrefix; }
	virtual void	Update(float dt) {}

public:
	void			SetBuildingName(const STRING& name) { m_buildingName = name; }
	void			SetAbility(int slotIndex, const SAbilityData* pData);

protected:
	virtual	void	OnSelected(bool bSelected);

private:
	STRING			m_buildingName;
	SAbilityData*	m_pAbilitySlots[MAX_ABILITY_SLOT];
};


#endif // Building_h__