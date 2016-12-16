/*
  if some stuff is same that is in phase 1 then we have not re-itraded that here.  
*/

#include <time.h>
#include <stdio.h>
#include <iostream>
#include <cmath>
#include <vector>
#include <queue>
#include <pthread.h>
#include <limits>


#define NoOfNodes 501 
#define NoOfThreads 4


using namespace std;

typedef long long LL;
/* Global variables */

  pthread_mutex_t lock;
  pthread_t t[NoOfThreads];   //thread variables

  /* mutex for syncronization */
  pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;   /*in place of atomic operation*/
  pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;  /* setting the thread id */
  pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;  /* incrementing the master counter*/
  pthread_mutex_t mutex3 = PTHREAD_MUTEX_INITIALIZER;  /* printing the master counter */    

int thread_id1=0;           /* global variable used to set thread_id variable */

        /* global variables */

          FILE *fp,*fp2;                  /*file pointers */ 
            int operation[5];             /*file counters*/
            int edgeCount[3];
            int nodeCount[3];

            int var1[3],var2[3];


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
  vector< vector <int> > returnexcess;           // becomes a 2 dimensional array when comapared to phase one

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


  PushRelabel p(NoOfNodes);


  void print_queue(std::queue<int>& queue1)   //printing queue let's see==>come back to it.
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
  p.excess[s]=0;


  for(i=(s+1);i<t;i++)
  {
    p.dist[i]=0;
    p.excess[i]=0;
  }
    for(l=0;l<p.G[s].size();l++)            //  from 0 to G[s].size() ==> number of adjacent edges to the source node.
    {
      p.G[s][l].flow = p.G[s][l].cap;         // flow of the edges from source is same as its capacity. 
            p.G[p.G[s][l].to][p.G[s][l].index].cap=p.G[s][l].flow;  // uses the "index" varilable to set the capacity of the reverse-edge as the flow             
            p.excess[p.G[s][l].to] = p.G[s][l].flow;      //  excess value of the nodes adjacent to the source is set to the flow through these edges
            p.excess[s] = (p.excess[s]-p.G[s][l].flow);     //  excess of source is old excess-flow through this iteration
            p.Enqueue (p.G[s][l].to);             // equeue the active edges
            
            /*Basic round robin scheduler*/
            /* this section of the code is responsibe for assigning nodes that are adjacent
               to the source. 
               this is done by pushing all the adjacent nodes to the source into a two diemensional array. 
               let's say we have 2,4,5,7,8,9,12,14,25,16 as the adjacent node to the source. 

               we have put these adjacent nodes into the returnexcess array which has become 2 dimensional. 

               [0][0]   2     [0][1]    8       [0][2]    25        
               [1][0]   4     [1][1]    9       [1][2]    16        
               [2][0]   5     [2][1]    12      
               [3][0]   7     [3][1]    14      


               what the next few lines does is it populates the array as mentioned above. 
                if condition is crucial for not allowing the source and sink into the returnexcess array
            */
            if (((p.G[s][l].to)!=s) && ((p.G[s][l].to)!=t))
            {
              cout<<"node "<<p.G[s][l].to<<" going to thread "<<l%NoOfThreads<<"\n";
            p.returnexcess[l%NoOfThreads].push_back(p.G[s][l].to);
            }       
      }

      /* Pushes the size of each row as the final element of the row */  
            for(r=0;r<NoOfThreads;r++)
            {
              p.returnexcess[r].push_back(p.returnexcess[r].size());
            }  

      /* Resizes just in case */       
            for(k=0;k<NoOfThreads;k++)
            { 
              p.returnexcess[k].resize(p.returnexcess[k].size());
            }          
}

/*
    input parameter void *tp points to p.returnexcess[thread id][size of the row in the returnexcess array]
    ==>*tp points to last but one'th element in that row of returnexcess array.
    if no nodes pointed by *tp. if *tp is a NULL pointer.
    ant becomes 0.
*/

void *thread(void *tp)
{
  cout<<"entered thread"<<"\n";


  int s,t,u,Qbuffer;        
  int i,j,k,l;     //iteratrors
  int Qsize=0;
  int e,v,h1,h2,d;    //used to find the edge of least height
  int flag=0;         /*to check if it is the first push operation inside the thread. 
                      Will make sure we know 
                       what is so special about the first push operation */
  int master_count=0;
  pthread_t tid = pthread_self();
  std::queue<int> privQ;           //private queue data structure  
  int thread_id=0;        //unique thread_id

s=1;                        //source node number is 1 (always)
t=*nodeCount;               //total number of nodes (sink node)

 
int *ant=(int*)tp;          //  stores the input pointer. type casted to int, and stored in ant
pthread_mutex_lock(&mutex1);  // mutex1 is locked 
thread_id1++;                 /*
                                uses a hack, not sure if it is totally fool proof, it has worked till now 
                                What happens here is a global variable(thread_id1) is incremented underlock 
                                idea is it gives a unique thread_id to each thread that is incremental in nature. 
                                thread_ids will be lets say for 4 threads : 1,2,3,4.
                                this thread_id1 is stored in thread_id. This is done because we can from then on 
                                keep using thread_id. Thread_id is a local variable.Hence each thread will have a
                                thread_id that is unique and incremental.
                                */
thread_id=thread_id1;
pthread_mutex_unlock(&mutex1);

//cout<<"t value : "<<t<<" thread id "<<thread_id<<"\n";


//print_queue(privQ);

  while((p.excess[s]+p.excess[t])<0)          // standard exit condition. excess is a global variable  
  {                                           // all threads will leave this while loop in fairly the same time. As exit condition is based on a global variable   
  int satsat=0,bbb=0;                         // satsat,bbb used to find the node that is of least adjacent to a given node.  
     if(flag==0)                              // if flag=0, this means that is the first push/relabel operation inside the while loop. 
   {                                          /*    [0][0]   2     [0][1]    8       [0][2]    25     */
        for(k=1; k<=*ant;k++)                 //  *ant points first to [0][3] and is decremented one by one(25--8--2).  
       {                                      //   each of these nodes are activated in the following statement
        p.active[*(ant-k)]=1;                 // we choose nodes one by one from the end of the row 
          // repetitve section of the code used to stop the sink from being activated.
          // ant is 0, if tp is NULL pointer   
          if(*(ant-k)==t)                       // mostly s or t wont come till this point as we have an if condition in the initialize function             
          {                                   
          cout<<"red alert!!!!!!!!!\n\n\n\n";
          while(1);
          }
        privQ.push(*(ant-k));                 // the aforementioned nodes are pushed inside the private queue
                                          //  at this point, we have active[all nodes adjacent to the source]=1;
       }                                    // all the neighbouring nodes to the source is pushed into the privQ.
   }                                      // for the above example we now have [2,4,5,7] in the privQ (say in thread 0,we have threads 0 to 3)
   else                               //second push and lift operation 
  {
      Qsize = privQ.size();                           
      for(l=0;l<Qsize;l++)
      {
        if(privQ.front()==t)
        {
          cout<<"red alert!!!!!!!!!\n\n\n\n";
          while(1);
        }

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
              pthread_mutex_lock(&mutex);         //start of atomic part 

              p.G[privQ.front()][smallVindex].cap  =  p.G[privQ.front()][smallVindex].cap-amt;
              p.G[smallestHtVertex][p.G[privQ.front()][smallVindex].index].cap  =  p.G[smallestHtVertex][p.G[privQ.front()][smallVindex].index].cap+amt;
              p.excess[privQ.front()] = p.excess[privQ.front()]-amt;
              p.excess[smallestHtVertex] = p.excess[smallestHtVertex]+amt;


              pthread_mutex_unlock(&mutex);              //end of atomic part 


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
pthread_mutex_lock(&mutex2);
//print_queue(privQ);.
if(privQ.empty()==true)
{
 cout<<"queue empty : thread id "<<thread_id<<"\n";
 print_queue(privQ);
  //  while(1);
}
master_count++;
pthread_mutex_unlock(&mutex2);
/*
if(master_count==500)
{
 // break;
}*/
}           //big while

pthread_mutex_lock(&mutex3);
cout<<"master count : "<<master_count<<" thread id : "<<thread_id<<"\n"; 
pthread_mutex_unlock(&mutex3);
pthread_exit(NULL);
cout<<"exiting thread"<<"\n";
}         //end threadS


int main()
{
  int a,ret;
  int i,j,k,l;     //iteratrors
  int thread_number=0;
int co=0;
  fp = fopen("out7.txt", "r");
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
for(i=0;i<NoOfThreads;i++)
{
pthread_create(&t[i], NULL,thread,(void *) &p.returnexcess[i][p.returnexcess[i].size()-1]);
}

  for(i=0; i<NoOfThreads; i++)
  {
      pthread_join(t[i], NULL);
  }

  cout << "\n\nThreads terminated.. Back to main ";
cout<<"\nExcess of source is "<<p.excess[1]<<"\n ";
cout<<"\nExcess of sink is "<<p.excess[*nodeCount]<<"\n";
cout<<"\n";

return 0;

 pthread_exit(NULL);
}