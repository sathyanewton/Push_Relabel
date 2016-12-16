#include <sys/time.h>
#include <iostream>
#include <pthread.h>
#include <vector>
#include <cmath>
#include <queue>
#include <assert.h>
#include <algorithm>
#include <limits>
#include <atomic>
#include <sched.h>
#undef max

using namespace std;
typedef long long LL;

#define NoOfNodes 2001
#define NoOfThreads 4

  /*  File hadelling global variables */

          FILE *fp,*fp2;
            int operation[5];
            int edgeCount[3];
            int nodeCount[3];

            int var1[3],var2[3];

/*  Work sharing global variables */

//  int wc=0;
  vector<int> global_worksharing_buffer;  

/* Global variables : We have a many sets of global variables */

/* Thread-based global variables   */
  pthread_mutex_t lock;
  pthread_t t[NoOfThreads];
  pthread_t heuristics;
  pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;    // critical operations 
  pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;   // wc mutex
  pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;   // thread id
//  pthread_mutex_t mutex3 = PTHREAD_MUTEX_INITIALIZER;   // master count 
  pthread_mutex_t mutex4 = PTHREAD_MUTEX_INITIALIZER;   // worksharing buffer 
  pthread_mutex_t mutex5 = PTHREAD_MUTEX_INITIALIZER; 
  pthread_cond_t  cond   = PTHREAD_COND_INITIALIZER;      
//  int thread_id1=0;


 /* Global atomic variables and their constructors */ 

  struct AtomicCounter {
    std::atomic<int> value;

    AtomicCounter() : value(0) {}

    void increment(){
        ++value;
    }

    void decrement(){
        --value;
    }

    int get(){
        return value.load();
    }
};

/*
      member functions of atomic excess
            .value ==> reading the value of excess of a given index used in (LHS)
            .get == > used to write values from the excess array (RHS)


*/

struct AtomicExcess {
    std::atomic<int> value;

    AtomicExcess() : value() {}

    AtomicExcess(const std::atomic<int> &a)
       :value(a.load()) {}

    AtomicExcess &operator=(const AtomicExcess &other)
    {
        value.store(other.value.load());
    }

    AtomicExcess(const AtomicExcess& ae) : value(ae.value.load()) {}; // user-defined copy ctor

    AtomicExcess(int excess) : value(excess) {}

    int get(){
        return value.load();
    }
};

/*Functions that we have in the atomic-cap
          : .get() ==> if we have to write the capacity (generally if the capacity is in RHS)
          : .value() ==> When we just read the value of the capacity (generally used in LHS)
*/

struct AtomicCap {
    std::atomic<int> value;

    AtomicCap(const AtomicCap& ac) : value(ac.value.load()) {}; // user-defined copy ctor

    AtomicCap(int cap) : value(cap) {}

    int get(){
        return value.load();
    }
};



AtomicCounter wc;
AtomicCounter thread_id,cpuvar;

/*  Main global data structures */          

  struct Edge  {
  int from, to, flow, index;
  AtomicCap cap;                                          // atomic int capacity
  Edge(int from, int to, int cap, int flow, int index) :
     from(from), to(to), cap(cap), flow(flow), index(index) {}
 };

 /*Finally we have to replace the 4 operations with atomic_fetch_add*/

struct PushRelabel{

int N;

  vector<vector<Edge> > G;
  vector<AtomicExcess> excess;                          // atomic excess
  vector<int> dist, active;                             //  height and active array
  queue<int> Q;
  vector<int> buffer;
  vector< vector <int> > returnexcess;                  // becomes a 2 dimensional array

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
    if (!active[v] && excess[v].value > 0) 
      { 
        active[v] = true; 
        Q.push(v); 
      }
  }
};


  PushRelabel p(NoOfNodes);


  void print_queue(std::queue<int>& queue1)
  {
    int i;
    int buff;
    int size;
    size = queue1.size();
    for(i=0;i<size;i++);
    {
        cout<<queue1.front()<<"\n";
        buff = queue1.front();
        queue1.pop();
        queue1.push(buff);
    }


  }
 
void parallel_initialize(int s, int t)
{
  int m,n,d,i,j,l,r,k;                                    //index adjustment

//  cout<<"inside initialize"<<"\n";
  p.dist[s]=*nodeCount;                                //setting height of source as no.of nodes 

  
  for(i=s;i<=p.active.size();i++)
  {
    p.active[i]=0;
  }
  p.active[s]=p.active[t]=true;                       // s and t became active
  p.excess[s].value=0;


  for(i=(s+1);i<t;i++)
  {
    p.dist[i]=0;
    p.excess[i].value=0;
  }

      for(l=0;l<p.G[s].size();l++)
      {
          p.G[s][l].flow = p.G[s][l].cap.value;
            p.G[s][l].cap.value=0;
            p.G[p.G[s][l].to][p.G[s][l].index].cap.value=p.G[s][l].flow;
            p.excess[p.G[s][l].to].value = p.G[s][l].flow;
            p.excess[s].value = (p.excess[s].value-p.G[s][l].flow);
            p.Enqueue (p.G[s][l].to);
            
            /*Basic round robin scheduler*/
            if (((p.G[s][l].to)!=s) && ((p.G[s][l].to)!=t))
            {
            p.returnexcess[l%NoOfThreads].push_back(p.G[s][l].to);
            }       
      }  
            for(r=0;r<NoOfThreads;r++)
            {
              p.returnexcess[r].push_back(p.returnexcess[r].size());
            }   
            for(k=0;k<NoOfThreads;k++)
            {
              p.returnexcess[k].resize(p.returnexcess[k].size());
            } 
  
}

void *thread(void *tp)
{

  int s,t,u,Qbuffer;
  int i,j,k,l,m;     //iteratrors.
  int thread_stuff;
  int rc=0;
  int Qsize=0;
  int e,v,h1,h2,d;
  int flag=0;     //to check if it is the first push operation 
  int master_count=0;
  pthread_t tid = pthread_self();     
  std::queue<int> privQ;           //private queue data structure  
  int thread_id1=0;
  int stash,work_sharing_size;
  int number_of_requests=0;
  int number_of_push_operations=0;
  int number_of_lift_operations=0;



s=1;
t=*nodeCount;

cpu_set_t cpuset1, cpuset2 ;
CPU_ZERO(&cpuset1);
CPU_ZERO(&cpuset2);

pthread_mutex_lock(&mutex5);
CPU_SET((cpuvar.get())%8, &cpuset1);
cpuvar.increment();
pthread_mutex_unlock(&mutex5);
thread_stuff = pthread_setaffinity_np(tid, sizeof(cpu_set_t), &cpuset1);
  if (thread_stuff != 0)
  {
      cout<<"thread binding not happeneing"<<"\n";

  }


 // cout<<"entered thread : "<<thread_id1<<"\n";
 
int *ant=(int*)tp;  

pthread_mutex_lock(&mutex2);
thread_id.increment();
thread_id1=thread_id.get();
pthread_mutex_unlock(&mutex2);


  while((p.excess[s].value+p.excess[t].value)<0)
  {

    /*  vertex exchange */
      
    if(privQ.empty()==true && flag==1)
    {
     cout<<"@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ queue empty : thread id "<<thread_id1<<"\n\n\n\n\n";
    number_of_requests++;

      wc.increment();
   
pthread_mutex_lock(&mutex4);
 
            while(global_worksharing_buffer.empty()==true && (p.excess[s].value+p.excess[t].value)<0)
            {
                pthread_cond_wait(&cond,&mutex4);

            }
            rc = global_worksharing_buffer.size();
            cout<<"vertex exchange happeneing in thread : "<<thread_id1<<"from thread "<<i<<"\n";
            for(m=0;m<rc;m++)
            {
              p.active[global_worksharing_buffer[m]]=1;
              privQ.push(global_worksharing_buffer[m]);
            }
            global_worksharing_buffer.clear();

 pthread_mutex_unlock(&mutex4); 

      wc.decrement();    
              
    }

else if((wc.get())>0 && (privQ.size())>1)
{
 
  work_sharing_size = privQ.size();
  work_sharing_size=work_sharing_size/2;
 pthread_mutex_lock(&mutex4);
  for(j=0;j<work_sharing_size;j++)
  {
  stash = privQ.front();
  privQ.pop();
  global_worksharing_buffer.push_back(stash);
  }
  pthread_mutex_unlock(&mutex4);
  pthread_cond_signal(&cond);
  cout<<"giving thread : "<<thread_id1<<"\n";
}
    
else            //big big else 
{ 
  int satsat=0,bbb=0;
  if(flag==0)
   {
        for(k=1; k<=*ant;k++)     
       {                        
        p.active[*(ant-k)]=1;   // push 
        privQ.push(*(ant-k));
       }
       flag=1;
   }
   else                               //second push and lift operation --> big else 
  {
      Qsize = privQ.size();
      for(l=0;l<Qsize;l++)
      {
       
        if(p.excess[privQ.front()].value<=0)          //if the node's excess is less than 0. 
        {
          Qbuffer = privQ.front();
          privQ.pop();
          p.active[Qbuffer]=0;
          if(!p.active[Qbuffer])
          {
            p.active[Qbuffer]=1;
            /*check for sink*/
            privQ.push(Qbuffer);
          }
        }
        else       //sim_sim_else       //  not the first push operation and excess[node] is greater than 0
        {
          
          int amt;
          LL excess_t=p.excess[privQ.front()].value;
          int smallVindex, smallestHtVertex, dist2;
                int dist_t=std::numeric_limits<int>::max();        // h' is dist_t

          // at this point we have to find, adjacent vertex which has the least height, to direct the next push operation. 
              //  cout<<"at this point we have to find, adjacent vertex which has the least height, to direct the next push operation.\n";
          for (i=0; i<p.G[privQ.front()].size(); i++)   
            {
                  if ((p.G[privQ.front()][i].cap.value)<=0) continue;
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
              amt = min(excess_t,(long long)p.G[privQ.front()][smallVindex].cap.value);

             if(p.dist[privQ.front()]>p.dist[smallestHtVertex])   //AGR IF ==> 1
             { 
              number_of_push_operations++;
    
              /*  four main atomic operations */


       std::atomic_fetch_add(&(p.G[privQ.front()][smallVindex].cap.value), -amt);
      std::atomic_fetch_add(&(p.G[smallestHtVertex][p.G[privQ.front()][smallVindex].index].cap.value), amt);
      std::atomic_fetch_add(&(p.excess[privQ.front()].value), -amt);
     std::atomic_fetch_add(&(p.excess[smallestHtVertex].value), amt);

              satsat = privQ.front();
               privQ.pop();
               p.active[satsat]=0;
               if(((p.G[satsat][smallVindex].to)!=1)&((p.G[satsat][smallVindex].to)!=t)&(!p.active[p.G[satsat][smallVindex].to]))
                {
                    /*check for sink*/
                    p.active[p.G[satsat][smallVindex].to]=1;
                    privQ.push(p.G[satsat][smallVindex].to);
                }
                if(!p.active[satsat])
                {
                    p.active[satsat]=1;
                    privQ.push(satsat);
                }

            }           //AGR_IF 1 close brackets 
          }
            else          //if ((smallestHtVertex!=1)&(smallestHtVertex!=6))
            {
                bbb=privQ.front();
                privQ.pop();
                if(p.dist[bbb]<(dist_t + 1))    // FOR AGR HEURISTICS (AGR-IF 2)
                {
                  number_of_lift_operations++; 
                  p.active[bbb]=0;
                  p.dist[bbb] = dist_t + 1;
                if(!p.active[bbb])
                {
                  p.active[bbb]=1;
                  privQ.push(bbb);
                }
              }             //AGR IF CLOSE BRACKETS
            }
        }      //simsim_else close bracket
      }       // big for closing braces 
   }        //big else 
 }    // big big else 

master_count++;
}           //big while


pthread_mutex_lock(&mutex4); 
 // cout<<"master count : "<<master_count<<" thread id : "<<thread_id1<<"\n"; 

//  cout<<"number of push operations in thread : "<<thread_id1<<" is : "<<number_of_push_operations<<"\n";

//  cout<<"number of lift operations in thread : "<<thread_id1<<" is : "<<number_of_lift_operations<<"\n\n";

for(k=0;k<wc.get();k++)
{
    pthread_cond_signal(&cond);
}
pthread_mutex_unlock(&mutex4); 

pthread_exit(NULL);
//  cout<<"exiting thread"<<"\n";
}         //end threadS




/*  Asyncronous global relabling heuristics */



void *agr_thread(void *tp)
{
  int Color[NoOfNodes]={0};
  int CurrentLevel;
  int *sink;
  int stash=0;
  int i=0;

//  cout<<"entred agr thread\n";

  sink = (int*)tp;
  queue<int>privQ;

//  cout<<"sink is : "<<*sink<<"\n";

  privQ.push(*sink);
  Color[*sink]=1;
  CurrentLevel=0;

  while(privQ.empty()==false)
  {
     stash=privQ.front();
     privQ.pop();
     CurrentLevel=CurrentLevel+1;
     for(i=0;i<p.G[stash].size();i++)
     {
      if(Color[p.G[stash][i].to]==0 && p.G[stash][i].to!=1)
      {
        Color[p.G[stash][i].to]=1;
        if(p.dist[p.G[stash][i].to]<CurrentLevel)
        {
          p.dist[p.G[stash][i].to]=CurrentLevel;
          
        }
        privQ.push(p.G[stash][i].to);
      }
     }
/*
     if(privQ.empty()==true)
     {
      cout<<"queue empty\n\n\n\n\n";
     }
*/
  }

privQ.push(1);
Color[1]=1;
  CurrentLevel=(NoOfNodes-1);

 // cout<<"discovered  vertices : ";
  while(privQ.empty()==false)
  {
     stash=privQ.front();
     privQ.pop();
     CurrentLevel=CurrentLevel+1;
     for(i=0;i<p.G[stash].size();i++)
     {
      if(Color[p.G[stash][i].to]==0 && p.G[stash][i].to!=*sink)
      {
        Color[p.G[stash][i].to]=1;
        if(p.dist[p.G[stash][i].to]<CurrentLevel)
        {
          p.dist[p.G[stash][i].to]=CurrentLevel;          
        }
        privQ.push(p.G[stash][i].to);
      }
     }
/*
     if(privQ.empty()==true)
     {
      cout<<"queue empty\n\n\n\n\n";
     }
*/
  }

for(i=1;i<NoOfNodes;i++)
{
  if(Color[i]==0)
  {
    p.dist[i]= std::numeric_limits<int>::max();
  }
}

//  cout<<"\n";
//  cout<<"AGR thread exiting\n\n\n";

pthread_exit(NULL);
}


int main()
{
  int a,ret;
  int i,j,k,l;     //iteratrors
  int thread_number=0;
  int co=0;
  struct timeval tim;

  fp = fopen("out4.txt", "r");
            if (fp == NULL) {
                    printf("Can't open input file!\n");
                    while(1);
                }
            if(fscanf(fp, "%d %d", &nodeCount, &edgeCount) != EOF)
               {
               printf("\n Node count and Edge count are %d %d", *nodeCount, *edgeCount);

               }

            while (fscanf(fp, "%d %d %d", &operation,&var1,&var2) != EOF)
               {      //printf("\nThe values are %d %d %d", *operation, *var1, *var2);
                             p.AddEdge(*operation, *var1, *var2);
               }

cout<<"\n\n node count : "<<*nodeCount;

parallel_initialize(1,*nodeCount);

 cout<<"back to main"<<"\n"; 
cout<<"adjacent nodes to source"<<"\n";

cout<<"node count : "<<*nodeCount<<"\n";

/*  Timing begins */

gettimeofday(&tim, NULL);
double t1=tim.tv_sec+(tim.tv_usec/1000000.0);

for(i=0;i<(NoOfThreads);i++)
{
   pthread_create(&t[i], NULL,thread,(void *) &p.returnexcess[i][p.returnexcess[i].size()-1]);
}

pthread_create(&heuristics, NULL,agr_thread,(void *) &nodeCount);

  for(i=0; i<(NoOfThreads); i++)
  {
      pthread_join(t[i], NULL);
  }

  pthread_join(heuristics,NULL);

gettimeofday(&tim, NULL);


/*  Timing ends */

 double t2=tim.tv_sec+(tim.tv_usec/1000000.0);
                        cout<<"\n Seconds elapsed "<< t2-t1;

  cout << "\n\nThreads terminated.. Back to main ";
cout<<"\nExcess of source is "<<p.excess[1].value<<"\n ";
cout<<"\nExcess of sink is "<<p.excess[*nodeCount].value<<"\n";
cout<<"\n";


 pthread_exit(NULL);
return 0;


}