--与代码相对应
eHarvestStage_ToRes				=	0
eHarvestStage_NearRes			=	1
eHarvestStage_Gather			=	2
eHarvestStage_Return			=	3
eHarvestStage_NearBase			=	4
eHarvestStage_None				=	5

--------------------------------
---Scv blackboard
---------------------------------
function BBUpdate_Scv(unitID)
	obj = UnitTable[unitID]
	curStage = obj:GetHarvestStage()

	isGathering = false
	isCarryRes = false
	isNearBase = false
	isNearRes = false

	if curStage == eHarvestStage_NearRes then
		isNearRes = true
	elseif curStage == eHarvestStage_Gather then
		isGathering = true
	elseif curStage == eHarvestStage_Return then
		isCarryRes = true
	elseif curStage == eHarvestStage_NearBase then
		isCarryRes = true
		isNearBase = true
	end

	obj:SetBlackboardParamBool("IsCarryingRes", isCarryRes)
	obj:SetBlackboardParamBool("IsGathering", isGathering)
	obj:SetBlackboardParamBool("IsNearRes", isNearRes)
	obj:SetBlackboardParamBool("IsNearBase", isNearBase)

	fTime = obj:GetGatheringTime()
	obj:SetBlackboardParamFloat("fGatheringTime", fTime)
end


