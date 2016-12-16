#include <time.h>
#include <iostream>
#include <cmath>
#include <vector>
#include <iostream>
#include <queue>
#include <pthread.h>


#define NoOfNodes 7
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
  //vector<int> buffer;

  PushRelabel(int N) : N(N), G(N), excess(N), dist(N), active(N){}

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

  void print_index()
  {
    for(int i=1;i<=6;i++)
    {
      for(int j=0;j<G[i].size();j++)
      {
      cout<<"node from -> "<<G[i][j].from<<"to -> "<<G[i][j].to<<" index -> "<<G[i][j].index;
      }
      cout<<"\n";
    }
  }


};
  //pthread_mutex_t lock;
  pthread_t t[6];
  PushRelabel p(7);
  pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
  vector< vector<int> > buffer (7);

void Enqueue(int v) {
    if (!p.active[v] && p.excess[v] > 0) 
      { 
        p.active[v] = true; 
        p.Q.push(v); 
      }
  }

void parallel_initialize(int s, int t)
{
  int m,n,d;                                    //index adjustment

  cout<<"inside initialize"<<"\n";
  p.dist[s]=6;                        //setting height of source as no.of nodes 
  cout<<"p.dist[s]"<<p.dist[s]<<"\n";
  p.excess[s]=0;
  cout<<"p.excess[s]"<<p.excess[s]<<"\n";
  for(int i=(s+1);i<=t;i++)
  {
    p.dist.push_back(0);
    cout<<"p.dist["<<i<<"]"<<p.dist[i]<<"\n";
    p.excess.push_back(0);
    cout<<"p.excess["<<i<<"]"<<p.excess[i]<<"\n";
  }

      for(int i=(s);i<=t;i++)
      {
        for(int j=0;j<p.G[i].size();j++)
        {
          p.G[i][j].flow=0;
           //cout<<"p.G["<<i<<"]["<<j<<"].flow"<<" = "<<p.G[i][j].flow<<"\n";
        }
        cout<<"\n";
      }

      for(int i=0;i<p.G[s].size();i++)
      {
        p.G[s][i].flow=p.G[s][i].cap;
        p.G[p.G[s][i].to][p.G[s][i].index].flow=p.G[s][i].cap;
        p.excess[p.G[s][i].to]=p.G[s][i].cap;
        p.excess[1]=p.excess[1] - p.G[s][i].cap;
        cout<<"source push"<<" "<<p.G[s][i].flow<<" "<<p.excess[i]<<"\n";
       //Enqueue(p.G[s][i].to);
       
      cout<<p.excess[s]<<"\n";

      //scheduler();
      cout<<"POPULATING BUFF\n";
       
        if (((p.G[s][i].to)!=s) && ((p.G[s][i].to)!=t))
        {
          cout<<"inside if\n";
            buffer[i%NoOfThreads].push_back(p.G[s][i].to);

        }
        }
          
      //print_buffer();
    cout<<"PRINTING BUFF\n";
    for(int i=0;i<p.G[s].size();i++)
    {
     // cout<<buffer[s][i]<<"\n";
    }
    for(int i=0;i<NoOfThreads;i++)
    {
      buffer[i].resize(buffer[i].size());
    }
    for(int i=0;i<NoOfThreads;i++)
    {
       buffer[i].push_back(buffer[i].size());
    }
    //print whole print_buffer 
    /* The buffer round robins all the nodes connected to the source among number of 
    threads from a pre-defined macro. To be iterated over the indices of the buffer and 
    threads shall be spawned with the last but one value of each of these indices as parameters.*/                              
    for(int i=0;i<NoOfThreads;i++)
    {
      cout<<i<<" -> ";
      for(int j=0;j<buffer[i].size();j++)
      {
        cout<<buffer[i][j]<<" -> ";
      }
      cout <<"\n";
    }
}

struct thread_parameters
{
  int x,y,z;

};

struct thread_parameters1
{
  int x;

};

void *thread(void *tp)
{
  
  pthread_mutex_lock(&mutex);
  int s,t,u;
  thread_parameters *tp1;
  tp1= (thread_parameters *)tp;
  s=tp1->x;
  t=tp1->y;
  u=tp1->z;
  int e,v,h1,h2,d;

  cout<<"thread parameters of "<<u<<" -> "<<s<<' '<<t<<' '<<u<<"\n";
  pthread_mutex_unlock(&mutex);
   pthread_t tid = pthread_self();

//  cout<<s<<" "<<t<<" "<<u<<" "<<tid<<"\n";

  while((p.excess[s]+p.excess[t])<0)
  {
   // cout<<"inside while in thread "<<u<<"\n";
    if(p.excess[u]>0)
    {
      cout<<"inside if in thread "<<u<<"\n";
      e=p.excess[u];
      h1=1000;
      for(int i=0;i<p.G[u].size();i++)        //global V=i inside the for-loop
      {
        h2=p.dist[i];
        if(h2<h1)
        {
          v=(i);
          h1=h2;
        }
      }
      if(p.dist[u]>h1)
      {
        d=min(e,(p.G[u][v].cap-p.G[u][v].flow));

        //accquire mutex
        pthread_mutex_lock(&mutex);

        p.G[u][v].flow = (p.G[u][v].flow - d); 
        p.G[v][u].flow=(p.G[v][u].flow  + d);
        p.excess[u]=p.excess[u] - d;
        p.excess[v] = p.excess[v] + d;

       pthread_mutex_unlock(&mutex);

      }
      else
      {
        p.dist[u] = h1+1;
      }
    }
  }
  
  

  return NULL;
}

void *thread1(void *tp1) //thread's input parameters, pointer to the last but one element in each of the 
{                       //indices in the buffer. 
  pthread_mutex_lock(&mutex);
  int s,t,u,ref_cout=0,qlen=0,element_to_be_popped;

  thread_parameters1 *tp1;
  tp1= (thread_parameters1 *)tp;
  n=tp1->x;
 // t=tp1->y;
 // u=tp1->z;
  int e,v,h1,h2,d;
  queue<int> p_Q;

  while((p.excess[s]+p.excess[t])<0)
  {
      if(ref_count1==0)
      {
        for(int i=1;i<n;i++)
        {
          p.active[n-i]=1;
           p_Q.push(n-i);
        }
      }
      else
      {
          qlen=p_Q.size();
          for(int i=0;i<qlen;i++)
          {
            if(p.excess[p_Q.front()]<=0) 
            {
                
            }
          }
      }
  }


}



int main()
{
  int a,ret;


  //struct thread_parameters args;

  p.AddEdge(1,2,15);
  p.AddEdge(1,4,4);
  p.AddEdge(2,3,12); 
  p.AddEdge(4,5,10);
  p.AddEdge(2,5,5);
  p.AddEdge(4,3,3);
  p.AddEdge(3,6,7);
  p.AddEdge(5,6,10);

cout<<"print_graph\n";
//p.print_graph(1,6);
p.print_adj_list(1,6);

//p.print_index();

cout<<"inverse coming up";

thread_parameters args;
  

cout<<"after args pass\n";


parallel_initialize(1,6);

//version 1 pthread create calling thread
/*
for(int i=1;i<NoOfThreads;i++)
{ 
 
      args.x = 1;
    args.y = 6;
  args.z=i;
  cout<<"creation for loop\n";
  cout<<args.z<<"\n";
  pthread_create(&t[i], NULL, thread,(void *)&args);

}
*/
//version 2 calls thread1 
for(int i=0;i<NoOfThreads;i++)
{
  args.x=i;
  
    pthread_create(&t[z], NULL,(void *)&args;
      
      // mythread,(void *)&p.buffer[i][(p.buffer[i].size())-1]
}

for(int i=1;i<6;i++)
{
  cout<<p.dist[i]<<"\n";
}
return 0;
}
