/*
 * GM.cpp
 *
 *  Created on: Mar 25, 2013
 *      Author: Vibhav Gogate
 *				The University of Texas at Dallas
 *				All rights reserved
 */
typedef long double ldouble;
#include "GM.h"
#include "prims.h"
#include <cmath>
#include <fstream>
#include <cstdlib>
#include <set>
#include <map>
#include<string>
#include <conio.h>
#include <vector>
#include <sstream>
using namespace std;
//typedef boost::adjacency_list<boost::listS, boost::vecS, boost::undirectedS> adjGraph;

 
//typedef boost::property<boost::edge_weight_t, float> EdgeWeightProperty;
// 
//typedef boost::adjacency_list<boost::listS, boost::vecS, boost::directedS, boost::no_property,EdgeWeightProperty > adjGraph;
//



void GM::readUAIGM(char* uaifilename) {
	ifstream infile(uaifilename);
	if(!infile.good()){
		cerr<<"Error opening GM file\n";
		exit(-1);
	}
	int num_variables;
	string tmp_string;
	infile >> tmp_string;
	if (tmp_string.compare("BAYES") == 0) {
		infile >> num_variables;
		// Read domains
		variables = vector<Variable*>(num_variables);
		for (int i = 0; i < num_variables; i++) {
			variables[i] = new Variable();
			variables[i]->id=i;
			infile>>variables[i]->d;
		}
		int num_functions;
		infile >> num_functions;
		vector<vector<Variable*> > parents(num_functions);
		vector<int> func_order(num_functions);
		for (int i = 0; i < num_functions; i++) {
			// Read parents of variables
			int num_parents;
			infile >> num_parents;
			num_parents--;
			vector<Variable*> curr_parents(num_parents);
			for (int j = 0; j < num_parents; j++) {
				int temp;
				infile >> temp;
				curr_parents[j] = variables[temp];
			}
			int var;
			infile >> var;
			parents[var] = curr_parents;
			func_order[i]=var;
		}
		functions = vector<Function*>(num_functions);
		for (int i = 0; i < num_functions; i++) {

			int var=func_order[i];
			int num_probabilities;
			infile>>num_probabilities;
			functions[var] = new Function();
			functions[var]->id=i;
			vector<Variable*> unsorted_variables;
			functions[var]->variables=parents[var];
			functions[var]->parents = parents[var];
			functions[var]->variables.push_back(variables[var]);
			unsorted_variables=functions[var]->variables;
			functions[var]->unsorted_variables= unsorted_variables;
			sort(functions[var]->variables.begin(), functions[var]->variables.end(),less_than_comparator_variable);
			functions[var]->table = vector < ldouble > (num_probabilities);
			for (int j = 0; j < num_probabilities; j++) {
						Variable::setAddress(unsorted_variables, j);
				infile>>functions[var]->table[Variable::getAddress(functions[var]->variables)];
			}
		}
	} else  {
		infile >> num_variables;
		// Read domains
		variables = vector<Variable*>(num_variables);
		for (int i = 0; i < num_variables; i++) {
			variables[i] = new Variable();
			variables[i]->id = i;
			infile >> variables[i]->d;
		}
		int num_functions;
		infile >> num_functions;
		vector < vector<Variable*> > scope(num_functions);
		for (int i = 0; i < num_functions; i++) {
			// Read parents of variables
			int num_vars_in_func;
			infile >> num_vars_in_func;
			scope[i] = vector<Variable*>(num_vars_in_func);
			for (int j = 0; j < num_vars_in_func; j++) {
				int temp;
				infile >> temp;
				scope[i][j] = variables[temp];
			}
		}
		functions = vector<Function*>(num_functions);
		for (int i = 0; i < num_functions; i++) {
			int num_probabilities;
			infile >> num_probabilities;
			functions[i] = new Function();
			functions[i]->id=i;
			functions[i]->variables=scope[i];
			sort(functions[i]->variables.begin(), functions[i]->variables.end(),less_than_comparator_variable);
			functions[i]->table = vector < ldouble > (num_probabilities);
			for (int j = 0; j < num_probabilities; j++) {
				Variable::setAddress(scope[i], j);
				infile>>functions[i]->table[Variable::getAddress(functions[i]->variables)];
			}
		}
	}
	infile.close();
}

void GM::readUAIEvidence(char* evidfilename)
{
	ifstream infile(evidfilename);
	if (!infile.good()) {
		infile.close();
		return;
	}
	int num_evidence;
	infile>>num_evidence;
	for(int i=0;i<num_evidence;i++){
		int var,value;
		infile>>var;infile>>value;
		variables[var]->setEvidence(value);
	}
	infile.close();
}

void GM::readDataSet(char* datafilename)
{
	ifstream infile(datafilename);
	/*adjGraph g;
	adjGraph::vertex_descriptor vd[100];
	EdgeWeightProperty edgeWeight[10000];
	typedef std::pair < int, int >E;*/
	if(!infile.good())
	{
		infile.close();
		return;
	}
	int num_variables;
	infile>>num_variables;
	numVars = num_variables;
	/*for(int i=0;i<num_variables;i++)
		vd[i] = g.add_vertex();*/
	int num_data;
	infile>>num_data;
	dataSet.resize(num_data);
	for(int i=0;i<num_data;i++)
	{
		dataSet[i].resize(num_variables);
		for(int j=0;j<num_variables;j++)
		{
			int val;
			infile>>dataSet[i][j];
		}
	}
	//typedef boost::property_map<adjGraph, boost::edge_weight_t>::type EdgeWeightMap;
	//EdgeWeightMap edgeWeightMap = get(boost::edge_weight, g);
	//std::vector < boost::graph_traits< adjGraph >::vertex_descriptor > p(num_variables);
	//boost::prim_minimum_spanning_tree(g,&p[0]);
	//  for (std::size_t i = 0; i != p.size(); ++i)
 //   if (p[i] != i)
 //     std::cout << "parent[" << i << "] = " << p[i] << std::endl;
 //   else
 //     std::cout << "parent[" << i << "] = no parent" << std::endl;
}

void GM::writeUAIGM(char* filename, GM learnedModel)
{
	ofstream output (filename);
	if (output.is_open())
	{
		output << "BAYES\n";
		output  << learnedModel.variables.size() << "\n";
		for(int i = 0; i < learnedModel.variables.size(); i++)
		{
			output << learnedModel.variables[i]->d << " ";
		}
		output << "\n";
		output << learnedModel.functions.size() << "\n";
		for(int i = 0; i < learnedModel.functions.size(); i++) 
		{
			output << learnedModel.functions[i]->variables.size() << " ";
			for(int j = 0; j < learnedModel.functions[i]->variables.size(); j++) 
			{
				output << learnedModel.functions[i]->variables[j]->id << " ";
			}
			output << "\n";
		}
		for(int i = 0; i < learnedModel.functions.size(); i++) {
			output << "\n";
			output << learnedModel.functions[i]->table.size() << "\n";
			for(int j = 0; j < learnedModel.functions[i]->table.size() / 2; j++) {
				output << learnedModel.functions[i]->table[j] << " " << learnedModel.functions[i]->table[j+1] << "\n";
			}
		}
		output.close();
	}
	else cout << "Unable to open file";
}

int GM:: getNumOccurrences(vector<Variable*> functionVariables)
{
	int num_occurrences = 0;
	for(vector<vector<char> >::iterator iterData=dataSet.begin();iterData!=dataSet.end();iterData++)
	{	
		bool flag = true;
		for(vector<Variable*>::iterator iterFV = functionVariables.begin();iterFV !=functionVariables.end();iterFV ++)
		{
				vector<char> data = *iterData;
				if(data[(*iterFV)->id]-48!=(*iterFV)->getValue())
				{
					flag=false;
					break;
				}
		}	
		if(flag)
			num_occurrences++;
	}
	return num_occurrences;
}

ldouble GM::getLogLikelihood()
{
	ldouble logLikelihood=0;
	//LogDouble* logLikelihood = new LogDouble;
	for(int i=0;i<dataSet.size();i++)
	{
		ldouble likelihood =1;
			for(int k=0;k<functions.size();k++)
			{
				//cout<<"Function: "+std::to_string(k)<<endl;
				for(int l=0;l<functions[k]->variables.size();l++)
				{
					//cout<<"Variable: "+std::to_string(l)<<endl;
					//cout<<"Value: "+std::to_string(functions[k]->variables[l]->id)<<endl;
					functions[k]->variables[l]->setValue(dataSet[i][functions[k]->variables[l]->id]-48);					
				}
				//LogDouble *tableValue = new LogDouble(functions[k]->table[Variable::getAddress(functions[k]->variables)]);
				//*logLikelihood  = logLikelihood ->add(*tableValue);
				likelihood *= functions[k]->table[Variable::getAddress(functions[k]->variables)];			
			}
			if(likelihood!=0)
			logLikelihood += log(likelihood)/log(2);
	}
	return logLikelihood; 	
}
	
ldouble GM::getLogLikelihoodDifference(GM learnedModel)
{
	ldouble logLikelihoodDiff=0;
	//LogDouble* logLikelihood = new LogDouble;
	for(int i=0;i<dataSet.size();i++)
	{
		ldouble originalLikelihood =1;
			for(int k=0;k<functions.size();k++)
			{
				//cout<<"Function: "+std::to_string(k)<<endl;
				for(int l=0;l<functions[k]->variables.size();l++)
				{
					//cout<<"Variable: "+std::to_string(l)<<endl;
					//cout<<"Value: "+std::to_string(functions[k]->variables[l]->id)<<endl;
					functions[k]->variables[l]->setValue(dataSet[i][functions[k]->variables[l]->id]-48);					
				}
				//LogDouble *tableValue = new LogDouble(functions[k]->table[Variable::getAddress(functions[k]->variables)]);
				//*logLikelihood  = logLikelihood ->add(*tableValue);
				originalLikelihood *= functions[k]->table[Variable::getAddress(functions[k]->variables)];			
			}
		ldouble learnedLikelihood =1;
			for(int k=0;k<learnedModel.functions.size();k++)
			{
				//cout<<"Function: "+std::to_string(k)<<endl;
				for(int l=0;l<learnedModel.functions[k]->variables.size();l++)
				{
					//cout<<"Variable: "+std::to_string(l)<<endl;
					//cout<<"Value: "+std::to_string(functions[k]->variables[l]->id)<<endl;
					learnedModel.functions[k]->variables[l]->setValue(dataSet[i][learnedModel.functions[k]->variables[l]->id]-48);					
				}
				//LogDouble *tableValue = new LogDouble(functions[k]->table[Variable::getAddress(functions[k]->variables)]);
				//*logLikelihood  = logLikelihood ->add(*tableValue);
				learnedLikelihood *= learnedModel.functions[k]->table[Variable::getAddress(learnedModel.functions[k]->variables)];			
			}
			logLikelihoodDiff += abs(ldouble((originalLikelihood==0?0:log(originalLikelihood)/log(2)) - (learnedLikelihood==0?0:log(learnedLikelihood)/log(2))));
	}
	return logLikelihoodDiff; 	
}

ldouble GM::BE(vector<int> &order)
{
	ldouble pe=1;
	vector<vector<Function*> > buckets (order.size());

	vector<int> var_in_pos(variables.size());
	for(int i=0;i<order.size();i++)
		var_in_pos[order[i]]=i;
	// First put the functions in the proper buckets
	for(int i=0;i<functions.size();i++)
	{
		int pos=variables.size();
		// Boundary case
		// If all variables in a function are evidence variables, simply multiple the entry with pe
		Function* function=functions[i];
		bool all_assigned=true;
		for(int j=0;j<function->variables.size();j++)
		{
			if(function->variables[j]->isEvidence()) continue;
			if(var_in_pos[function->variables[j]->id] < pos)
				pos=var_in_pos[function->variables[j]->id];
			all_assigned=false;
		}
		if(all_assigned) {
			pe*=function->table[Variable::getAddress(function->variables)];
		}
		else{
			assert(pos!=(int)order.size());
			buckets[pos].push_back(function);
		}
	}
	//cout<<"Now processing buckets\n";
	//Process buckets
	for(int i=0;i<buckets.size();i++)
	{
		if(buckets[i].empty())
			continue;

		vector<Variable*> bucket_variables_;
		for(int j=0;j<buckets[i].size();j++)
		{
			do_set_union(bucket_variables_,buckets[i][j]->variables,bucket_variables_,less_than_comparator_variable);
		}
		vector<Variable*> bucket_variables;
		for(int j=0;j<bucket_variables_.size();j++)
			if(!bucket_variables_[j]->isEvidence()) bucket_variables.push_back(bucket_variables_[j]);
		vector<Variable*> bucket_variable;
		bucket_variable.push_back(variables[order[i]]);
		vector<Variable*> marg_variables;
		do_set_difference(bucket_variables,bucket_variable,marg_variables,less_than_comparator_variable);

		Function* function= new Function();
		Function::multiplyAndMarginalize(marg_variables,buckets[i],*function,false);
		if(function->variables.empty())
		{
			assert((int)function->table.size()==1);
			pe*=function->table[0];
			if (function->id <0){delete(function);}
			continue;
		}
		//Put the function in the appropriate bucket
		int pos=order.size();
		for(int j=0;j<function->variables.size();j++)
		{
			if(var_in_pos[function->variables[j]->id] < pos)
				pos=var_in_pos[function->variables[j]->id];
		}
		assert(pos!=(int)order.size());
		assert(pos > i);
		buckets[pos].push_back(function);
		for(int j=0;j<buckets[i].size();j++)
		{
			if (buckets[i][j]!=NULL && buckets[i][j]->id <0){
				delete(buckets[i][j]);
			}
		}
		buckets[i].clear();
	}
	for(int i=0;i<buckets.size();i++){
		for(int j=0;j<buckets[i].size();j++){
			if (buckets[i][j]!=NULL && buckets[i][j]->id <0){
				delete(buckets[i][j]);
			}
		}
	}
	buckets.clear();
	return pe;
}

void GM::getMinDegreeOrdering(vector<int>& order, vector<set<int> >& clusters)
{

	// number of non-evidence variables
	int nne=0;
	vector<bool> processed(variables.size());
	for(int i=0;i<variables.size();i++){
		if(variables[i]->isEvidence()) {
			processed[i]=true;
		}
		else{
			nne++;
		}
	}
	order=vector<int>(nne);
	clusters=vector<set<int> >(nne);
	vector<set<int> > graph(variables.size());
	for(int i=0;i<functions.size();i++)
	{
		// Ignore the evidence variables
		for(int j=0;j<functions[i]->variables.size();j++)
		{
			int a=functions[i]->variables[j]->id;
			if(variables[a]->isEvidence()) continue;
			for(int k=j+1;k<functions[i]->variables.size();k++)
			{
				int b=functions[i]->variables[k]->id;
				if(variables[b]->isEvidence()) continue;
				graph[a].insert(b);
				graph[b].insert(a);
			}
		}
	}
	int max_cluster_size=0;
	for(int i=0;i<nne;i++)
	{
		//Find the node with the minimum number of nodes
		int min=variables.size();
		for(int j=0;j<graph.size();j++){
			if (processed[j]) continue;
			if (min > graph[j].size()){
				order[i]=j;
				min=graph[j].size();
			}
		}
		// Connect the neighbors of order[i] to each other
		int var=order[i];
		processed[var]=true;
		for(set<int>::iterator a=graph[var].begin();a!=graph[var].end();a++){
			for(set<int>::iterator b=graph[var].begin();b!=graph[var].end();b++){
				if (*a==*b) continue;
				graph[*a].insert(*b);
				graph[*b].insert(*a);
			}
		}
		clusters[i]=graph[var];
		if(clusters[i].size()>max_cluster_size) max_cluster_size=clusters[i].size();
		// Remove var from the graph
		for(set<int>::iterator a=graph[var].begin();a!=graph[var].end();a++){
			graph[*a].erase(var);
		}
		graph[var].clear();
	}
	//cout<<"Max cluster size ="<<max_cluster_size<<endl;
}

ldouble GM::fodParam(char* uaiFileName,char* testFileName,char* modelFileName,bool readUai,GM& model)
{
	GM fodModel;
	if(readUai)
		fodModel.readUAIGM(uaiFileName);
	else
		fodModel= model;
	for (int i = 0; i < functions.size(); i++) 
	{
		int var=functions[i]->id;
		int num_probabilities;
		/*vector<Variable*> unsorted_variables;
		unsorted_variables=fodModel.functions[var]->unsorted_variables;*/
			//sort(functions[var]->variables.begin(), functions[var]->variables.end(),less_than_comparator_variable);
			//functions[var]->table = vector < ldouble > (num_probabilities);
		for (int j = 0; j < fodModel.functions[var]->table.size(); j++)
		{
			Variable::setAddress(fodModel.functions[var]->variables, j);
			int num_X = getNumOccurrences(fodModel.functions[var]->variables);
			int num_ParX = getNumOccurrences(fodModel.functions[var]->parents);
			fodModel.functions[var]->table[Variable::getAddress(fodModel.functions[var]->variables)]= (ldouble(num_X+1))/ldouble(num_ParX+2);
		}
	/*	for (int j = 0; j < functions[var]->table.size(); j++) {
			Variable::setAddress(unsorted_variables, j);
			ldouble sum = functions[var]->table[Variable::getAddress(functions[var]->variables)];
			int index =	Variable::getAddress(functions[var]->variables);
			while(index<functions[var]->table.size())
			{
				sum += functions[var]->table[index];
				index+=2;
			}
		}*/
	}
	readDataSet(testFileName);
	writeUAIGM(modelFileName,fodModel);
	return getLogLikelihoodDifference(fodModel);
}

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems)
{
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) 
	{
        elems.push_back(item);
    }
    return elems;
}


std::vector<std::string> split(const std::string &s, char delim) 
{
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}

ldouble GM::podParam(char* uaiFileName,char* testFileName,char* modelFileName)
{
	/*for (int i = 0; i < functions.size(); i++)
	{
		int var=functions[i]->id;
		int num_probabilities;*/
		//vector<Variable*> unsorted_variables;
		//unsorted_variables=functions[var]->unsorted_variables;
		//	//sort(functions[var]->variables.begin(), functions[var]->variables.end(),less_than_comparator_variable);
		//	//functions[var]->table = vector < ldouble > (num_probabilities);
		//for (int j = 0; j < functions[var]->table.size(); j++) {
		//		Variable::setAddress(unsorted_variables, j);
		//		/*int num_X = getNumOccurrences(functions[var]->variables);
		//		int num_ParX = getNumOccurrences(functions[var]->parents);*/
		//		functions[var]->table[Variable::getAddress(functions[var]->variables)]= (ldouble(1)/ldouble(functions[var]->table.size()));
		//	}
		//}
	GM learnedModel;
	learnedModel.readUAIGM(uaiFileName);
	
	//string strTrain = trainFileName;
	/*vector<string> tokens= split(trainFileName,'/');
	string fileName = tokens[tokens.size()-1];*/
	/*replace(strTrain.begin(),strTrain.end(),'-f-','-p-');
	readDataSet(trainFileName);*/
	vector<vector<ldouble> > xuTable(learnedModel.functions.size());
	for (int i = 0; i < learnedModel.functions.size(); i++)
	{
		xuTable[i].resize(learnedModel.functions[i]->table.size());
		//assuming uniform distribution initially
		for(int j = 0; j < learnedModel.functions[i]->table.size(); j++)
			learnedModel.functions[i]->table[j] = 0.5;
	}
	for(int i=0;i<20;i++)
	{	
		for (int p = 0; p < learnedModel.functions.size(); p++)
			for(int q = 0; q < learnedModel.functions[p]->table.size(); q++)
				xuTable[p][q] = 0;
		for(int j=0;j<dataSet.size();j++)
		{
			//xuTable[j].resize(learnedModel.functions[j]->table.size());
			//vector<Variable*> incompleteVariables;
			for(int k=0;k<dataSet[j].size();k++)
			{
				if(dataSet[j][k]!='?')
				{
					learnedModel.variables[k]->setEvidence(dataSet[j][k]-48);
				}
			}
			vector<int> order;
			vector<set<int> > clusters;
			learnedModel.getMinDegreeOrdering(order,clusters);
			ldouble Pe = learnedModel.BE(order);
			for(int v=0;v<learnedModel.variables.size();v++)
				learnedModel.variables[v]->resetEvidence();
			for (int fn = 0; fn <learnedModel.functions.size(); fn++) 
			{
				int var=learnedModel.functions[fn]->id;
				int num_probabilities;
				/*vector<Variable*> unsorted_variables;
				unsorted_variables=learnedModel.functions[var]->unsorted_variables;*/
			//sort(functions[var]->variables.begin(), functions[var]->variables.end(),less_than_comparator_variable);
			//functions[var]->table = vector < ldouble > (num_probabilities);
				map<int,int> varVal;
				for (int f = 0; f < learnedModel.functions[var]->table.size(); f++) 
				{
					Variable::setAddress(learnedModel.functions[var]->variables, f);
					ldouble jPe=1;		
					for(int fv=0;fv<learnedModel.functions[var]->variables.size();fv++)
					{	
						if(dataSet[j][learnedModel.functions[var]->variables[fv]->id]!='?' && (dataSet[j][learnedModel.functions[var]->variables[fv]->id]-48)!=learnedModel.functions[var]->variables[fv]->getValue())
						{
							jPe=0;
							break;
						}
						varVal[learnedModel.functions[var]->variables[fv]->id] = learnedModel.functions[var]->variables[fv]->getValue(); 
					}
					if(jPe==0)
						continue;
					for(int ev=0;ev<learnedModel.variables.size();ev++)
					{
						if(dataSet[j][ev]!='?')
							varVal[ev] = dataSet[j][ev]-48;
					}
					for(map<int,int>::iterator iter=varVal.begin();iter!=varVal.end();iter++)
						learnedModel.variables[iter->first]->setEvidence(iter->second);
					vector<int> order;
					vector<set<int> > clusters;
					learnedModel.getMinDegreeOrdering(order,clusters);
					jPe = learnedModel.BE(order);
					for(map<int,int>::iterator iter=varVal.begin();iter!=varVal.end();iter++)
						learnedModel.variables[iter->first]->resetEvidence();
					//xuTable[j][Variable::getAddress(functions[var]->variables)] += Pe==0?0:jPe/Pe;
					xuTable[fn][f] += Pe==0?0:jPe/Pe;
				}
			}
		}
		for (int x = 0; x < learnedModel.functions.size(); x++) 
		{
			//calculating the probability
			int block = learnedModel.functions[x]->table.size();
			for (int l = 0; l < learnedModel.functions[x]->variables.size(); l++) 
			{
				block /= learnedModel.functions[x]->variables[l]->d;
				if (learnedModel.functions[x]->variables[l]->id == x)
				break;
			}
			ldouble total = 0;
			for (int y = 0; y < learnedModel.functions[x]->table.size(); y++) 
			{
				if (y % block== 0 &&(y / block) % 2 == 1)
				{
					y += block- 1;
					continue;
				}
				total = xuTable[x][y] + xuTable[x][y + block];
				learnedModel.functions[x]->table[y] = (total == 0 ? 0.5 : (xuTable[x][y] / total));
				learnedModel.functions[x]->table[y + block] = (total== 0 ? 0.5 : (xuTable[x][y + block] / total));
				/*if((j+1)%block==0 && !((j+1)/block%2==1))
					j+=1+block;*/
			}
		}			
		//	getIncompleteData(dataSet[j]);			
		//	for(int v=0;v<GM::variables.size();v++)
		//	{
		//	for(int w=0;w<incomplete.size();w++)
		//	{
		//		if(incomplete.at(w) == GM::variables.at(v)->id)
		//		{
		//			incompleteVariables.push_back(variables.at(v));
		//		}
		//	}
		//	}
		//	incompleteVariablesSet.push_back(incompleteVariables);
		//	int domainSize = Variable::getDomainSize(incompleteVariables);
		//	vector<ldouble> probabilities;
		//	for(int d=0;d<domainSize;d++)
		//	{
		//		ldouble value = 1;
		//		Variable::setAddress(incompleteVariables,d);
		//		for(int f=0;f<functions.size();f++)
		//		{
		//			int func_entry = Variable::getAddress(functions[f]->variables);
		//			ldouble func = (functions[f]->table[func_entry]);
		//			value *= func;
		//		}
		//		ldouble jPe = value;
		//		probabilities.push_back(jPe/Pe);
		//	}
		//	probabilityTable.push_back(probabilities);
		//	//LogDouble *sum = new LogDouble(0);
		//	ldouble sum= 0;
		//	for(int d=0;d<domainSize;d++)
		//	{
		//		sum+= probabilities[d];
		//	}
		//	for(int d=0;d<domainSize;d++)
		//	{
		//		probabilityTable[j][d] = probabilityTable[j][d]/sum;
		//	}
		//	}
		//int totalDomainSize = Variable::getDomainSize(variables);
		//fullTable.resize(totalDomainSize);
		//for(int i=0;i<totalDomainSize;i++)
		//{
		//	Variable::setAddress(variables,i);
		//	for(int j=0;j<dataSet.size();j++)
		//	{
		//		fullTable[Variable::getAddress(variables)] +=  probabilityTable[j][Variable::getAddress(incompleteVariablesSet[j])];
		//	}
		//	fullTable[Variable::getAddress(variables)] /= dataSet.size();
		//}
		//for (int i = 0; i < functions.size(); i++) {
		//int var=functions[i]->id;
		//int num_probabilities;
		//vector<Variable*> unsorted_variables;
		//unsorted_variables=functions[var]->unsorted_variables;
		//	//sort(functions[var]->variables.begin(), functions[var]->variables.end(),less_than_comparator_variable);
		//	//functions[var]->table = vector < ldouble > (num_probabilities);
		//for (int j = 0; j < functions[var]->table.size(); j++) {
		//		Variable::setAddress(unsorted_variables, j);
		//		ldouble prob_X = getOccurrencesProb(functions[var]->unsorted_variables);
		//		ldouble prob_ParX = getOccurrencesProb(functions[var]->parents);
		//		functions[var]->table[Variable::getAddress(functions[var]->variables)]= prob_X/prob_ParX;
		//	}
	}
	/*for(int i=0;i<learnedModel.functions.size();i++)
	{
		for(int j=0;j<functions[i]->table.size();j++)
		{
			cout<<"Function: "<<i<<"Table row: "<<j<<learnedModel.functions[i]->table[j]<<endl;
		}
	}*/
	writeUAIGM(modelFileName,learnedModel);
	readDataSet(testFileName);
	return getLogLikelihoodDifference(learnedModel);
}

typedef struct Edge {
	int v1, v2;
	double weight;
	Edge(int v1, int v2, double weight) 
	{
		this->v1 = v1;
		this->v2 = v2;
		this->weight = weight;
	}
} edge;

typedef struct Vertex {
	int parent;
	bool selected;
	Vertex() 
	{
		parent = -1;
		this->selected = false;
	}
} vertex;

ldouble GM::chowLiu(char* uaiFileName,char* testFileName,char* modelFileName)
{
	//prims* p = new prims();
	//p->graph.resize(numVars);
	//vector< vector<bool> > processed(numVars, vector<bool>(numVars));
	//for(int i=0;i<numVars;i++)
	//{
	//	//vector<bool> processed;
	//	for(int j=0;j<numVars;j++)
	//	{
	//		processed[i][j] = false;
	//		//processed[j][i] = false;
	//	}
	//}
	//for(int i=0;i<numVars;i++)
	//{
	//	p->graph[i].resize(numVars);
	//	for(int j=0;j<numVars;j++)
	//	{
	//		if(p->graph[j].size()<numVars)
	//			p->graph[j].resize(numVars);
	//		if(i!=j && !processed[i][j] && !processed[j][i])
	//		{
	//			ldouble miscore = (getMIScore(i,j));
	//			//Graph::edge_property_type ep;
	//			//ep.m_value = edgeWeight[k];
	//			//boost::add_edge(i,j,edgeWeight[k++],g);
	//			p->graph[i][j] = miscore;
	//			p->graph[j][i] = miscore;
	//			processed[i][j] = true;
	//			processed[j][i] = true;
	//		}
	//	}
	//}
	//p->primsAlgo(numVars);
	vector<edge> edges;
	vector<vertex> vertices(variables.size());
	int current = 0;
	while (true) 
	{
		vertices[current].selected = true;
		int undiscovered = 0;
		for (int i = 0; i < vertices.size(); i++) 
		{
			if (vertices[i].selected == false) 
			{
				double weight = 0;
				int x, u;
				for (int j = 0; j < 4; j++) 
				{
					u = j % 2;
					x = j / 2;
					double xuProb = 0, xProb = 0, uProb = 0;
					for (int d = 0; d < dataSet.size(); d++) 
					{
						if (dataSet[d][current] == x)
							xProb++;
						if (dataSet[d][i] == u)
							uProb++;
						if (dataSet[d][current] == x && dataSet[d][i] == u)
							xuProb ++;
					}
					xuProb /= dataSet.size(), xProb /= dataSet.size(), uProb /=
							dataSet.size();
					weight +=
						(xuProb == 0 ?0 :xuProb  * log(xuProb  / (xProb * uProb)));
				}
				edges.push_back(edge(current, i, weight));
				undiscovered++;
			}
		}
		if (undiscovered == 0)
			break;
			//get the maximum of all weights
		double maxWeight = 0;
		int maxIndex = 0;
		for (unsigned int i = 0; i < edges.size(); i++) {
			if (edges[i].weight >= maxWeight
				&& !(vertices[edges[i].v1].selected
							&& vertices[edges[i].v2].selected)) {
				maxWeight = edges[i].weight;
				maxIndex = i;
			}
		}
		if (vertices[edges[maxIndex].v1].selected) {
			vertices[edges[maxIndex].v2].parent = edges[maxIndex].v1;
			current = edges[maxIndex].v2;
		}
		else
		{
			vertices[edges[maxIndex].v1].parent = edges[maxIndex].v2;
			current = edges[maxIndex].v1;
		}
	}
	GM clModel;
	clModel.variables = variables;
	clModel.functions = vector<Function*>(clModel.variables.size());
	for(int i=0;i<clModel.variables.size();i++)
	{
		clModel.functions[i] = new Function();
		clModel.functions[i]->id = i;
		if(vertices[i].parent != -1)
		{
			clModel.functions[i]->variables.push_back(clModel.variables[vertices[i].parent]);
			clModel.functions[i]->parents.push_back(clModel.variables[vertices[i].parent]);
		}
		clModel.functions[i]->variables.push_back(clModel.variables[i]);
		clModel.functions[i]->table = vector < ldouble > (clModel.functions[i]->variables.size()*2);
	}
	return fodParam(uaiFileName,testFileName,modelFileName,false,clModel);
}



