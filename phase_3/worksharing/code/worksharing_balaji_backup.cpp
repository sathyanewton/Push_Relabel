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
/* Global variables */

/* Thread-based global variables   */
  pthread_mutex_t lock;
  pthread_t t[NoOfThreads];
  pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;    // critical operations 
  pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;  // thread id and work sharing 
  pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER; 
  pthread_mutex_t mutex3 = PTHREAD_MUTEX_INITIALIZER; 
  pthread_mutex_t mutex4 = PTHREAD_MUTEX_INITIALIZER; 
  pthread_mutex_t mutex5 = PTHREAD_MUTEX_INITIALIZER; 
  pthread_cond_t  cond   = PTHREAD_COND_INITIALIZER;      
  int thread_id1=0;


  /*  File hadelling global variables */

          FILE *fp,*fp2;
            int operation[5];
            int edgeCount[3];
            int nodeCount[3];

            int var1[3],var2[3];



/*  Work sharing global variables */

  bool working[NoOfThreads+1];
  bool request[NoOfThreads+1];
  bool dirty[NoOfThreads+1];
  vector<int> global_worksharing_buffer;            

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
  //cout<<"entered thread"<<"\n";


  int s,t,u,Qbuffer;
  int i,j,k,l,m;     //iteratrors.
  int rc=0;
  int Qsize=0;
  int e,v,h1,h2,d;
  int flag=0;     //to check if it is the first push operation 
  int master_count=0;
  pthread_t tid = pthread_self();     
  std::queue<int> privQ;           //private queue data structure  
  int thread_id=0;

s=1;
t=*nodeCount;

 
int *ant=(int*)tp;  
pthread_mutex_lock(&mutex2);
thread_id1++;
thread_id=thread_id1;
pthread_mutex_unlock(&mutex2);
working[thread_id]=1;
cout<<"entered thread : "<<thread_id<<"\n";


  while((p.excess[s]+p.excess[t])<0)
  {
    /*  vertex exchange */
      
    if(privQ.empty()==true && flag==1)
    {
     cout<<"queue empty : thread id "<<thread_id<<"\n\n\n\n\n";

     i=thread_id+1;
     if(i>NoOfThreads)
     {
        i=1;
     } 
    pthread_mutex_lock(&mutex1); 

    if(working[i]==1 && request[i]!=1 && dirty[i]==0) 
    {     
      request[i]=1;
            /*use conditional variable here*/
       
       cout<<"empty thread : "<<thread_id<<" setting the request in : "<<i<<"\n"; 

       /* think*/
            
            while(global_worksharing_buffer.empty()==true)
            {
                pthread_cond_wait(&cond,&mutex1);
            }
            rc = global_worksharing_buffer.size();
            cout<<"vertex exchange happeneing in thread : "<<thread_id<<"from thread "<<i<<"\n";
            for(m=0;m<rc;m++)
            {
              p.active[global_worksharing_buffer[m]]=1;
              privQ.push(global_worksharing_buffer[m]);
            }
            global_worksharing_buffer.clear(); 
              
    }

    pthread_mutex_unlock(&mutex1);       
  }

  
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
   }
   else                               //second push and lift operation 
  {
      Qsize = privQ.size();
      for(l=0;l<Qsize;l++)
      {
        if(privQ.front()==t||privQ.front()==s)
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
            else          //if ((smallestHtVertex!=1)&(smallestHtVertex!=6))
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
      
      master_count++;
      if(master_count==8)
      {
       // break;
      }


   }        //big else 
flag=1;
pthread_mutex_lock(&mutex5);
//print_queue(privQ);.
master_count++;
//print_queue(privQ);
pthread_mutex_unlock(&mutex5);
/*
if(master_count==500)
{
 // break;
}*/
  /* work sharing  */
int stash,work_sharing_size;
if(request[thread_id]==1)
{
  pthread_mutex_lock(&mutex1);
  work_sharing_size = privQ.size();
  work_sharing_size=work_sharing_size/2;
  dirty[thread_id]=1;
  for(j=0;j<work_sharing_size;j++)
  {
  stash = privQ.front();
  privQ.pop();
  global_worksharing_buffer.push_back(stash);
  }
  cout<<"giving thread : "<<thread_id<<"\n";
  request[thread_id]=0;
  pthread_mutex_unlock(&mutex1);
  pthread_cond_signal(&cond);
  
}

}           //big while

//pthread_mutex_lock(&mutex1);
cout<<"master count : "<<master_count<<" thread id : "<<thread_id<<"\n"; 
//pthread_mutex_unlock(&mutex1);
pthread_exit(NULL);
cout<<"exiting thread"<<"\n";
}         //end threadS


int main()
{
  int a,ret;
  int i,j,k,l;     //iteratrors
  int thread_number=0;
int co=0;
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