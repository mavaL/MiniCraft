function BBUpdate_Zergling(unitID)
	obj = UnitTable[unitID]
	curTarget =  obj:GetAttackTargetID()
	
	if curTarget == -1 then
		curTarget = obj:FindNearestEnemy()
		if curTarget ~= -1 then
			obj:SetBlackboardParamBool("bFindEnemy", true)
		else
			obj:SetBlackboardParamBool("bFindEnemy", false)
		end
	end
	
	obj:SetBlackboardParamInt("AttackTargetID", curTarget)
	obj:SetAttackTargetID(curTarget)
end



