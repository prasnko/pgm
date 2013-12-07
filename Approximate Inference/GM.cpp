/*
 * GM.cpp
 *
 *  Created on: Mar 25, 2013
 *      Author: Vibhav Gogate
 *				The University of Texas at Dallas
 *				All rights reserved
 */
#include "GM.h"
#include "LogDouble.h"
#include <fstream>
#include <cstdlib>
#include <set>
#include <string>
#include <hash_map>
#include <math.h>

using namespace std;
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
			functions[var]->variables.push_back(variables[var]);
			unsorted_variables=functions[var]->variables;
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
				infile >> functions[i]->table[Variable::getAddress(functions[i]->variables)];
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

LogDouble GM::BE(vector<int> &order)
{
	ldouble pe=1;
	LogDouble* lgPe = new LogDouble(pe);
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
			LogDouble* lgTable = new LogDouble(function->table[Variable::getAddress(function->variables)]);
			*lgPe = (lgPe->multiply(*lgTable));
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
			LogDouble* lgTab0 = new LogDouble(function->table[0]);
			*lgPe = (lgPe->multiply(*lgTab0));
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
	return *lgPe;
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
	cout<<"Max cluster size ="<<max_cluster_size<<endl;
}

void GM:: computeWCutset(vector<set<int> >& clusters, vector<int>& order, int w,vector<int>& wcv)
{	
	for(int i=0;i<clusters.size();i++)
		{
			set<int> vars; 
			if(clusters[i].size()<=0)
			 continue;
			while(clusters[i].size()>w+1)
			{	
			 for(int j=order.size()-1;j>=0;j--)
			 {
			if(clusters[i].size()>w+1)
			{
			  for(set<int>::iterator siter = clusters[i].begin();siter!=clusters[i].end();siter++)
			  {
				//set<int>::iterator temp = siter++;
					if(*siter== order[j])
					 {
						//if(std::find(wcv.begin(), wcv.end(), *siter) != wcv.end()) 
						wcv.push_back(*siter);
						//else
						//continue;
						for(int k=0;k<clusters.size();k++)
						{
							if(clusters[k].size()>0)
							{
								if(std::find(clusters[k].begin(), clusters[k].end(), order[j]) != clusters[k].end()) 
								{
								clusters[k].erase(order[j]);
								}
						    }
					    }
						break;
					 }
					}
				 }
				 else
				 break;
				}						
		}
	}
}

 LogDouble GM:: getUniformDistribution(const vector<Variable*>& variables,vector<int> &wcv)
	{
		vector<Variable*> removedVariables;
		for(vector<int>::iterator iter= wcv.begin();iter!=wcv.end();iter++)
		{
			for (int i = 0; i < variables.size(); i++)
			{
				if(*iter==variables[i]->id)
				removedVariables.push_back(variables[i]);
			}
		}
		long double totalDomainSize = Variable::getDomainSize(removedVariables);
		long double u = (1/totalDomainSize);
		LogDouble* lgU = new LogDouble(u);
		return *lgU;
	}

 void GM:: createEvidMap(vector<int> evidVariables)
 {	 
	 int num_evidence = evidVariables.size();
	 int d,var;
	 for (int i = 0; i < num_evidence; i++) {
		 for(int j=0;j<GM::variables.size();j++)
		 {
			 if(GM::variables.at(j)->id==evidVariables.at(i))
			{				 
				d = GM::variables.at(j)->d;
				var = GM::variables.at(j)->id;
				vector<pair<int,int> > evidList;
				evidList.clear();
				for(int j=0;j<d;j++)
				{				
					int val = j;			
					evidMap = make_pair(var,val);	
					evidList.push_back(evidMap);
				}
				varEvidList.push_back(evidList);
			 }
		 }		
		}
 }

 double GM:: setEvidence(vector<int> evidVariables)
 {
	 int val;
	 int index; 
	 pair<int,int> evidence;
	 vector<pair<int,int> > evidences;
	 vector<pair<int,int> > currentEvidence;
	 for(int i=0;i<evidences.size();i++)	 
	 {
		 evidences = varEvidList.at(i);
		if(evidences.size()>0)
		{
			index = (rand())%(evidences.size());		 
		}
		else
			index = 0;
		evidence = evidences.at(index);
		currentEvidence.push_back(evidence);
		GM::variables.at(evidence.first)->setValue(evidence.second);
		evidences.clear();
		//evidList.erase(evidList.begin()+index);
	 }
	 if(!idxCount.count(currentEvidence))
		idxCount[currentEvidence]++;
	 else
		idxCount[currentEvidence]=1;
	 return idxCount[currentEvidence];
 }

 LogDouble GM:: wCutsetSampling(vector<int>& order, vector<int>& wcv, int num_samples)
{
	long double totalDomainSize; 
	LogDouble* Z = new LogDouble(0);
	LogDouble* u = new LogDouble(0); 
	LogDouble val;
	*u = getUniformDistribution(GM::variables,wcv);
	vector<Variable*> evidVariables;
	for(int v=0;v<GM::variables.size();v++)
	{
		for(int w=0;w<wcv.size();w++)
		{
			if(wcv.at(w) == GM::variables.at(v)->id)
			{
				evidVariables.push_back(variables.at(v));
			}
		}
	}
	createEvidMap(wcv);
	totalDomainSize = Variable::getDomainSize(evidVariables);
	long double domainSizeCount = totalDomainSize;
	int n = 1;
	for(int i=0;i<num_samples;i++)
	{
			double update = GM::setEvidence(wcv);
			if(wcv.size()==0)
			val = BE(order);
			else
			val = ((BE(order)).divide((*u)));
			*Z = Z->add(val);
	/*		domainSizeCount--;
			n++;*/
	}
	LogDouble* N = new LogDouble(num_samples);
	return (Z->divide((*N)));
}



  LogDouble GM:: adaptiveWCutsetSampling(vector<int>& order, vector<int>& wcv, int num_samples)
{
	int totalDomainSize; 
	LogDouble* Z = new LogDouble(0);
	LogDouble* u = new LogDouble(1); 
	double updt;
	LogDouble val;
	LogDouble lgUpdt;
	*u = getUniformDistribution(GM::variables,wcv);
	vector<Variable*> evidVariables;
	for(int v=0;v<GM::variables.size();v++)
	{
		for(int w=0;w<wcv.size();w++)
		{
			if(wcv.at(w) == GM::variables.at(v)->id)
			{
				evidVariables.push_back(variables.at(v));
			}
		}
	}
	createEvidMap(wcv);
	totalDomainSize = Variable::getDomainSize(evidVariables);
	int domainSizeCount = totalDomainSize;
	int n = 1;
	for(int i=1;i<=num_samples;i++)
	{
		updt = GM::setEvidence(wcv);		
		if(i>=100 && i%100==0)		
		{
			LogDouble* lgUpdt = new LogDouble((updt/(double)(i)));
			*u = u->add(*lgUpdt);
		}
		if(wcv.size()==0)
		val = BE(order);
		else
		val = (BE(order)).divide(*u);
		*Z = Z->add(val);
		/*domainSizeCount--;
		n++;*/
	}
	LogDouble* N = new LogDouble(num_samples);
	return (Z->divide(*N));
}