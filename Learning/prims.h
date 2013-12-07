#ifndef PRIMS_H
#define PRIMS_H
typedef long double ldouble;
#include<vector>
using namespace std;

struct prims
{
   vector<vector<ldouble> > graph;
   vector<int> vertices;
   vector<int> parent;
   public:
   prims();
   void createGraph(vector<vector<int> > weight);
   void primsAlgo(int nodes);
};

#endif