/********************************************************************
	created:	30:3:2013   10:21
	filename	prac.cpp
	author:		maval

	purpose:	行为树条件解析lexer的单元测试
*********************************************************************/
#if 0

#include <iostream>
#include <vector>
#include <unordered_map>
#include <assert.h>
#include <map>
#include <cstdlib>
#include <Ogre.h>
#include "KratosPrerequisites.h"
#include "BlackBoard.h"
#include "BehaviorTreeCondition.h"

using namespace std;

void Print(const std::string& condition, bool result)
{
	char* strResult = result ? "true" : "false";

	cout << "The condition is: " << condition.c_str() << endl;
	cout << "The result is: " << strResult << endl;
	cout << endl;
}

int main()
{
	aiBlackBoard bb;
	bb.DefineParam("True1", "true", aiBlackBoard::eVarType_Bool);
	bb.DefineParam("True2", "true", aiBlackBoard::eVarType_Bool);
	bb.DefineParam("False1", "false", aiBlackBoard::eVarType_Bool);
	bb.DefineParam("False2", "false", aiBlackBoard::eVarType_Bool);
	bb.DefineParam("Int1", "1", aiBlackBoard::eVarType_Int);
	bb.DefineParam("Int2", "2", aiBlackBoard::eVarType_Int);
	bb.DefineParam("Float1", "1.0", aiBlackBoard::eVarType_Float);
	bb.DefineParam("Float2", "2", aiBlackBoard::eVarType_Float);

	///test1,constant
	{
		const std::string condition("true");
		aiBehaviorConditon test(condition, bb);
		bool ret = test.Evaluate(bb);
		Print(condition, ret);
	}

	///test2,variable
	{
		const std::string condition("True1");
		aiBehaviorConditon test(condition, bb);
		bool ret = test.Evaluate(bb);
		Print(condition, ret);
	}

	///test3,unary logical
	{
		const std::string condition("!True1");
		aiBehaviorConditon test(condition, bb);
		bool ret = test.Evaluate(bb);
		Print(condition, ret);
	}

	///test4, binary logical
	{
		std::string condition("True1 or False1");
		aiBehaviorConditon test1(condition, bb);
		bool ret = test1.Evaluate(bb);
		Print(condition, ret);

		condition = "False1 == true";
		aiBehaviorConditon test2(condition, bb);
		ret = test2.Evaluate(bb);
		Print(condition, ret);

		condition = "True1 == False1";
		aiBehaviorConditon test3(condition, bb);
		ret = test3.Evaluate(bb);
		Print(condition, ret);
	}

	///test4, multi
	{
		std::string condition("(True1 or False2) and (False1 and True2)");
		aiBehaviorConditon test1(condition, bb);
		bool ret = test1.Evaluate(bb);
		Print(condition, ret);

		bb.SetParam("False1", "true");
		cout << "False1 set to true" << endl;
		aiBehaviorConditon test2(condition, bb);
		ret = test2.Evaluate(bb);
		Print(condition, ret);
	}

	///test5, 算术比较..
	{
		std::string condition("Int1 == 2 and Float1 == 1.0");
		aiBehaviorConditon test1(condition, bb);
		bool ret = test1.Evaluate(bb);
		Print(condition, ret);

		condition = "Int2 > Int1 and Float1 !=100 and Int2 >= -3 and Float2 > Float1";
		aiBehaviorConditon test2(condition, bb);
		ret = test2.Evaluate(bb);
		Print(condition, ret);

		bb.SetParam("Float2", "-5.3");
		cout << "Float2 set to -5.3" << endl;
		ret = test2.Evaluate(bb);
		Print(condition, ret);
	}

 	system("pause");

	return 0;
}

#endif