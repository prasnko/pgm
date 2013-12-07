/*
 * Main.cpp
 *
 *  Created on: Mar 25, 2013
 *      Author: Vibhav Gogate
 *				The University of Texas at Dallas
 *				All rights reserved
 */




#include "GM.h"
#include <conio.h>
using namespace std;
#include <cstdlib>

int main(int argc, char* argv[])
{
	if (argc < 2){
		cerr<<"Usage: "<<argv[0]<<" program.exe <program-no> <input-uai-file> <training-data> <test-data> <output-uai-file>\n";
		exit(-1);
	}
	//ofstream outfile(argv[5]);
	GM gm;
	vector<int> order;
	vector<set<int> > clusters;
	gm.readUAIGM(argv[2]); //read the uai model
	gm.readDataSet(argv[4]); //read test file
	//ldouble originalLikelihood = gm.getLogLikelihood(); 
	//FOD	
	if(atoi(argv[1])==1)
	{
		gm.readDataSet(argv[3]); //read train file
		ldouble fodLogLikelihoodDiff = gm.fodParam(argv[2],argv[4],argv[5],true,gm); //calculate parameters on trainfile, read test file and calculate loglikelihood difference
		cout<<"FOD log likelihood(base 2) difference ="<<fodLogLikelihoodDiff<<endl;
	}
	//CHOW-LIU
	else if(atoi(argv[1])==3)
	{
		gm.readDataSet(argv[3]); //read train file
		ldouble clLogLikelihoodDiff = gm.chowLiu(argv[2],argv[4],argv[5]); //calculate parameters on trainfile, read test file and calculate loglikelihood difference
		cout<<"CHOW-LIU log likelihood(base 2) difference ="<<clLogLikelihoodDiff<<endl;
	}
	//POD
	else if(atoi(argv[1])==2)
	{
		gm.readDataSet(argv[3]); //read train file
		ldouble podLogLikelihoodDiff = gm.podParam(argv[2],argv[4],argv[5]); //calculate parameters on trainfile, read test file and calculate loglikelihood difference
		cout<<"POD log likelihood(base 2) difference ="<<podLogLikelihoodDiff<<endl;
	}

	/*ofstream myfile;
	myfile.open ("C:\Prasanna\example.txt");
	myfile << "log likelihood difference ="<<trainLikelihood-testLikelihood;
	myfile.close();*/

	//gm.getMinDegreeOrdering(order,clusters);
	//gm.podParam(order);
	//ldouble pe=gm.BE(order);
	//cout<<"The partition function or P(e) is "<<pe<<endl;

	/*
	int num_samples=atoi(argv[3]);
	int w=atoi(argv[4]);
	vector<int> wcv;
	gm.computeWCutset(clusters, w,wcv);
	gm.wCutsetSampling(order,  wcv, num_samples);
	*/
	//system("PAUSE");
	//outfile.close();
}
