#ifndef __SCHEME_AST_H__
#define __SCHEME_AST_H__ 1

#include "Output.h"
#include <vector>
#include <map>
#include <iostream>
#include "DemandStructure.h"
#include <functional>
#include <cstring>
#include <utility>
using namespace std;
namespace Scheme {

namespace AST {

struct StrCompare : public std::binary_function<const char*, const char*, bool> {
public:
    bool operator() (const char* str1, const char* str2) const
    { return std::strcmp(str1, str2) < 0; }
};
class ExprNode;


typedef unordered_set<string> EdgeSet;
extern unordered_map<string, EdgeSet> revCallGraph;

enum exprType
{
	nilExpr,
	constIntExpr,
	constBoolExpr,
	constStringExpr,
	varExpr,
	letExpr,
	funcDefine,
	funcApplicationExpr,
	consExpr,
	carExpr,
	cdrExpr,
	nullqExpr,
	pairqExpr,
	addExpr,
	subExpr,
	mulExpr,
	divExpr,
	gtExpr,
	ltExpr,
	modExpr,
	eqExpr,
	ifExpr,
	defaultType
};

enum resType {heap, stringType, intType, boolType, errorType, funcType};

//class resultValue
//{
//public:
//	resultValue(resType t, int val);
//
//	resultValue(resType t, bool val);
//
//	resultValue(resType t, std::string *val);
//
//	resultValue(resType t, void* val);
//
//	resultValue(const resultValue& r);
//
//	resType type;
//	boost::variant<void*, std::string*, int, bool> res;
//};

class resultValue
{
public:
	resultValue(resType t, int val);

	resultValue(resType t, bool val);

	resultValue(resType t, std::string *val);

	resultValue(resType t, void* val);

	resultValue(const resultValue& r);

	resType type;
	union
	{
		void* addrVal;
		int intVal;
		bool boolVal;
		std::string* stringVal;
	}val;
};


class Node {
public:
	virtual ~Node();

	virtual void doLabel(bool = true) = 0;
	virtual std::string getLabel() const;

	virtual Node * clone() const = 0;
	virtual Node * getANF() const = 0;

	// Return type by reference??
	virtual Scheme::Demands::LivenessInformation
	transformDemand() const = 0;

	virtual std::ostream & print(std::ostream &, unsigned = 0, bool = true, bool = false,
			Scheme::output::output_t format = Scheme::output::PLAIN) const = 0;
	enum exprType type;
	const std::string node_name;
protected:
	std::string label;
	//LivenessInformation livenessInformation;

	Node(const std::string);

};




class ExprNode : public Node {
public:
	virtual ~ExprNode();

	/* We need to re-write this to tell the compiler that,
	 * the clone() and getANF() of any derived classes of
	 * ExprNode, is Exprnode * and not Node *.                */
	virtual ExprNode * clone() const = 0;
	virtual ExprNode * getANF() const = 0;
	virtual resultValue evaluate() = 0;
	// virtual LivenessInfo analyse(Liveness)
	// {
	// 	std::cout << "Method should be overriden in the derived class." <<std::endl;
	// 	exit(-1);
	// }
	//virtual void nextExpr() = 0;
	virtual bool isFunctionCallExpression()	{return false;}
	virtual bool isConsExpression() {return false;}
	virtual std::string getIDStr() const {return "";}

	// LivenessInformation& getLiveness()
	// {
	// 	return livehhnessInformation;
	// }

	

	virtual EdgeSet dependentFunctions(EdgeSet& s);

	
protected:
	ExprNode(const std::string name);
	//Liveness l;
};


class IdExprNode : public ExprNode {
public:
	virtual ~IdExprNode();
	IdExprNode(std::string *);

	virtual void doLabel(bool = true);

	virtual IdExprNode * clone() const;
	virtual ExprNode * getANF() const;

	std::string getIDStr() const;

	virtual Scheme::Demands::LivenessInformation
	transformDemand() const;

	virtual std::ostream & print(std::ostream &, unsigned = 0, bool = true, bool = false,
			Scheme::output::output_t format = Scheme::output::PLAIN) const;
	std::string getName() {return *pID;}
	virtual resultValue evaluate();
	//virtual LivenessInfo analyse(Liveness);
	//virtual void nextExpr();

	virtual std::string getLabel() const;

protected:
	std::string * pID;
};

class ReturnExprNode : public ExprNode {
public:
	virtual ~ReturnExprNode();
	ReturnExprNode(IdExprNode *);

	virtual void doLabel(bool = true);

	virtual ReturnExprNode * clone() const;
	virtual ReturnExprNode * getANF() const;

	virtual Scheme::Demands::LivenessInformation
	transformDemand() const;
	virtual resultValue evaluate();
	virtual std::ostream & print(std::ostream &, unsigned = 0, bool = true, bool = false,
			Scheme::output::output_t format = Scheme::output::PLAIN) const;
	//virtual LivenessInfo analyse(Liveness);
	//virtual void nextExpr();
	virtual std::string getLabel() const;

protected:
	IdExprNode * pID;

	friend ExprNode * pushDown(ExprNode *, ExprNode *);
};



class LetExprNode : public ExprNode {
public:
	virtual ~LetExprNode();
	LetExprNode(IdExprNode *, ExprNode *, ExprNode *);

	virtual void doLabel(bool = true);

	virtual LetExprNode * clone() const;
	virtual ExprNode * getANF() const;

	virtual LetExprNode * fillHoleWith(ExprNode * pSubExpr);

	virtual Scheme::Demands::LivenessInformation
	transformDemand() const;

	virtual std::ostream & print(std::ostream &, unsigned = 0, bool = true, bool = false,
			Scheme::output::output_t format = Scheme::output::PLAIN) const;
	std::string getVar();
	ExprNode* getVarExpr();
	ExprNode* getBody();
	virtual resultValue evaluate();
	//virtual LivenessInfo analyse(Liveness);
	//virtual void nextExpr();

	

	virtual EdgeSet dependentFunctions(EdgeSet& s);

	

protected:
	IdExprNode * pID;
	ExprNode * pExpr, * pBody;

	friend ExprNode * pushDown(ExprNode *, ExprNode *);
};


class IfExprNode : public ExprNode {
public:
	virtual ~IfExprNode();
	IfExprNode(ExprNode *, ExprNode *, ExprNode *);

	virtual void doLabel(bool = true);

	virtual IfExprNode * clone() const;
	virtual ExprNode * getANF() const;

	virtual IfExprNode * fillHoleWith(IdExprNode * pSubExpr);
	//
	virtual Scheme::Demands::LivenessInformation
	transformDemand() const;

	virtual std::ostream & print(std::ostream &, unsigned = 0, bool = true, bool = false,
			Scheme::output::output_t format = Scheme::output::PLAIN) const;
	ExprNode* getCond();
	ExprNode* getThenPart();
	ExprNode* getElsePart();
	virtual resultValue evaluate();
	//virtual LivenessInfo analyse(Liveness);
	//virtual void nextExpr();

	

	virtual EdgeSet dependentFunctions(EdgeSet& s);

	

protected:
	ExprNode * pCond, * pThen, * pElse;

	friend ExprNode * pushDown(ExprNode *, ExprNode *);
};


class ConstExprNode : public ExprNode {
public:
	virtual ~ConstExprNode();
	virtual LetExprNode * getANF() const;

	virtual void doLabel(bool = true);

	virtual Scheme::Demands::LivenessInformation
	transformDemand() const;

protected:
	ConstExprNode(const std::string);
};

class HoleConstExprNode : public ConstExprNode {
public:
	HoleConstExprNode();

	virtual HoleConstExprNode * clone() const;
	virtual LetExprNode * getANF() const;
	virtual resultValue evaluate();
	//virtual LivenessInfo analyse(Liveness);
	virtual std::ostream & print(std::ostream &, unsigned = 0, bool = true, bool = false,
			Scheme::output::output_t format = Scheme::output::PLAIN) const;
	//virtual void nextExpr();
};

class NilConstExprNode : public ConstExprNode {
public:
	NilConstExprNode();

	virtual NilConstExprNode * clone() const;

	virtual std::ostream & print(std::ostream &, unsigned = 0, bool = true, bool = false,
			Scheme::output::output_t format = Scheme::output::PLAIN) const;
	virtual resultValue evaluate();
	//virtual void nextExpr();
};

class IntConstExprNode : public ConstExprNode {
public:
	virtual ~IntConstExprNode();
	IntConstExprNode(int *);

	virtual IntConstExprNode * clone() const;

	virtual std::ostream & print(std::ostream &, unsigned = 0, bool = true, bool = false,
			Scheme::output::output_t format = Scheme::output::PLAIN) const;
	int getVal();
	virtual resultValue evaluate();
	//virtual LivenessInfo analyse(Liveness);
	//virtual void nextExpr();
protected:
	int * pIntVal;
};


class StrConstExprNode : public ConstExprNode {
public:
	virtual ~StrConstExprNode();
	StrConstExprNode(std::string *);

	virtual StrConstExprNode * clone() const;

	virtual std::ostream & print(std::ostream &, unsigned = 0, bool = true, bool = false,
			Scheme::output::output_t format = Scheme::output::PLAIN) const;
	string* getVal();
	virtual resultValue evaluate();
	//virtual LivenessInfo analyse(Liveness);
	//virtual void nextExpr();
protected:
	std::string * pStrVal;
};


class BoolConstExprNode : public ConstExprNode {
public:
	virtual ~BoolConstExprNode();
	BoolConstExprNode(bool *);

	virtual BoolConstExprNode * clone() const;

	virtual std::ostream & print(std::ostream &, unsigned = 0, bool = true, bool = false,
			Scheme::output::output_t format = Scheme::output::PLAIN) const;
	bool getVal();
	virtual resultValue evaluate();
	//virtual LivenessInfo analyse(Liveness);
	//virtual void nextExpr();
protected:
	bool * pBoolVal;
};


class PrimExprNode : public ExprNode {
public:
	virtual ~PrimExprNode();
	//virtual void nextExpr()=0;
protected:
	PrimExprNode(const std::string);
};


class UnaryPrimExprNode : public PrimExprNode {
public:
	virtual ~UnaryPrimExprNode();
	UnaryPrimExprNode(const std::string, ExprNode *);

	virtual void doLabel(bool = true);

	virtual UnaryPrimExprNode * clone() const;
	virtual ExprNode * getANF() const;

	virtual UnaryPrimExprNode * fillHoleWith(IdExprNode * pSubExpr);

	virtual Scheme::Demands::LivenessInformation
	transformDemand() const;

	virtual std::ostream & print(std::ostream &, unsigned = 0, bool = true, bool = false,
			Scheme::output::output_t format = Scheme::output::PLAIN) const;
	virtual resultValue evaluate();
	//virtual LivenessInfo analyse(Liveness);
	//virtual void nextExpr();

protected:
	ExprNode * pArg;

	resultValue evaluateCarExpr();
	resultValue evaluateCdrExpr();
	resultValue evaluateNullqExpr();
	resultValue evaluatePairqExpr();
	friend class LetExprNode;
	friend ExprNode * pushDown(ExprNode *, ExprNode *);
};


class BinaryPrimExprNode : public PrimExprNode {
public:
	virtual ~BinaryPrimExprNode();
	BinaryPrimExprNode(const std::string, ExprNode *, ExprNode *);

	virtual void doLabel(bool = true);

	virtual BinaryPrimExprNode * clone() const;
	virtual ExprNode * getANF() const;

	virtual BinaryPrimExprNode * fillHoleWith(IdExprNode * pSubExpr);

	virtual Scheme::Demands::LivenessInformation
	transformDemand() const;

	virtual std::ostream & print(std::ostream &, unsigned = 0, bool = true, bool = false,
			Scheme::output::output_t format = Scheme::output::PLAIN) const;
	virtual resultValue evaluate();
	///virtual LivenessInfo analyse(Liveness);
	//virtual void nextExpr();
	virtual bool isConsExpression() {return (node_name == "cons");}
protected:
	ExprNode * pArg1, * pArg2;

	resultValue evaluateCons();
	resultValue evaluateAdd();
	resultValue evaluateSub();
	resultValue evaluateMul();
	resultValue evaluateDiv();
	resultValue evaluateMod();
	resultValue evaluateLT();
	resultValue evaluateGT();
	resultValue evaluateEQ();

	friend class LetExprNode;
	friend ExprNode * pushDown(ExprNode *, ExprNode *);
};


class FuncExprNode : public ExprNode {
public:
	virtual ~FuncExprNode();
	FuncExprNode(IdExprNode *, std::list<ExprNode *> *);

	virtual void doLabel(bool = true);

	virtual FuncExprNode * clone() const;
	virtual ExprNode * getANF() const;

	virtual FuncExprNode * fillHoleWith(IdExprNode * pSubExpr);

	            virtual Scheme::Demands::LivenessInformation
	            transformDemand() const;

	virtual std::ostream & print(std::ostream &, unsigned = 0, bool = true, bool = false,
			Scheme::output::output_t format = Scheme::output::PLAIN) const;
	std::string getFunction();

	std::vector<ExprNode*> getArgs();
	virtual resultValue evaluate();
	//virtual LivenessInfo analyse(Liveness);
	////virtual void nextExpr();
	virtual bool isFunctionCallExpression()	{return true;}
	void setNextExpr(std::string);
	std::string getNextExpr();

	

	virtual EdgeSet dependentFunctions(EdgeSet& s);

	

protected:
	IdExprNode * pID;
	std::list<ExprNode *> * pListArgs;
	std::vector<ExprNode*> vargs;
	std::string nextExpr;
	friend class LetExprNode;
	friend ExprNode * pushDown(ExprNode *, ExprNode *);
};


class DefineNode : public Node {
public:
	virtual ~DefineNode();
	DefineNode(IdExprNode *, std::list<IdExprNode *> *, ExprNode *);

	virtual void doLabel(bool = true);

	virtual DefineNode * clone() const;
	virtual DefineNode * getANF() const;

	virtual std::string getFuncName() const;
	            virtual Scheme::Demands::LivenessInformation
	            transformDemand() const;
	 void init();
	virtual std::ostream & print(std::ostream &, unsigned = 0, bool = true, bool = false,
			Scheme::output::output_t format = Scheme::output::PLAIN) const;
	std::string getFunctionName() {return pID->getIDStr();}
	std::vector<std::string> getArgs();
	ExprNode* getFunctionBody() {return pExpr;}

	

	EdgeSet makeCallGraph();

	

protected:
	ExprNode * pExpr;
	IdExprNode * pID;
	std::list<IdExprNode *> * pListArgs;
	std::vector<std::string> vargs;
};

class ProgramNode : public Node {
public:
	virtual ~ProgramNode();
	ProgramNode(std::list<DefineNode *> *, ExprNode *);

	virtual void doLabel(bool = true);

	virtual ProgramNode * clone() const;
	virtual ProgramNode * getANF() const;

	virtual Scheme::Demands::LivenessInformation
            transformDemand() const;

	virtual std::ostream & print(std::ostream &, unsigned = 0, bool = true, bool = false,
			Scheme::output::output_t format = Scheme::output::PLAIN) const;
	resultValue evaluate();

	Node* getFunction(std::string);

	void doLivenessAnalysis();

	unordered_map<string, EdgeSet> makeRevCallGraph();


	//TODO : Add a function to process function definitions (Similar to the LE function in python)
protected:
	std::list<DefineNode *> * pListDefines;

	ExprNode * pExpr;
	std::map<std::string, Node*> funcmap;
};

ExprNode * pushDown(ExprNode *, ExprNode *);

void printRevCallGraph();

}
}




#endif
