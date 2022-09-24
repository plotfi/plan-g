#ifndef __DEMANDSTRUCTURE_H__
#define __DEMANDSTRUCTURE_H__ 1
#include <iostream>
#include <list>
#include <string>
#include <unordered_map>
#include <map>
#include <unordered_set>
#include <utility>
#include <vector>
#include <functional>

namespace Scheme {

namespace Demands {

/*
	Defines a type for possible liveness values
	Functions:
		operator+(): union of liveness
*/
class LivenessState;
bool operator<(const LivenessState& s1, const LivenessState& s2);
bool operator==(const LivenessState&, const LivenessState&);
bool operator!=(const LivenessState&, const LivenessState&);
class LivenessState
{

public:
	std::string idString;
	int latticeNo;
	static std::unordered_map<std::string, std::unordered_map<std::string, LivenessState> > unionRules;
	static std::unordered_map<std::string, LivenessState> catZeroRules;
	static std::unordered_map<std::string, LivenessState> catOneRules;
	static std::unordered_map<std::string, LivenessState> stripZeroRules;
	static std::unordered_map<std::string, LivenessState> stripOneRules;

	LivenessState();
	LivenessState(const std::string& s, const int&);
	LivenessState operator+(const LivenessState&);	//Union
	LivenessState catZero();						//Concatenate Zero
	LivenessState catOne();							//Concatenate One
	LivenessState stripZero();						//Strip's Zero from front
	LivenessState stripOne();						//Strip's One from front
	friend std::ostream& operator<<(std::ostream& out, const LivenessState& l);
	friend bool operator==(const LivenessState&, const LivenessState&);
	friend bool operator!=(const LivenessState&, const LivenessState&);

};

extern LivenessState PHI;
extern LivenessState EPSILON;
extern LivenessState ZERO;
extern LivenessState ONE;
extern LivenessState ZEROALL;
extern LivenessState ONEALL;
extern LivenessState ONE_STAR;
extern LivenessState ALL;

//Operator overloading
bool operator==(const LivenessState l1, const LivenessState l2);

//Trivial hasher for maps/sets of LivenessState
class LivenessStateHasher
{
public:
	std::size_t operator()(const LivenessState& obj) const { return std::hash<int>()(obj.latticeNo); }
};

class LivenessTable;
bool operator==(const LivenessTable&, const LivenessTable&);
bool operator!=(const LivenessTable&, const LivenessTable&);
//Defines a type for liveness table of any variable
typedef std::map< std::string, LivenessTable> LivenessInformation;
typedef std::unordered_map<std::string, LivenessTable> LivenessInformation2;
bool operator==(const LivenessInformation&, const LivenessInformation&);
LivenessInformation mapLiveness(const LivenessTable&, const LivenessInformation&);
class LivenessTable
{
public:
	std::map<LivenessState, LivenessState> table;
	std::string varName;
	LivenessTable();
	LivenessTable(std::string, bool self);
	LivenessTable operator+(const LivenessState&);
	LivenessState& operator[](LivenessState k);
	const LivenessState& operator[](const LivenessState& k) const;
	void catZero();
	void catOne();
	void stripZero();
	void stripOne();
	void doUnion(const LivenessTable&);


	friend std::ostream& operator<<(std::ostream& out, const LivenessTable& t);
	friend bool operator==(const LivenessTable&, const LivenessTable&);
	friend bool operator!=(const LivenessTable&, const LivenessTable&);
	friend LivenessInformation mapLiveness(const LivenessTable& lt, const LivenessInformation& li);

};


typedef std::map< std::string, LivenessInformation> ProgramLiveness;
typedef std::unordered_map<std::string, std::vector<std::string> > ArgumentsOrder;
typedef std::unordered_map<std::string, LivenessInformation2> ProgramLiveness2;

//Printing Demands
std::ostream& operator<<(std::ostream& out, const LivenessState& l);
std::ostream& operator<<(std::ostream& out, const LivenessTable& t);
std::ostream& operator<<(std::ostream& out, const LivenessInformation& t);
std::ostream& operator<<(std::ostream& out, const ProgramLiveness& t);
bool printLivenessToFile(const ProgramLiveness&, const std::string&);

void doUnion(LivenessInformation &, const LivenessInformation &);
}
}
extern Scheme::Demands::ProgramLiveness progLiveness;
extern Scheme::Demands::ProgramLiveness functionCallDemands;
extern Scheme::Demands::ProgramLiveness2 progLiveness2;


#endif

