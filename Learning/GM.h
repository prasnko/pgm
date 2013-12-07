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

struct GM
{
	int numVars;
	vector<Variable*> variables;
	vector<Function*> functions;
	vector<vector<char> > dataSet;
	void readUAIGM(char* uaifilename);
	void readUAIEvidence(char* evidfilename);
	void readDataSet(char* datafilename);
	void writeUAIGM(char* fileName,GM model);
	void getMinDegreeOrdering(vector<int>& order, vector<set<int> >& clusters);
	ldouble BE(vector<int>& order);	
	int getNumOccurrences(vector<Variable*> functionVariables);
	ldouble getLogLikelihood();
	ldouble getLogLikelihoodDifference(GM learnedModel);
	ldouble fodParam(char* uaiFileName,char* testFileName,char* modelFileName,bool readUai,GM& model);
	ldouble podParam(char* uaiFileName,char* testFileName,char* modelFileName);	
	ldouble chowLiu(char* uaiFileName,char* testFileName,char* modelFileName);
};

#endif /* GM_H_ */
