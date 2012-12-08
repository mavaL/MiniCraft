-------Unit状态机

----------------------
--常量定义
----------------------
eUnitState_Idle		=	0
eUnitState_Move		=	1
eUnitState_Attack	=	2
eUnitState_Collect	=	3
eUnitState_Return	=	4
eUnitState_Build	=	5

eCommandType_Move	=	0
eCommandType_Attack	=	1
eCommandType_Harvest =	2
eCommandType_Build	=	3

resX, resY, resZ	=	0, 0, 0
collectResX, collectResY, collectResZ	=	-1.25, 0, 9.64
returnResX, returnResY, returnResZ	= 0, 0, 16

---------------------
---Idle State
---------------------
function IdleState_Enter()
	Unit0:PlayAnimation("IdleTop", "");
end

function IdleState_Update(dt)
end

function IdleState_Exit()
	Unit0:StopAnimation();
end

---------------------
---Move State
---------------------

function MoveState_Enter()
	if not Unit0:FindPath(Unit0:GetDestPosition()) then
		--寻路失败,转入空闲状态
		Unit0:SetState(eUnitState_Idle);
	end

	--打开跑动画
	Unit0:PlayAnimation("RunTop", "RunBase");
end

function MoveState_Update(dt)
	--寻路结束,到达目的地
	if Unit0:UpdatePathFinding(dt) then
		cmdType = Unit0:GetCurCommandType();
		if cmdType == eCommandType_Move then
			--普通移动结束,转入休闲状态
			Unit0:SetState(eUnitState_Idle);
		elseif cmdType == eCommandType_Harvest then
			--转入采集状态
			Unit0:SetState(eUnitState_Collect);
		end
	end
end

function MoveState_Exit()
	Unit0:StopAnimation();
end

---------------------
---CollectRes State
---------------------
local pastTime = 0;
--采集时间
local collectTime = 3;

function CollectResState_Enter()
	Unit0:PlayAnimation("SliceHorizontal", "");
	Unit0:Lookat(0, 0, -1.8);
	pastTime = 0;
end

function CollectResState_Update(dt)
	pastTime = pastTime + dt;
	--转入返还资源状态
	if pastTime >= collectTime then
		Unit0:SetDestPosition(returnResX, returnResY, returnResZ);
		Unit0:SetState(eUnitState_Return);
	end
end

function CollectResState_Exit()
	Unit0:StopAnimation();
end

---------------------
---ReturnRes State
---------------------

function ReturnResState_Enter()
	Unit0:PlayAnimation("", "RunBase");
	if Unit0:FindPath(returnResX, returnResY, returnResZ) then
		--背负一个资源物体..
		Unit0:AttachRes("knot.mesh", 0, 5, 0, 0.02);
	end
end

function ReturnResState_Update(dt)
	if Unit0:UpdatePathFinding(dt) then
		--继续去采集资源
		Unit0:SetDestPosition(collectResX, collectResY, collectResZ);
		Unit0:SetState(eUnitState_Move);
	end
end

function ReturnResState_Exit()
	Unit0:StopAnimation();
	Unit0:DetachRes();
end