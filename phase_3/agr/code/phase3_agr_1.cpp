//#define _GNU_SOURCE
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

#define NoOfNodes 101
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
//  pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;   // thread id
//  pthread_mutex_t mutex3 = PTHREAD_MUTEX_INITIALIZER;   // master count 
  pthread_mutex_t mutex4 = PTHREAD_MUTEX_INITIALIZER;   // worksharing buffer 
  pthread_mutex_t mutex5 = PTHREAD_MUTEX_INITIALIZER; 
  pthread_cond_t  cond   = PTHREAD_COND_INITIALIZER;      
//  int thread_id1=0;

int Wave[NoOfNodes]={0};

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
  vector<int> dist, active;     
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
   // cout<<" print queue called \n";
      //cout<<"first value in a queue : "<<queue1.front();


    int i;
    int buff;
    int size;
    size = queue1.size();
    //cout<<"size : "<<size<<"\n";
   // cout<<"printing queue\n";
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

  cout<<"inside initialize"<<"\n";
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

            cout<<"initialize 2 "<<"\n";         
}

void *thread(void *tp)
{
  cout<<"entered thread"<<"\n";


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
 // int thread_id=0;
  int stash,work_sharing_size;
  int number_of_requests=0;

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



 
int *ant=(int*)tp;  
/*
pthread_mutex_lock(&mutex2);
thread_id1++;
thread_id=thread_id1;
pthread_mutex_unlock(&mutex2);
*/

thread_id.increment();

cout<<"entered thread : "<<thread_id.get()<<"\n";
//cout<<"p.excess[s].value+p.excess[t].value : "<<p.excess[s].value+p.excess[t].value<<"\n";

  while((p.excess[s].value+p.excess[t].value)<0)
  {


    /*  vertex exchange */
      
    if(privQ.empty()==true && flag==1)
    {
     cout<<"@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ queue empty : thread id "<<thread_id.get()<<"\n\n\n\n\n";
    number_of_requests++;

      /*  
      pthread_mutex_lock(&mutex1); 
      wc=wc+1;
      pthread_mutex_unlock(&mutex1);  
      */

      wc.increment();
   
pthread_mutex_lock(&mutex4);
 
            while(global_worksharing_buffer.empty()==true && (p.excess[s].value+p.excess[t].value)<0)
            {
                pthread_cond_wait(&cond,&mutex4);
               
               /* 
                if((p.excess[s].value+p.excess[t].value)>=0)
                {
                  global_worksharing_buffer.clear();
                  cout<<"recived condition from exit state-->thread id : "<<thread_id.get()<<"\n";
                  break;
                }*/
          

            }
            rc = global_worksharing_buffer.size();
            cout<<"vertex exchange happeneing in thread : "<<thread_id.get()<<"from thread "<<i<<"\n";
            for(m=0;m<rc;m++)
            {
              p.active[global_worksharing_buffer[m]]=1;
              privQ.push(global_worksharing_buffer[m]);
            }
            global_worksharing_buffer.clear();

 pthread_mutex_unlock(&mutex4); 

      /*
      pthread_mutex_lock(&mutex1); 
      wc=wc-1;
      pthread_mutex_unlock(&mutex1); 
      */ 
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
  cout<<"giving thread : "<<thread_id.get()<<"\n";
  }
    
else            //big big else 
{ 
  int satsat=0,bbb=0;
  if(flag==0)
   {
        for(k=1; k<=*ant;k++)     
       {                        
        p.active[*(ant-k)]=1;   // push 
          if(*(ant-k)==t||*(ant-k)==s)
          {
          cout<<"red alert!!!!!!!!!\n\n\n\n";
          while(1);
          }
        privQ.push(*(ant-k));
       }
       flag=1;
   }
   else                               //second push and lift operation --> big else 
  {
      Qsize = privQ.size();
      for(l=0;l<Qsize;l++)
      {
        if(privQ.front()==t||privQ.front()==s)
        {
          cout<<"red alert!!!!!!!!!\n\n\n\n";
          while(1);
        }

        if(p.excess[privQ.front()].value<=0)          //if the node's excess is less than 0. 
        {
          Qbuffer = privQ.front();
          privQ.pop();
          p.active[Qbuffer]=0;
          if(!p.active[Qbuffer])
          {
            p.active[Qbuffer]=1;
            /*check for sink*/
          if(Qbuffer==t||Qbuffer==s)
          {
          cout<<"red alert!!!!!!!!!\n\n\n\n";
          while(1);
          }
            privQ.push(Qbuffer);
          }
        }
        else              //  not the first push operation and excess[node] is greater than 0
        {
          
          imnt amt;
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

/*
             
              pthread_mutex_lock(&mutex);         //start of atomic part 

              p.G[privQ.front()][smallVindex].cap.value  =  p.G[privQ.front()][smallVindex].cap.value-amt;
              p.G[smallestHtVertex][p.G[privQ.front()][smallVindex].index].cap.value  =  p.G[smallestHtVertex][p.G[privQ.front()][smallVindex].index].cap.value+amt;
              p.excess[privQ.front()].value = (p.excess[privQ.front()].value)-amt;
              p.excess[smallestHtVertex].value = (p.excess[smallestHtVertex].value)+amt;

              pthread_mutex_unlock(&mutex);              //end of atomic part 
*/
    
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
                  if(p.G[satsat][smallVindex].to==t||p.G[satsat][smallVindex].to==s)
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
                  if(satsat==t||satsat==s)
                  {
                  cout<<"red alert!!!!!!!!!\n\n\n\n";
                  while(1);
                  }
                    privQ.push(satsat);
                }

            }
            else          // height of current node is < height of least heightt adjacent node 
            {
                bbb=privQ.front();
                privQ.pop();
                p.active[bbb]=0;
                p.dist[bbb] = dist_t + 1;
                if(!p.active[bbb])
                {
                  p.active[bbb]=1;
                  if(bbb==t||bbb==s)
                  {
                  cout<<"red alert!!!!!!!!!\n\n\n\n";
                  while(1);
                  }
                  privQ.push(bbb);
                }
            }

        }

      }       // big for closing braces 


   }        //big else 
// flag=1;
 }    // big big else 

master_count++;

/*
if(master_count==500)
{
 // break;
}*/

}           //big while


//cout<<"excess[s] "<<p.excess[s]<<"excess[t] "<<p.excess[t]<<"in thread "<<thread_id.get()<<"\n";

pthread_mutex_lock(&mutex4); 
cout<<"master count : "<<master_count<<" thread id : "<<thread_id.get()<<"\n"; 
//cout<<"number of requests raised : "<<number_of_requests<<"thread id : "<<thread_id.get()<<"\n";
//cout<<"wc count : "<<wc.get()<<" in thread : "<<thread_id.get()<<"\n";
for(k=0;k<wc.get();k++)
{
    pthread_cond_signal(&cond);
}
pthread_mutex_unlock(&mutex4); 

pthread_exit(NULL);
cout<<"exiting thread"<<"\n";
}         //end threadS




/*  Asyncronous global relabling heuristics */



void *agr_thread(void *tp)
{
  int Color[NoOfNodes]={0};
//  int CurrentWave;
  int CurrentLevel;
  int *sink;
  int stash=0;
  int i=0;
  int node_counter=0;


  int tid = pthread_self();

cpu_set_t cpuset1, cpuset2 ;
CPU_ZERO(&cpuset1);
CPU_ZERO(&cpuset2);

pthread_mutex_lock(&mutex5);
CPU_SET((cpuvar.get())%8, &cpuset1);
pthread_mutex_unlock(&mutex5);
thread_stuff = pthread_setaffinity_np(tid, sizeof(cpu_set_t), &cpuset1);
  if (thread_stuff != 0)
  {
      cout<<"thread binding not happeneing"<<"\n";

  }

  cout<<"entred agr thread\n";



  sink = (int*)tp;
  queue<int>privQ;
  CurrentWave=CurrentWave+1;

  cout<<"sink is : "<<*sink<<"\n";

  privQ.push(*sink);
  CurrentLevel=0;

   //  p.print_adj_list(1,*sink);

  while(privQ.empty()==false)
  {
     stash=privQ.front();
     privQ.pop();
     CurrentLevel=CurrentLevel+1;
     for(i=0;i<p.G[stash].size();i++)
     {
      if(Color[p.G[stash][i].from]==0)
      {
        Color[p.G[stash][i].from]=1;
        if(p.dist[p.G[stash][i].from]<CurrentLevel)
        {
          p.dist[p.G[stash][i].from]==CurrentLevel;
          
        }
        //Wave[p.G[stash][i].from]=CurrentWave;
        privQ.push(p.G[stash][i].from);
      }
     }

     if(privQ.empty()==true)
     {
      cout<<"queue empty\n\n\n\n\n";
     }
     cout<<"CurrentWave : "<<CurrentWave<<"\n";
     node_counter++;
  }

for()


privQ.push(1);
  CurrentLevel=(NoOfNodes-1);

     p.print_adj_list(1,*sink);

  while(privQ.front()!=*sink)
  {
     stash=privQ.front();
     privQ.pop();
     CurrentLevel=CurrentLevel+1;
     for(i=0;i<p.G[stash].size();i++)
     {
      if(Color[p.G[stash][i].to]==0)
      {
        Color[p.G[stash][i].to]=1;
        if(p.dist[p.G[stash][i].to]<CurrentLevel)
        {
          p.dist[p.G[stash][i].to]=CurrentLevel;
          
        }
        //Wave[p.G[stash][i].to]=CurrentWave;
        privQ.push(p.G[stash][i].to);
      }
     }

     if(privQ.empty()==true)
     {
      cout<<"queue empty\n\n\n\n\n";
     }
     //cout<<"CurrentWave : "<<CurrentWave<<"\n";
     node_counter++;
  }


  cout<<"node count : "<<node_counter<<"\n";
  cout<<"AGR thread exiting\n\n\n";

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

cout<<"\n\n\n\n\n\n\n\n\n\n node count : "<<*nodeCount;

parallel_initialize(1,*nodeCount);

 cout<<"back to main"<<"\n"; 
cout<<"adjacent nodes to source"<<"\n";

cout<<"node count : "<<*nodeCount<<"\n";

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

 double t2=tim.tv_sec+(tim.tv_usec/1000000.0);
                        cout<<"\n Seconds elapsed "<< t2-t1;

  cout << "\n\nThreads terminated.. Back to main ";
cout<<"\nExcess of source is "<<p.excess[1].value<<"\n ";
cout<<"\nExcess of sink is "<<p.excess[*nodeCount].value<<"\n";
cout<<"\n";


 pthread_exit(NULL);
return 0;


}