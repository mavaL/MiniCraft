#include "BehaviorTreeCondition.h"
#include <OgreMath.h>
#include <OgreStringConverter.h>
#include "BehaviorTree.h"
#include "BlackBoard.h"
#include "BehaviorTreeTemplateManager.h"

namespace Kratos
{
	class SimpleLexer
	{
	public:
		SimpleLexer(const char* buffer)
			: m_buffer(buffer)
			, m_obuffer(buffer)
		{
		}

		int peek(std::string* ident = 0)
		{
			const char* obuffer = m_buffer;
			std::string oident(m_ident);
			int tok = operator()();
			m_buffer = obuffer;
			if (ident)
				ident->swap(m_ident);
			m_ident.swap(oident);

			return tok;
		}

		const char* ident() const
		{
			return m_ident.c_str();
		}

		const char* buf() const
		{
			return m_obuffer;
		}


		int operator()()
		{
			while (char ch = *m_buffer++)
			{
				char next = *m_buffer;

				switch (ch)
				{
				case '(':
				case ')':
					return (int)ch;
				case '=':
					if (next == '=')
					{
						++m_buffer;
						return '==';
					}
					return ch;
				case '!':
					if (next == '=')
					{
						++m_buffer;
						return '!=';
					}
					return ch;
				case '>':
					if (next == '=')
					{
						++m_buffer;
						return '>=';
					}
					return ch;
				case '<':
					if (next == '=')
					{
						++m_buffer;
						return '<=';
					}
					return ch;
				default:
					{
						if (isalpha(ch) || isalnum(ch) || (ch == '_') || (ch == '-'))
						{
							m_ident.clear();
							m_ident.push_back(ch);
							while ((ch = *m_buffer) && (isalnum(ch) || (ch == '.') || (ch == '_')))
							{
								++m_buffer;
								m_ident.push_back(ch);
							}

							//constant number
							if(isdigit(m_ident[0]) || (m_ident[0] == '-'))
								return 'num';
							// keywords
							else if (!stricmp(m_ident.c_str(), "or"))
								return 'or';
							else if (!stricmp(m_ident.c_str(), "and"))
								return 'and';
							else if (!stricmp(m_ident.c_str(), "xor"))
								return 'xor';
							else if (!stricmp(m_ident.c_str(), "true"))
								return 'true';
							else if (!stricmp(m_ident.c_str(), "false"))
								return 'fals';
							return 'var';
						}
						else if (isspace(ch))
							continue;

						return ch;
					}
				}
			}

			return 0;
		}

	private:
		const char* m_buffer;
		const char* m_obuffer;
		std::string m_ident;
	};


	aiBehaviorConditon::aiBehaviorConditon()
		: m_rootID(-1)
		,m_pRaceGlobalBB(nullptr)
	{
	}

	aiBehaviorConditon::aiBehaviorConditon(const STRING& condition, aiBlackBoard& bb)
	{
		m_pRaceGlobalBB = aiBehaviorTreeTemplateManager::GetSingleton().GetGlobalBB(bb.GetParent()->GetRace());
		m_rootID = Parse(condition, bb);

		if (m_rootID >= 0)
			Optimise();
	}

	int aiBehaviorConditon::AddOp(const ConditionOp& op)
	{
		m_conditionOps.push_back(op);
		return (int)m_conditionOps.size() - 1;
	}

	int aiBehaviorConditon::ParseLogical(SimpleLexer& lex, int tok, aiBlackBoard& bb)
	{
		int leftID = ParseCompOp(lex, tok, bb);
		if (leftID == -1)
			return -1;

		while ((tok = lex.peek()) && (tok == 'and') || (tok == 'or') || (tok == 'xor'))
		{
			lex();

			int rightID = ParseCompOp(lex, lex(), bb);
			if (rightID == -1)
				return -1;

			switch (tok)
			{
			case 'or':
				leftID = AddOp(ConditionOp(ConditionOp::Or, leftID, rightID));
				break;
			case 'and':
				leftID = AddOp(ConditionOp(ConditionOp::And, leftID, rightID));
				break;
			case 'xor':
				leftID = AddOp(ConditionOp(ConditionOp::Xor, leftID, rightID));
				break;
			}
		}

		return leftID;
	}

	int aiBehaviorConditon::ParseCompOp(SimpleLexer& lex, int tok, aiBlackBoard& bb)
	{
		bool bVarLeft;
		int leftID = ParseUnary(lex, tok, bb, bVarLeft);
		if (leftID == -1)
			return -1;

		tok = lex.peek();
		if ((tok == '==')	|| 
			(tok == '!=')	|| 
			(tok == '>')	||
			(tok == '>=')	||
			(tok == '<')	||
			(tok == '<=')	)
		{
			lex();

			bool bVarRight;
			int rightID = ParseUnary(lex, lex(), bb, bVarRight);
			if (rightID == -1)
				return -1;

			assert(bVarLeft == bVarRight);

			switch (tok)
			{
			case '==':
				return AddOp(ConditionOp(bVarLeft?ConditionOp::EqualArithmetical:ConditionOp::EqualLogical, leftID, rightID));
			case '!=':
				return AddOp(ConditionOp(bVarLeft?ConditionOp::NotEqualArithmetical:ConditionOp::NotEqualLogical, leftID, rightID));
			case '>':
				return AddOp(ConditionOp(ConditionOp::Greater, leftID, rightID));
			case '>=':
				return AddOp(ConditionOp(ConditionOp::GreaterEqual, leftID, rightID));
			case '<':
				return AddOp(ConditionOp(ConditionOp::Lessthan, leftID, rightID));
			case '<=':
				return AddOp(ConditionOp(ConditionOp::LessthanEqual, leftID, rightID));
			}
		}

		return leftID;
	}

	int aiBehaviorConditon::ParseUnary(SimpleLexer& lex, int tok, aiBlackBoard& bb, bool& bVariable)
	{
		if (tok == '!')
		{
			int opID = ParseValue(lex, lex(), bb, bVariable);
			if (opID != -1)
				return AddOp(ConditionOp(ConditionOp::Not, opID, -1));
			return -1;
		}

		return ParseValue(lex, tok, bb, bVariable);
	}

	int aiBehaviorConditon::ParseValue(SimpleLexer& lex, int tok, aiBlackBoard& bb, bool& bVariable)
	{
		bVariable = true;
		if (tok == '(')
		{
			bVariable = false;
			int opID = ParseLogical(lex, lex(), bb);
			if ((opID == -1) || (lex() != ')'))
				return -1;

			return opID;
		}
		else if (tok == 'true')
			return AddOp(ConditionOp(ConditionOp::Variable, "true", aiBlackBoard::eVarType_Bool, false));
		else if (tok == 'fals')
			return AddOp(ConditionOp(ConditionOp::Variable, "false", aiBlackBoard::eVarType_Bool, false));
		else if (tok == 'var')
		{
			//是否是全局黑板参数
			const STRING varName = lex.ident();
			if(bb.IsGlobalParam(varName))
			{
				const aiBlackBoard::SValue& val = m_pRaceGlobalBB->GetParam(varName);
				return AddOp(ConditionOp(ConditionOp::Variable, varName, val.m_type, true));
			}
			else
			{
				const aiBlackBoard::SValue& val = bb.GetParam(varName);
				return AddOp(ConditionOp(ConditionOp::Variable, varName, val.m_type, false));
			}
		}
		else if (tok == 'num')
		{
			//将条件语句里的常量记录入黑板
			std::string str = lex.ident();
			if (str.find('.') != std::string::npos)
			{
				//实数
				float f = Ogre::StringConverter::parseReal(str, 10000);
				assert(f != 10000 && "Invalid number!");
				bb.DefineParam(str, str, aiBlackBoard::eVarType_Float);
				return AddOp(ConditionOp(ConditionOp::Variable, str, aiBlackBoard::eVarType_Float, false));
			}
			else
			{
				//整数
				int i = Ogre::StringConverter::parseInt(str, 10000);
				assert(i != 10000 && "Invalid number!");
				bb.DefineParam(str, str, aiBlackBoard::eVarType_Int);
				return AddOp(ConditionOp(ConditionOp::Variable, str, aiBlackBoard::eVarType_Int, false));
			}
		}

		return -1;
	}

	int aiBehaviorConditon::Parse(const STRING& condition, aiBlackBoard& bb)
	{
		SimpleLexer lex = SimpleLexer(condition.c_str());
		return ParseLogical(lex, lex(), bb);
	}

	void aiBehaviorConditon::Optimise()
	{
		// TODO(M醨cio)
	}

	bool aiBehaviorConditon::EvaluateOp(aiBlackBoard& bb, const ConditionOp& op) const
	{
		switch (op.opType)
		{
		case ConditionOp::Or:
			return EvaluateOp(bb, m_conditionOps[op.operandLeft]) 
				|| EvaluateOp(bb, m_conditionOps[op.operandRight]);
		case ConditionOp::And:
			return EvaluateOp(bb, m_conditionOps[op.operandLeft])
				&& EvaluateOp(bb, m_conditionOps[op.operandRight]);
		case ConditionOp::Xor:
			return EvaluateOp(bb, m_conditionOps[op.operandLeft])
				^ EvaluateOp(bb, m_conditionOps[op.operandRight]);
		case ConditionOp::Not:
			return !EvaluateOp(bb, m_conditionOps[op.operandLeft]);
		case ConditionOp::EqualLogical:
			return EvaluateOp(bb, m_conditionOps[op.operandLeft])
				== EvaluateOp(bb, m_conditionOps[op.operandRight]);
		case ConditionOp::NotEqualLogical:
			return EvaluateOp(bb, m_conditionOps[op.operandLeft])
				!= EvaluateOp(bb, m_conditionOps[op.operandRight]);

			///TODO: 实现非常ugly和低效,有什么好的方法呢?
		case ConditionOp::EqualArithmetical:
			return CompEqual(bb, op);
		case ConditionOp::NotEqualArithmetical:
			return !CompEqual(bb, op);
		case ConditionOp::Greater:
			return CompGreater(bb, op);
		case ConditionOp::GreaterEqual:
			return CompGreaterEqual(bb, op);
		case ConditionOp::Lessthan:
			return !CompGreaterEqual(bb, op);
		case ConditionOp::LessthanEqual:
			return !CompGreater(bb, op);
		case ConditionOp::Variable:
			{
				assert(op.varType == aiBlackBoard::eVarType_Bool && !op.varGlobal);
				auto& val = bb.GetParam(op.varName);
				return Ogre::StringConverter::parseBool(val.m_value);
			}
		default: assert(0);
		}

		return false;
	}

	bool aiBehaviorConditon::Evaluate(aiBlackBoard& bb) const
	{
		return EvaluateOp(bb, m_conditionOps[m_rootID]);
	}

	bool aiBehaviorConditon::Valid() const
	{
		return m_rootID >= 0;
	}

	void aiBehaviorConditon::GetCompParam( const ConditionOp& op, const aiBlackBoard::SValue*& lhs, 
		const aiBlackBoard::SValue*& rhs, aiBlackBoard& bb ) const
	{
		//lhs
		if(m_conditionOps[op.operandLeft].varGlobal)
			lhs = &m_pRaceGlobalBB->GetParam(m_conditionOps[op.operandLeft].varName);
		else
			lhs = &bb.GetParam(m_conditionOps[op.operandLeft].varName);
		//rhs
		if(m_conditionOps[op.operandRight].varGlobal)
			rhs = &m_pRaceGlobalBB->GetParam(m_conditionOps[op.operandRight].varName);
		else
			rhs = &bb.GetParam(m_conditionOps[op.operandRight].varName);
	}

	bool aiBehaviorConditon::CompEqual( aiBlackBoard& bb, const ConditionOp& op ) const
	{
		const aiBlackBoard::SValue* lhs = nullptr;
		const aiBlackBoard::SValue* rhs = nullptr;
		GetCompParam(op, lhs, rhs, bb);

		//以左操作数类型为准,假定应该是一样的
		switch (m_conditionOps[op.operandLeft].varType)
		{
		case aiBlackBoard::eVarType_Int: return Ogre::StringConverter::parseInt(lhs->m_value) == Ogre::StringConverter::parseInt(rhs->m_value);
		case aiBlackBoard::eVarType_Float: return Ogre::Math::RealEqual(Ogre::StringConverter::parseReal(lhs->m_value), Ogre::StringConverter::parseReal(rhs->m_value));
		case aiBlackBoard::eVarType_Bool: return Ogre::StringConverter::parseBool(lhs->m_value) == Ogre::StringConverter::parseBool(rhs->m_value);
		default: assert(0);
		}
		return false;
	}

	bool aiBehaviorConditon::CompGreater( aiBlackBoard& bb, const ConditionOp& op ) const
	{
		const aiBlackBoard::SValue* lhs = nullptr;
		const aiBlackBoard::SValue* rhs = nullptr;
		GetCompParam(op, lhs, rhs, bb);

		//以左操作数类型为准,假定应该是一样的
		switch (m_conditionOps[op.operandLeft].varType)
		{
		case aiBlackBoard::eVarType_Int: return Ogre::StringConverter::parseInt(lhs->m_value) > Ogre::StringConverter::parseInt(rhs->m_value);
		case aiBlackBoard::eVarType_Float: return Ogre::StringConverter::parseReal(lhs->m_value) > Ogre::StringConverter::parseReal(rhs->m_value);
		default: assert(0);
		}
		return false;
	}

	bool aiBehaviorConditon::CompGreaterEqual( aiBlackBoard& bb, const ConditionOp& op ) const
	{
		const aiBlackBoard::SValue* lhs = nullptr;
		const aiBlackBoard::SValue* rhs = nullptr;
		GetCompParam(op, lhs, rhs, bb);

		//以左操作数类型为准,假定应该是一样的
		switch (m_conditionOps[op.operandLeft].varType)
		{
		case aiBlackBoard::eVarType_Int: return Ogre::StringConverter::parseInt(lhs->m_value) >= Ogre::StringConverter::parseInt(rhs->m_value);
		case aiBlackBoard::eVarType_Float: return Ogre::StringConverter::parseReal(lhs->m_value) >= Ogre::StringConverter::parseReal(rhs->m_value);
		default: assert(0);
		}
		return false;
	}
}

