/*
 * Main.cpp
 *
 *  Created on: Mar 25, 2013
 *      Author: Vibhav Gogate
 *				The University of Texas at Dallas
 *				All rights reserved
 */




#include "GM.h"
#include "LogDouble.h"
typedef long double ldouble;
#include<math.h>

int main(int argc, char* argv[])
{
	if (argc < 5){
		cerr<<"Usage: "<<argv[0]<<" <uaifile> <evidfile> <num_samples> <w> <a=1:0 (adaptive:wcutset)>\n";
		exit(-1);
	}
	GM gm;
	gm.readUAIGM(argv[1]);
	gm.readUAIEvidence(argv[2]);

	vector<int> order;
	vector<set<int> > clusters;
	gm.getMinDegreeOrdering(order,clusters);

	//LogDouble pe=gm.BE(order);
	//cout<<"The partition function or P(e) is "<<exp(pe.getValue())<<endl;

	LogDouble sPe;
	int num_samples=atoi(argv[3]);
	int w=atoi(argv[4]);
	vector<int> wcv;
	bool yes = false;
	gm.computeWCutset(clusters, order, w,wcv);
	if(atoi(argv[5]))
	 sPe= gm.adaptiveWCutsetSampling(order,  wcv, num_samples);
	else
	 sPe= gm.wCutsetSampling(order,  wcv, num_samples);
	/*ofstream outfile("C:\\Prasanna\\Spring13\\PGM\\HW3.5\\SamplingVE\\Output.txt");
	if(!outfile.good()){
		cerr<<"Error opening GM file\n";
		exit(-1);
	}	

	outfile<< std::fixed  << sPe.ConvertToDouble();
	outfile.close();*/
	cout<<"lgePr(e)= "<< sPe.ConvertToDouble()<<endl;
	cout<<"Pr(e)= "<< exp(sPe.ConvertToDouble());
	//cout<<"The partition function or P(e) is "<<exp(sPe.getValue())<<endl;
}
