#include <time.h>
#include <iostream>
#include <cmath>
#include <vector>
#include <iostream>
#include <queue>
#include <pthread.h>
#include <limits>

#define NoOfNodes 7
#define NoOfThreads 3

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
  vector< vector <int> > returnexcess;  // becomes a 2 dimensional array which is not the case in phase 1
                                              

  PushRelabel(int N) : N(N), G(N), excess(N), dist(N), active(N), returnexcess(N) {}

  void AddEdge(int from, int to, int cap) {
    G[from].push_back(Edge(from, to, cap, 0, G[to].size()));
    if (from == to) G[from].back().index++;
    G[to].push_back(Edge(to, from, 0, 0, G[from].size() - 1));
  }

 void print_graph(int s,int t)    //prints the adjacency list 
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


  pthread_mutex_t lock;
  pthread_t t[NoOfThreads];
  PushRelabel p(NoOfNodes);
  pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
  



void parallel_initialize(int s, int t)
{
  int m,n,d,i,j,l,r,k;                                 //index adjustment

  cout<<"inside initialize"<<"\n";
  p.dist[s]=NoOfNodes;                                //setting height of source as no.of nodes 

  
  for(i=s;i<=p.active.size();i++)
  {
    p.active[i]=0;   
  }
  p.active[s]=p.active[t]=true;                       // s and t became active

  cout<<"active of s = "<<p.active[s]<<"\n";
  cout<<"active of t = "<<p.active[t]<<"\n";
  p.excess[s]=0;



  for(i=(s+1);i<t;i++)
  {
    p.dist[i]=0;
    p.excess[i]=0;
  }

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
           // cout<<"node "<<p.G[s][l].to<<" going to thread "<<l%NoOfThreads<<"\n";
            p.returnexcess[l%NoOfThreads].push_back(p.G[s][l].to);
            /*Basic round-robin schduler of adjecent nodes to the source across number of threads 
            that are there.*/
            }       
      }  
            for(r=0;r<NoOfThreads;r++)
            {
              p.returnexcess[r].push_back(p.returnexcess[r].size());
              /*Pushing the size of the array as an element*/
            }   
            for(k=0;k<NoOfThreads;k++)
            {
              p.returnexcess[k].resize(p.returnexcess[k].size());
              /*Resizing the returnexcess array*/

            }
  cout<<"returning to main"<<"\n";          
}



void *thread(void *tp)
{
  cout<<"entered thread"<<"\n";


  int s,t,u,Qbuffer,tq,*vecbuff,Qindex,tempQint,Qsize_summa;
  int i,j,k,l;     //iteratrors
  int Qsize=0;
  int e,v,h1,h2,d;
  int flag=0;     //to check if it is the first push operation 
  int master_count=0;
  pthread_t tid = pthread_self();
  std::queue<int> privQ,tempQ;           //private queue data structure  
  

s=1;
t=6;
 
int *ant=(int*)tp;        //tp points to the final element in the row of the double dimelnsional array 'returnexcess'
//we have one row per thread of the double dimensional array

   //main while
//cout<<"inside thread : "<<tid<<" excess[s] before while :"<<p.excess[s]<<"excess[t] before while :"<<p.excess[t]<<"\n";

  while((p.excess[s]+p.excess[t])<0)
  {
  int satsat,bbb;
 //  inside while in thread 
   if(flag==0)
   {
    //  cout<< "inside flag==0 loop"<<"\n";
        for(k=1; k<=*ant;k++)     
       {                        
        p.active[*(ant-k)]=1;   //name all the nodes in the returnexcess array active 
        privQ.push(*(ant-k));   //push the active node inside the privQ
       }
   }
   else                               //second push and lift operation(flag=1)
  {
       //second push and lift operation
      Qsize = privQ.size();
      for (l=0;l<Qsize;l++)
      {
        if(p.excess[privQ.front()]<=0)         //if the node's excess is less than 0. 
        {
         Qbuffer = privQ.front();             //Qbuffer holds the first node out of the queue  
          privQ.pop();                        
          p.active[Qbuffer]=0;
          if(!p.active[Qbuffer])
          {
            p.active[Qbuffer]=1;
            privQ.push(Qbuffer);
          }
           
        }

        //++++++++++++++++++++++++++++++start from here +++++++++++++++++++++++++++++++++

        else              //  not the first push operation and excess[node] is greater than 0
        {
          
          LL amt;
          LL excess_t=p.excess[privQ.front()];

                int smallVindex, smallestHtVertex, dist2;
                int dist_t=std::numeric_limits<int>::max();        // h' is dist_t

// at this point we have to find, adjacent vertex which has the least height, 
// to direct the next push operation. 

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
              /* Clarify */
             amt = min(excess_t,(long long)p.G[privQ.front()][smallVindex].cap);

              pthread_mutex_lock(&mutex);         //start of atomic part 

              p.G[privQ.front()][smallVindex].cap  =  p.G[privQ.front()][smallVindex].cap-amt;
              p.G[smallestHtVertex][p.G[privQ.front()][smallVindex].index].cap  =  p.G[smallestHtVertex][p.G[privQ.front()][smallVindex].index].cap+amt;
              p.excess[privQ.front()] = p.excess[privQ.front()]-amt;
              p.excess[smallestHtVertex] = p.excess[smallestHtVertex]+amt;


              pthread_mutex_unlock(&mutex);              //end of atomic part 


              satsat = privQ.front();
               privQ.pop();
               p.active[satsat]=0;
              pthread_mutex_lock(&mutex);

cout<<"node : "<<satsat<<" thread id : "<<tid<<"\n";
              pthread_mutex_unlock(&mutex); 
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
                if(!p.active[bbb])
                {
                  p.active[bbb]=1;
                  privQ.push(bbb);
                }
            }

        }

        //flag=1;
        //break;

      }       // big for closing braces 
      
      master_count++;
     // cout<<"master count : "<<master_count<<"\n";
      if(master_count==4)
      {
        // break;
      }

   }        //big else 
  flag=1;
  //cout<<"thread id = "<<tid<<" privq.empty = "<<privQ.empty()<<"\n";
  if(privQ.size()==0)
  {
      cout<<"thread id = "<<tid<<" privQ.empty is empty \n";
  }
  /*   plasable logic   */
  if(privQ.empty()==false)
  {
    
    Qsize_summa = privQ.size();
    for(Qindex=0;Qindex<Qsize_summa;Qindex++)
    {
      tempQint=privQ.front();
      privQ.pop();
        if(tempQint==t)
        {
          cout<<"red alert!!!!!\n\n";
          while(1);
        }
        privQ.push(tempQint);
    }
    if(privQ.size()!=Qsize_summa)
    {
      cout<<"logic putukichhu\n\n\n\n";
      while(1);
    }
  }
  /*   plasable logic   */
}           //big while

pthread_exit(NULL);
cout<<"exiting thread"<<"\n";
}         //end threadS



int main()
{
  int a,ret;
  int i,j,k,l;     //iteratrors
  int thread_number=0;


  //struct thread_parameters args;

  p.AddEdge(1,2,15);
  p.AddEdge(1,4,4);
 // p.AddEdge(1,6,4);
 // p.AddEdge(1,3,14);
  p.AddEdge(2,3,12); 
  p.AddEdge(4,5,10);
  p.AddEdge(2,5,5);
  p.AddEdge(4,3,3);
  p.AddEdge(3,6,7);
  p.AddEdge(5,6,10);

cout<<"print_graph\n";

p.print_adj_list(1,6);



parallel_initialize(1,6);

 cout<<"back to main"<<"\n"; 
 
for(i=0;i<NoOfThreads;i++)
{
  for(j=0;j<(p.returnexcess[i].size());j++)
 {
    cout<<"i -> "<<i<<"j -> "<<j<<" returnexcess value : "<<p.returnexcess[i][j]<<"\n";
 }
}
for(i=0;i<NoOfThreads;i++)
{
pthread_create(&t[i], NULL,thread,(void *) &p.returnexcess[i][p.returnexcess[i].size()-1]);
}

  for(i=0; i<NoOfThreads; i++)
  {
      pthread_join(t[i], NULL);
  }

  cout << "\n\nThreads terminated.. Back to main ";
for(i=1;i<=6;i++)
{
  cout<<"dist loop : "<<p.dist[i]<<"\n" ;
  cout<<"excess loop : "<<p.excess[i]<<"\n";
}
cout<<"\nExcess of source is "<<p.excess[1]<<"\n ";
cout<<"\nExcess of sink is "<<p.excess[6]<<"\n";
cout<<"\n";

return 0;

 pthread_exit(NULL);
}


