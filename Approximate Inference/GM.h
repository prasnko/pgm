/*
 * GM.h
 *
 *  Created on: Mar 25, 2013
 *      Author: Vibhav Gogate
 *				The University of Texas at Dallas
 *				All rights reserved
 */

#ifndef GM_H_
#define GM_H_

#include <iostream>
#include <fstream>
#include <set>
using namespace std;
#include "Variable.h"
#include "Function.h"
#include "LogDouble.h"
#include <map>

struct GM
{
	vector<Variable*> variables;
	vector<Function*> functions;
	pair<int,int> evidMap;
	map<vector<pair<int,int> >,int> idxCount;
	vector<vector<pair<int,int> > > varEvidList;
	void readUAIGM(char* uaifilename);
	void readUAIEvidence(char* evidfilename);
	void createEvidMap(vector<int> evidVariables);
	double setEvidence(vector<int> evidVariables);
	LogDouble getUniformDistribution(const vector<Variable*>& variables,vector<int> &wcv);
	LogDouble BE(vector<int>& order);
	void getMinDegreeOrdering(vector<int>& order, vector<set<int> >& clusters);
	void computeWCutset(vector<set<int> >& clusters, vector<int>& order, int w,vector<int>& wcv);
	LogDouble wCutsetSampling(vector<int>& order, vector<int>& wcv, int num_samples);
	LogDouble adaptiveWCutsetSampling(vector<int>& order, vector<int>& wcv, int num_samples);
};

#endif /* GM_H_ */
