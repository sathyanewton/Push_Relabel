#include <time.h>
#include <stdio.h>
#include <iostream>
#include <cmath>
#include <vector>
#include <queue>
#include <pthread.h>
#include <limits>


#define NoOfNodes 9
#define NoOfThreads 4

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
        //cout<<"inside enqueue0\n";
        active[v] = true; 
        //cout<<"inside enqueue1\n";
        Q.push(v); 
        //cout<<"inside enqueue2\n";
      }
  }
};

/* Global variables */

  pthread_mutex_t lock;
  pthread_t t[NoOfThreads];
  PushRelabel p(NoOfNodes);
  pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
  pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;
  int thread_id=0;                           //debugging variables         

          FILE *fp,*fp2;
            int operation[5];
            int edgeCount[3];
            int nodeCount[3];

            int var1[3],var2[3];




void parallel_initialize(int s, int t)
{
  int m,n,d,i,j,l,r,k;                                    //index adjustment

  cout<<"inside initialize"<<"\n";
  p.dist[s]=NoOfNodes;                                //setting height of source as no.of nodes 

  
  for(i=s;i<=p.active.size();i++)
  {
    p.active[i]=0;
    //cout<<"p.active["<<i<<"]"<<" = "<<p.active[i]<<"\n";    //printing active[i]
  }
  p.active[s]=p.active[t]=true;                       // s and t became active

  cout<<"active of s = "<<p.active[s]<<"\n";
  cout<<"active of t = "<<p.active[t]<<"\n";
  //cout<<"p.dist["<<s<<"]"<<" = "<<p.dist[s]<<"\n";
  p.excess[s]=0;
  //cout<<"p.excess["<<s<<"]"<<" = "<<p.excess[s]<<"\n";


  for(i=(s+1);i<t;i++)
  {
    p.dist[i]=0;
    //cout<<"p.dist["<<i<<"]"<<" = "<<p.dist[i]<<"\n";
    p.excess[i]=0;
    //cout<<"p.excess["<<i<<"]"<<" = "<<p.excess[i]<<"\n";
  }

      for(l=0;l<p.G[s].size();l++)
      {
        //cout<<"inside main chunk of parallel_initialize\n";

            p.G[s][l].flow = p.G[s][l].cap;
            p.G[s][l].cap=0;
            p.G[p.G[s][l].to][p.G[s][l].index].cap=p.G[s][l].flow;
            p.excess[p.G[s][l].to] = p.G[s][l].flow;
            p.excess[s] = (p.excess[s]-p.G[s][l].flow);
            p.Enqueue (p.G[s][l].to);
            
            /*Basic round robin scheduler*/
            //cout<<"Basic round robin scheduler"<<"\n";
            if (((p.G[s][l].to)!=s) && ((p.G[s][l].to)!=t)&&(p.G[s][l].to)!=0)
            {
           // cout<<"node "<<p.G[s][l].to<<" going to thread "<<l%NoOfThreads<<"\n";
            p.returnexcess[l%NoOfThreads].push_back(p.G[s][l].to);
            }       
      }  
            for(r=0;r<NoOfThreads;r++)
            {
             // cout<<"pushing size for thread :"<<r<<"\n";
              p.returnexcess[r].push_back(p.returnexcess[r].size());
            }   
            for(k=0;k<NoOfThreads;k++)
            {
             // cout<<"resizing thread :"<<k<<"\n";
              p.returnexcess[k].resize(p.returnexcess[k].size());
            }
  cout<<"returning to main"<<"\n";          
}



void *thread(void *tp)
{
  cout<<"entered thread"<<"\n";


  int s,t,u,Qbuffer,tq,*vecbuff;
  int i,j,k,l;     //iteratrors
  int Qsize=0;
  int e,v,h1,h2,d;
  int flag=0;     //to check if it is the first push operation 
  int master_count=0;
  pthread_t tid = pthread_self();
  std::queue<int> privQ;           //private queue data structure  

s=1;
t=*nodeCount;
 pthread_mutex_lock(&mutex2);
 thread_id++;
 pthread_mutex_unlock(&mutex2);

int *ant=(int*)tp;  


  //cout<<"after checkpoint"<<"\n";
   //main while
cout<<"inside thread : "<<thread_id<<" excess[s] before while :"<<p.excess[s]<<"excess[t] before while :"<<p.excess[t]<<"\n";
//cout <<"Excess of s is "<<p.excess[s]<<"\n";
  // cout<<"Excess of t is "<<p.excess[t]<<"\n";
  while((p.excess[s]+p.excess[t])<0)
  {
  int satsat,bbb;
 //  cout<<"inside while in thread "<<u<<"\n";
  // cout <<"Excess of s is "<<p.excess[s]<<"\n";
  //cout<<"Excess of t is "<<p.excess[t]<<"\n";
   if(flag==0)
   {
      cout<< "inside flag==0 loop"<<"\n";
        for(k=1; k<=*ant;k++)     
       {                        //
        p.active[*(ant-k)]=1;   // push 
        privQ.push(*(ant-k));
        cout<<"whats in the queue : "<<*(ant-k)<<"of thread :"<<thread_id<<"\n";
       }
   }
   else                               //second push and lift operation 
  {
   // while(1);
     // cout<<"second push and lift operation"<<"\n"; 
      Qsize = privQ.size();
     // cout <<"PrivQ.size inside the 'else' case of for loop is "<<Qsize<<"\n";
      for (l=0;l<Qsize;l++)
      {
        if(p.excess[privQ.front()]<=0)          //if the node's excess is less than 0. 
        {
          //cout << "Excess value is <= 0 value of privQ.front is "<< privQ.front()<<"\n";
          Qbuffer = privQ.front();
          privQ.pop();
          p.active[Qbuffer]=0;
          //cout << "privQ's size before a push operation : "<<privQ.size()<<" privQ's front "<< privQ.front()<<"\n";
          if(!p.active[Qbuffer])
          {
            p.active[Qbuffer]=1;
            privQ.push(Qbuffer);
          }
           //cout << "privQ's size after a push operation : "<<privQ.size()<<" privQ's front "<< privQ.front()<<"\n";
        }

        //+++++++++++++++++++++++start from here +++++++++++++++++++++++++++++++++

        else              //  not the first push operation and excess[node] is greater than 0
        {
          
         // cout << "Entered Discharge fn for node "<<privQ.front()<<"\n";

          LL amt;
          LL excess_t=p.excess[privQ.front()];

         // cout << "Excess of node "<<privQ.front()<< " is "<<excess_t<<"\n";
          int smallVindex, smallestHtVertex, dist2;
                int dist_t=std::numeric_limits<int>::max();        // h' is dist_t

          // at this point we have to find, adjacent vertex which has the least height, to direct the next push operation. 
              //  cout<<"at this point we have to find, adjacent vertex which has the least height, to direct the next push operation.\n";
          for (i=0; i<p.G[privQ.front()].size(); i++)   
            {
                  if ((p.G[privQ.front()][i].cap)<=0) continue;
               //   cout << "\nEntered for loop while finding min-height of the node i = " <<i<<"\n";
                    dist2 = p.dist[p.G[privQ.front()][i].to];    // h" is dist2
                      if (dist2 < dist_t)
                      {
                       //   cout <<"\n entering 'if' loop to check dist2 < dist_t\n";
                          smallestHtVertex=p.G[privQ.front()][i].to;    // v' is smallestHtVertex
                          smallVindex=i;
                       //   cout << "\n ------- Value of smallest ht vertex is "<<smallestHtVertex<<"\n";
                          dist_t=dist2;                   // height of the lowest height vertex
                      }
            }

            // do the push for (u, lowest v chosen by the above for loop)
            if(p.dist[privQ.front()]>dist_t)
            {
              /* Clarify */
             // cout << "\nGoing to execute push-----Entering loop to check if dist[u] > dist_t\n";
             // cout<<"excess is "<<excess_t<<"\n";

              

              amt = min(excess_t,(long long)p.G[privQ.front()][smallVindex].cap);
             // cout << "\nMin amount is " << amt<<"\n";

              pthread_mutex_lock(&mutex);         //start of atomic part 

              p.G[privQ.front()][smallVindex].cap  =  p.G[privQ.front()][smallVindex].cap-amt;
              p.G[smallestHtVertex][p.G[privQ.front()][smallVindex].index].cap  =  p.G[smallestHtVertex][p.G[privQ.front()][smallVindex].index].cap+amt;
              p.excess[privQ.front()] = p.excess[privQ.front()]-amt;
              p.excess[smallestHtVertex] = p.excess[smallestHtVertex]+amt;


              pthread_mutex_unlock(&mutex);              //end of atomic part 


              satsat = privQ.front();
               privQ.pop();
               p.active[satsat]=0;
           //   pthread_mutex_lock(&mutex);

             cout<<"node : "<<satsat<<" thread id : "<<thread_id<<"\n";
            //cout<<"node : "<<satsat<<" thread id : "<<tid<<"\n";
             // pthread_mutex_unlock(&mutex); 
               if(((p.G[satsat][smallVindex].to)!=1)&((p.G[satsat][smallVindex].to)!=6)&(!p.active[p.G[satsat][smallVindex].to]))
                {
                    p.active[p.G[satsat][smallVindex].to]=1;
                    privQ.push(p.G[satsat][smallVindex].to);
                }
                if(!p.active[satsat])
                {
                    p.active[satsat]=1;
                    privQ.push(satsat);
                }

            }
            else          //if ((smallestHtVertex!=1)&(smallestHtVertex!=6))
            {
                bbb=privQ.front();
                privQ.pop();
                p.active[bbb]=0;
                p.dist[bbb] = dist_t + 1;
                //cout << "\nDistance of node "<<bbb<<" is " << p.dist[bbb]<<"\n";
                if(!p.active[bbb])
                {
                  p.active[bbb]=1;
                  privQ.push(bbb);
                }
            }

          //  cout<<"\n Size of privQ is " <<privQ.size()<<"\n";
             
            //break;
        }

        //flag=1;
        //break;

      }       // big for closing braces 
      
      master_count++;
     // cout<<"master count : "<<master_count<<"\n";
      if(master_count==4)
      {
         //break;
      }

   }        //big else 
  flag=1;
  
}
           //big while
//pthread_mutex_unlock(&mutex);
pthread_exit(NULL);
cout<<"exiting thread"<<"\n";
}         //end threadS



int main()
{
  int a,ret;
  int i,j,k,l;     //iteratrors
  int thread_number=0;
int co=0;
  fp = fopen("out3.txt", "r");
            if (fp == NULL) {
                    printf("Can't open input file!\n");
                    while(1);
                }
            if(fscanf(fp, "%d %d", &nodeCount, &edgeCount) != EOF)
               {
               //printf("\n Node count and Edge count are %d %d", *nodeCount, *edgeCount);

               }

            while (fscanf(fp, "%d %d %d", &operation,&var1,&var2) != EOF)
               {     // printf("\nThe values are %d %d %d", *operation, *var1, *var2);
                             p.AddEdge(*operation, *var1, *var2);
               }

  //struct thread_parameters args;
/*
  p.AddEdge(1,2,15);
  p.AddEdge(1,4,4);
  p.AddEdge(1,6,4);
  p.AddEdge(1,3,14);
  p.AddEdge(2,3,12); 
  p.AddEdge(4,5,10);
  p.AddEdge(2,5,5);
  p.AddEdge(4,3,3);
  p.AddEdge(3,6,7);
  p.AddEdge(5,6,10);
*/
//cout<<"print_graph\n";
//p.print_graph(1,6);
  *nodeCount=(NoOfNodes-1);
p.print_adj_list(1,*nodeCount);

//cout<<"inverse coming up";
 

//cout<<"after args pass\n";

cout<<"\n\n\n\n\n\n\n\n\n\n node count : "<<*nodeCount;

parallel_initialize(1,*nodeCount);

 cout<<"back to main"<<"\n"; 
cout<<"adjacent nodes to source"<<"\n";
for(i=0;i<NoOfThreads;i++)
{
 for(j=0;j<(p.returnexcess[i].size());j++)
 {
  cout<<p.returnexcess[i][j]<<" ,";
 }
}
cout<<"\n";
for(i=0;i<NoOfThreads;i++)
{
  for(j=0;j<(p.returnexcess[i].size());j++)
 {
  cout<<"i -> "<<i<<"j -> "<<j<<" returnexcess value : "<<p.returnexcess[i][j]<<"\n";
  //cout<<p.returnexcess[i][j]<<" ,";
 }
}
cout<<"\n";
for(i=0;i<NoOfThreads;i++)
{
pthread_create(&t[i], NULL,thread,(void *) &p.returnexcess[i][p.returnexcess[i].size()-1]);
}

  for(i=0; i<NoOfThreads; i++)
  {
      pthread_join(t[i], NULL);
  }

  cout << "\n\nThreads terminated.. Back to main ";
for(i=1;i<=*nodeCount;i++)
{
 // cout<<"dist loop : "<<p.dist[i]<<"\n" ;
 // cout<<"excess loop : "<<p.excess[i]<<"\n";
}
cout<<"\nExcess of source is "<<p.excess[1]<<"\n ";
cout<<"\nExcess of sink is "<<p.excess[*nodeCount]<<"\n";
cout<<"\n";

return 0;

 pthread_exit(NULL);
}


