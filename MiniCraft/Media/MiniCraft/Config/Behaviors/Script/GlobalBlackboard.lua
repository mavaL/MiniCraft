eRace_Terran = 0
eRace_Zerg = 1

--------------------------------
---Terran global blackboard
---------------------------------
function GlobalBBUpdate_Terran()
	local numUnit = world:GetPlayerUnitNum(eRace_Terran)
	world:SetGlobalBBParam_Int(eRace_Terran, "UnitNumber", numUnit)
end


--------------------------------
---Zerg global blackboard
---------------------------------
function GlobalBBUpdate_Zerg()
	local numUnit = world:GetPlayerUnitNum(eRace_Zerg)
	world:SetGlobalBBParam_Int(eRace_Zerg, "UnitNumber", numUnit)
end


