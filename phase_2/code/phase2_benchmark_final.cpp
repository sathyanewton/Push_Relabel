#include <time.h>
#include <stdio.h>
#include <iostream>
#include <cmath>
#include <vector>
#include <queue>
#include <pthread.h>
#include <limits>

#define NoOfNodes 501

using namespace std;

typedef long long LL;

  struct Edge  {
  int from, to, cap, flow, index;
  Edge(int from, int to, int cap, int flow, int index) :
     from(from), to(to), cap(cap), flow(flow), index(index) {}
 };

 struct PushRelabel{

int N;

  vector<vector<Edge> > G;
  vector<LL> excess;
  vector<int> dist, active;
  queue<int> Q;
  vector<int> buffer;
  vector< vector <int> > returnexcess;           // becomes a 2 dimensional array

  PushRelabel(int N) : N(N), G(N), excess(N), dist(N), active(N), returnexcess(N) {}

  void AddEdge(int from, int to, int cap) {
    G[from].push_back(Edge(from, to, cap, 0, G[to].size()));
    if (from == to) G[from].back().index++;
    G[to].push_back(Edge(to, from, 0, 0, G[from].size() - 1));
  }

 void print_graph(int s,int t)
  {
      for(int i=s;i<t;i++)
      {
        for(int j=0;j<G[i].size();j++)
        {
          cout<<"from -> "<<G[i][j].from<<"to -> "<<G[i][j].to <<"index -> "<<G[i][j].index;
        }
        cout<<"\n";
      }
  }

   void print_adj_list(int s,int t)
  {
      for(int i=s;i<=t;i++)
      {
        cout<<i;
        for(int j=0;j<G[i].size();j++)
        {
          cout<<"->"<<G[i][j].to;
        }
        cout<<"\n";
      }
  }
  void Enqueue(int v) {
    if (!active[v] && excess[v] > 0) 
      { 
        active[v] = true; 
        Q.push(v); 
      }
  }
};

/* Global variables */

  PushRelabel p(NoOfNodes);     
  int thread_id=0;

          FILE *fp,*fp2;
            int operation[5];
            int edgeCount[3];
            int nodeCount[3];

            int var1[3],var2[3];

void initialize(int s, int t)
{
  int m,n,d,i,j,l,r,k;                                    //index adjustment

  cout<<"inside initialize"<<"\n";
  p.dist[s]=*nodeCount;                                //setting height of source as no.of nodes 

  
  for(i=s;i<=p.active.size();i++)
  {
    p.active[i]=0;
  }
  p.active[s]=p.active[t]=true;                       // s and t became active
  p.excess[s]=0;


  for(i=(s+1);i<t;i++)
  {
    p.dist[i]=0;
    p.excess[i]=0;
  }
cout<<"nodes adjecent to the source "<<"\n";
cout<<"size of the source array "<<p.G[s].size()<<"\n";
      for(l=0;l<p.G[s].size();l++)
      {
    	    p.G[s][l].flow = p.G[s][l].cap;
            p.G[s][l].cap=0;
            p.G[p.G[s][l].to][p.G[s][l].index].cap=p.G[s][l].flow;
            p.excess[p.G[s][l].to] = p.G[s][l].flow;
            p.excess[s] = (p.excess[s]-p.G[s][l].flow);
            p.Enqueue (p.G[s][l].to);
            
            /*Basic round robin scheduler*/
           
            
            if (((p.G[s][l].to)!=s) && ((p.G[s][l].to)!=t))
            {
            // cout<<p.G[s][l].to<<"\n";
            p.returnexcess[0].push_back(p.G[s][l].to);
            }      
      }  
               
}

void thread(int tp)
{
  cout<<"entered thread-function"<<"\n";


  int s,t,u,Qbuffer;
  int i,j,k,l;     //iteratrors
  int Qsize=0;
  int e,v,h1,h2,d;
  int flag=0;     //to check if it is the first push operation 
  int master_count=0;
 // pthread_t tid = pthread_self();
  std::queue<int> privQ;           //private queue data structure  

s=1;
t=*nodeCount;
int ant=(int)tp;  
cout<< "excess[s] before while :"<<p.excess[s]<<" excess[t] before while :"<<p.excess[t]<<"\n\n\n";
//cout<<"number of nodes adjacent to source "<<tp<<"\n";
  while((p.excess[s]+p.excess[t])<0)
  {
  	//cout<< "excess["<<s<<"] :"<<p.excess[s]<<"excess["<<t<<"] :"<<p.excess[t]<<"\n";
  	//cout<<"inside while\n";
  int satsat=0,bbb=0;
   if(flag==0)
   {
       for(k=0; k<=ant;k++)     
       {                        
        p.active[p.returnexcess[0][k]]=1;   // push 
       // cout<<"nodes adjacent to source : "<<p.returnexcess[0][k]<<"\n";
        privQ.push(p.returnexcess[0][k]);
       }
      
   }
   else                               //second push and lift operation 
  {
      Qsize = privQ.size();
      for(l=0;l<Qsize;l++)
      {
        if(p.excess[privQ.front()]<=0)          //if the node's excess is less than 0. 
        {
          Qbuffer = privQ.front();
          privQ.pop();
          p.active[Qbuffer]=0;
          if(!p.active[Qbuffer])
          {
            p.active[Qbuffer]=1;
            /*check for sink*/
          if(Qbuffer==t)
          {
          cout<<"red alert!!!!!!!!!\n\n\n\n";
          while(1);
          }
            privQ.push(Qbuffer);
          }
        }
        else              //  not the first push operation and excess[node] is greater than 0
        {
          
          LL amt;
          LL excess_t=p.excess[privQ.front()];
          int smallVindex, smallestHtVertex, dist2;
                int dist_t=std::numeric_limits<int>::max();        // h' is dist_t

          // at this point we have to find, adjacent vertex which has the least height, to direct the next push operation. 
              //  cout<<"at this point we have to find, adjacent vertex which has the least height, to direct the next push operation.\n";
          for (i=0; i<p.G[privQ.front()].size(); i++)   
            {
                  if ((p.G[privQ.front()][i].cap)<=0) continue;
                    dist2 = p.dist[p.G[privQ.front()][i].to];    // h" is dist2
                      if (dist2 < dist_t)
                      {
                          smallestHtVertex=p.G[privQ.front()][i].to;    // v' is smallestHtVertex
                          smallVindex=i;
                          dist_t=dist2;                   // height of the lowest height vertex
                      }
            }
            // do the push for (u, lowest v chosen by the above for loop)
            if(p.dist[privQ.front()]>dist_t)
            {
              amt = min(excess_t,(long long)p.G[privQ.front()][smallVindex].cap);
           //   pthread_mutex_lock(&mutex);         //start of atomic part 

              p.G[privQ.front()][smallVindex].cap  =  p.G[privQ.front()][smallVindex].cap-amt;
              p.G[smallestHtVertex][p.G[privQ.front()][smallVindex].index].cap  =  p.G[smallestHtVertex][p.G[privQ.front()][smallVindex].index].cap+amt;
              p.excess[privQ.front()] = p.excess[privQ.front()]-amt;
              p.excess[smallestHtVertex] = p.excess[smallestHtVertex]+amt;


            //  pthread_mutex_unlock(&mutex);              //end of atomic part 


              satsat = privQ.front();
               privQ.pop();
               p.active[satsat]=0;
               if(((p.G[satsat][smallVindex].to)!=1)&((p.G[satsat][smallVindex].to)!=t)&(!p.active[p.G[satsat][smallVindex].to]))
                {
                    /*check for sink*/
                  if(p.G[satsat][smallVindex].to==t)
                  {
                  cout<<"red alert!!!!!!!!!\n\n\n\n";
                  while(1);
                  }
                    p.active[p.G[satsat][smallVindex].to]=1;
                    privQ.push(p.G[satsat][smallVindex].to);
                }
                if(!p.active[satsat])
                {
                    p.active[satsat]=1;
                  if(satsat==t)
                  {
                  cout<<"red alert!!!!!!!!!\n\n\n\n";
                  while(1);
                  }
                    privQ.push(satsat);
                }

            }
            else          //if ((smallestHtVertex!=1)&(smallestHtVertex!=6))
            {
                bbb=privQ.front();
                privQ.pop();
                p.active[bbb]=0;
                p.dist[bbb] = dist_t + 1;
                if(!p.active[bbb])
                {
                  p.active[bbb]=1;
                  if(bbb==t)
                  {
                  cout<<"red alert!!!!!!!!!\n\n\n\n";
                  while(1);
                  }
                  privQ.push(bbb);
                }
            }

        }

      }       // big for closing braces 
      
      master_count++;
      if(master_count==8)
      {
       // break;
      }

   }        //big else 
  flag=1;
  
}
           //big while
cout<<"exiting thread"<<"\n";
}         //end threadS

int main()
{
  int a,ret;
  int i,j,k,l;     //iteratrors
  int thread_number=0;
int co=0;
  fp = fopen("out5.txt", "r");
            if (fp == NULL) {
                    printf("Can't open input file!\n");
                    while(1);
                }
            if(fscanf(fp, "%d %d", &nodeCount, &edgeCount) != EOF)
               {
              // printf("\n Node count and Edge count are %d %d", *nodeCount, *edgeCount);

               }

            while (fscanf(fp, "%d %d %d", &operation,&var1,&var2) != EOF)
               {      //printf("\nThe values are %d %d %d", *operation, *var1, *var2);
                             p.AddEdge(*operation, *var1, *var2);
               }

cout<<"\n node count : "<<*nodeCount<<"\n";

initialize(1,*nodeCount);

//cout<<"goma : "<<p.returnexcess[0].size()<<"\n";

 cout<<"back to main"<<"\n"; 
cout<<"adjacent nodes to source"<<"\n";

thread((int) p.returnexcess[0].size());


//  cout << "\n\nThreads terminated.. Back to main ";
cout<<"\nExcess of source is "<<p.excess[1]<<"\n ";
cout<<"\nExcess of sink is "<<p.excess[*nodeCount]<<"\n";
cout<<"\n";

return 0;
}