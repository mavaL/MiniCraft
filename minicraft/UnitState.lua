-------Unit状态机

--[[
			Unit类导出函数
	void PlayAnimation(string, string)		param:top层动画名,base层动画名		ret:无
	void StopAnimation()					param:无			ret:无
	bool FindPath(float, float, float)		param:目的坐标	ret:成功返回true,否则false
	void SetState(int)							param:要切换的状态标志,见下面的eUnitState_xx	ret:无
	void SetDestPosition(float, float, float)	param:设置目标点	ret:无
	float,float,float GetDestPosition()		param:无			ret:获取当前目标点
	int GetCurCommandType()					param:无			ret:获取当前命令标志,见下面的eCommandType_xx
	bool UpdatePathFinding(float)			param:帧间隔时间	ret:已完成寻路,到达目标则返回true
	void Lookat(float, float, float)		param:lookat坐标点	ret:无
	void AttachRes(string, float, float, float, float)	param:mesh名,物体相对Unit节点的位置,物体的缩放系数	ret:无
	void DetachRes()						param:无			ret:无
--]]


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
function IdleState_Enter(id)
	UnitTable[id]:PlayAnimation("IdleTop", "");
end

function IdleState_Update(id,dt)
end

function IdleState_Exit(id)
	UnitTable[id]:StopAnimation();
end

---------------------
---Move State
---------------------

function MoveState_Enter(id)
	if not UnitTable[id]:FindPath(UnitTable[id]:GetDestPosition()) then
		--寻路失败,转入空闲状态
		UnitTable[id]:SetState(eUnitState_Idle);
	end

	--打开跑动画
	UnitTable[id]:PlayAnimation("RunTop", "RunBase");
end

function MoveState_Update(id,dt)
	--寻路结束,到达目的地
	if UnitTable[id]:UpdatePathFinding(dt) then
		cmdType = UnitTable[id]:GetCurCommandType();
		if cmdType == eCommandType_Move then
			--普通移动结束,转入休闲状态
			UnitTable[id]:SetState(eUnitState_Idle);
		elseif cmdType == eCommandType_Harvest then
			--转入采集状态
			UnitTable[id]:SetState(eUnitState_Collect);
		end
	end
end

function MoveState_Exit(id)
	UnitTable[id]:StopAnimation();
end

---------------------
---CollectRes State
---------------------
local pastTime = 0;
--采集时间
local collectTime = 3;

function CollectResState_Enter(id)
	UnitTable[id]:PlayAnimation("SliceHorizontal", "");
	UnitTable[id]:Lookat(0, 0, -1.8);
	pastTime = 0;
end

function CollectResState_Update(id,dt)
	--FIXME:pastTime移入对象成员属性,是不同个体独有的
	pastTime = pastTime + dt;
	--转入返还资源状态
	if pastTime >= collectTime then
		UnitTable[id]:SetDestPosition(returnResX, returnResY, returnResZ);
		UnitTable[id]:SetState(eUnitState_Return);
	end
end

function CollectResState_Exit(id)
	UnitTable[id]:StopAnimation();
end

---------------------
---ReturnRes State
---------------------

function ReturnResState_Enter()
	UnitTable[id]:PlayAnimation("", "RunBase");
	if UnitTable[id]:FindPath(returnResX, returnResY, returnResZ) then
		--背负一个资源物体..
		UnitTable[id]:AttachRes("knot.mesh", 0, 5, 0, 0.02);
	end
end

function ReturnResState_Update(dt)
	if UnitTable[id]:UpdatePathFinding(dt) then
		--继续去采集资源
		UnitTable[id]:SetDestPosition(collectResX, collectResY, collectResZ);
		UnitTable[id]:SetState(eUnitState_Move);
	end
end

function ReturnResState_Exit()
	UnitTable[id]:StopAnimation();
	UnitTable[id]:DetachRes();
end