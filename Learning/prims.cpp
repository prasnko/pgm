
#include "prims.h"
#include "GM.h"
#define infi 5000  //infi for infinityclass prims


prims :: prims(){
     /*for(int i=0;i<ROW;i++)
       for(int j=0;j<COL;j++)
     graph[i][j]=0;*/
}

//void prims :: createGraph(){
//    int i,j;
//    //cout<<"Enter Total Nodes : ";
//    //cin>>nodes;
//    //cout<<"\n\nEnter Adjacency Matrix : \n";
//    for(i=0;i<nodes;i++)
//        for(j=0;j<nodes;j++)
//        cin>>graph[i][j];
//
//    //Assign infinity to all graph[i][j] where weight is 0.for(i=0;i<nodes;i++){
//        for(j=0;j<nodes;j++){
//           if(graph[i][j]==0)
//          graph[i][j]=infi;
//        }
//    }
//}

void prims :: primsAlgo(int nodes){
    vector<int> selected(nodes);
	
	int i,j,ne; //ne for no. of edges
	int x,y;
	parent.resize(nodes);
	for(i=0;i<nodes;i++)
       parent[i]=-1;
    for(i=0;i<nodes;i++)
       selected[i]=false;

    selected[0]=true;
    ne=0;
	
    while(ne < nodes-1){
       long double max=0;
	   
       for(i=0;i<nodes;i++)
       {
          if(selected[i]==true){
         for(j=0;j<nodes;j++){
            if(selected[j]==false){
               if(max < graph[i][j])
               {
               max=graph[i][j];
               x=i;
               y=j;
               }
            }
           }
          }
       }
       selected[y]=true;
       /*cout<<"\n"<<x+1<<" --> "<<y+1;*/
	   if(ne==0)
		   vertices.push_back(x);
	   vertices.push_back(y);
	   parent[y]=x;
       ne=ne+1;
    }
}
