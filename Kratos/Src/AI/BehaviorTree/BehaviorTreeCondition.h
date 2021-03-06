/********************************************************************
	created:	28:3:2013   22:05
	filename	BehaviorTreeCondition.h
	author:		maval

	purpose:	行为树条件节点的条件处理
*********************************************************************/

#ifndef BehaviorTreeCondition_h__
#define BehaviorTreeCondition_h__

#include "KratosPrerequisites.h"
#include "BlackBoard.h"

namespace Kratos
{
	class SimpleLexer;

	class aiBehaviorConditon
	{
	public:
		aiBehaviorConditon();
		aiBehaviorConditon(const STRING& condition, aiBlackBoard& bb);

		bool Evaluate(aiBlackBoard& bb) const;
		bool Valid() const;

	private:
		struct ConditionOp;
		int AddOp(const ConditionOp& op);
		int ParseLogical(SimpleLexer& lex, int tok, aiBlackBoard& bb);
		int ParseCompOp(SimpleLexer& lex, int tok, aiBlackBoard& bb);
		int ParseUnary(SimpleLexer& lex, int tok, aiBlackBoard& bb, bool& bVariable);
		int ParseValue(SimpleLexer& lex, int tok, aiBlackBoard& bb, bool& bVariable);
		int Parse(const STRING& condition, aiBlackBoard& bb);
		void Optimise();

		void GetCompParam(const ConditionOp& op, const aiBlackBoard::SValue*& lhs, const aiBlackBoard::SValue*& rhs, aiBlackBoard& bb) const;
		bool CompEqual(aiBlackBoard& bb, const ConditionOp& op) const;
		bool CompGreater(aiBlackBoard& bb, const ConditionOp& op) const;
		bool CompGreaterEqual(aiBlackBoard& bb, const ConditionOp& op) const;

		bool EvaluateOp(aiBlackBoard& bb, const struct ConditionOp& op) const;

		struct ConditionOp
		{
			enum Type
			{
				Not = 1,
				And,
				Or,
				Xor,
				EqualLogical,
				NotEqualLogical,
				EqualArithmetical,
				NotEqualArithmetical,
				Greater,
				GreaterEqual,
				Lessthan,
				LessthanEqual,
				Variable,
			};

			ConditionOp()
			{
			}

			ConditionOp(int type, int left, int right)
				: opType((Type)type)
				, operandLeft((Ogre::uint8)left)
				, operandRight((Ogre::uint8)right)
			{
			}

			ConditionOp(int type, const STRING& var, aiBlackBoard::eVarType vartype, bool bGlobal)
				: opType((Type)type)
				, varName(var)
				, varType(vartype)
				, varGlobal(bGlobal)
			{
			}

			STRING		varName;
			aiBlackBoard::eVarType	varType;
			bool		varGlobal;

			Ogre::uint8 opType;
			Ogre::uint8 operandLeft;
			Ogre::uint8 operandRight;
		};

		typedef std::vector<ConditionOp> ConditionOps;
		ConditionOps	m_conditionOps;
		int				m_rootID;
		aiBlackBoard*	m_pRaceGlobalBB;
	};
}



#endif // BehaviorTreeCondition_h__