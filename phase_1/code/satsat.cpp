#include <time.h>
#include <iostream>
#include <cmath>
#include <vector>
#include <iostream>
#include <queue>
#include <pthread.h>


#define NoOfNodes 7
#define NoOfThreads 2

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
  vector<int> returnexcess;

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
/*
  void print_index()
  {
    for(int i=s;i<=6;i++)
    {
      for(int j=0;j<G[i].size();j++)
      {
      cout<<"node from -> "<<G[i][j].from<<"to -> "<<G[i][j].to<<" index -> "<<G[i][j].index;
      }
      cout<<"\n";
    }
  }

*/
  void Enqueue(int v) {
    if (!active[v] && excess[v] > 0)
      {
        cout<<"inside enqueue0\n";
        active[v] = true;
        cout<<"inside enqueue1\n";
        Q.push(v);
        cout<<"inside enqueue2\n";
      }
  }
};
  //pthread_mutex_t lock;
  pthread_t t[6];
  PushRelabel p(NoOfNodes);
  pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
 



void parallel_initialize(int s, int t)
{
  int m,n,d,i,j;                                    //index adjustment

  cout<<"inside initialize"<<"\n";
  p.dist[s]=NoOfNodes;                                //setting height of source as no.of nodes


  for(i=s;i<=p.active.size();i++)
  {
    p.active[i]=0;
    cout<<"p.active["<<i<<"]"<<" = "<<p.active[i]<<"\n";    //printing active[i]
  }
  p.active[s]=p.active[t]=true;                       // s and t became active

  cout<<"active of s = "<<p.active[s]<<"\n";
  cout<<"active of t = "<<p.active[t]<<"\n";
  cout<<"p.dist["<<s<<"]"<<" = "<<p.dist[s]<<"\n";
  p.excess[s]=0;
  cout<<"p.excess["<<s<<"]"<<" = "<<p.excess[s]<<"\n";

  for(i=(s+1);i<t;i++)
  {
    p.dist.push_back(0);
    cout<<"p.dist["<<i<<"]"<<" = "<<p.dist[i]<<"\n";
    p.excess.push_back(0);
    cout<<"p.excess["<<i<<"]"<<" = "<<p.excess[i]<<"\n";
    p.returnexcess.push_back(0);
    cout<<"p.returnexcess["<<i<<"]"<<" = "<<p.returnexcess[i]<<"\n";
  }



/*
      for(i=(s);i<=t;i++)
      {
        for(j=0;j<p.G[i].size();j++)
        {
          p.G[i][j].flow=0;
           //cout<<"p.G["<<i<<"]["<<j<<"].flow"<<" = "<<p.G[i][j].flow<<"\n";
        }
       // cout<<"\n";
      }
*/
      for(i=0;i<p.G[s].size();i++)
      {
        cout<<"inside main chunk of parallel_initialize\n";
        p.G[s][i].flow=p.G[s][i].cap;      //first push operation's flow 
        p.G[p.G[s][i].to][p.G[s][i].index].flow=p.G[s][i].cap;  // capacity of neighbouring nodes of the source
        p.excess[p.G[s][i].to]=p.G[s][i].cap;     //excess of the neighbouring nodes
        p.excess[1]=p.excess[1] - p.G[s][i].cap;  //excess of "s" is -(sum(all outgoing capacities))
        cout<<"source push"<<" "<<p.G[s][i].flow<<" "<<p.excess[i]<<"\n";
       p.Enqueue(p.G[s][i].to);
       cout<<"after enqueue"<<"\n";
       p.returnexcess[i]=p.G[s][i].to;
    cout << "\nValues in returnexcess["<<i<<"] = "<<p.returnexcess[i]<<"\n";
     

      //scheduler();
      //cout<<"POPULATING BUFF\n";
     // cout<<"pointless cout\n";
/*
        if (((p.G[s][i].to)!=s) && ((p.G[s][i].to)!=t))
        {
          cout<<"inside if";
            buffer[i%NoOfThreads].push_back(p.G[s][i].to);
        }
   */
       
   }   
     p.returnexcess.resize(p.G[s].size());
        cout << "\nsize of returnexcess inside initialization is "<<p.returnexcess.size();
        cout << "\nExcess of s value is "<<p.excess[s]<<"\n"; 
      //print_buffer();
            cout<<"PRINTING BUFF\n";
    for(int i=0;i<p.G[s].size();i++)
    {
      //cout<<p.buffer[s][i]<<"\n";
    }

/* basic round robin scheduler
      if (((p.G[s][i].to)!=s) && ((p.G[s][i].to)!=t))
            buffer[i%NoOfThreads].push_back(G[s][i].to); */
}

struct thread_parameters
{
  int x,y,z;

};


void *thread(void *tp)
{
  thread_parameters *tp1;
  tp1= (thread_parameters *)tp;
  cout<<"Entered thread"<<tp1->z<<"\n";
 // pthread_mutex_lock(&mutex);
  int s,t,u,Qsize,Qbuffer;
  s=tp1->x;         //x = s
  t=tp1->y;       //y = t
  u=tp1->z;      //number of thread
  int e,v,h1,h2,d;
  int flag=0;     //to check if it is the first push operation
  int satsat,bbb;
  int master_count=0;
  cout<<"thread parameters of "<<u<<" -> "<<s<<' '<<t<<' '<<u<<"\n";
  //pthread_mutex_unlock(&mutex);
   pthread_t tid = pthread_self();
   std::queue<int> privQ;           //private queue data structure 
//  cout<<s<<" "<<t<<" "<<u<<" "<<tid<<"\n";

   int i,j,k,l;     //iteratrors

  while((p.excess[1]+p.excess[6])<0)
  {
   cout<<"inside while in thread "<<u<<"\n";
   cout <<"Excess of 1 is "<<p.excess[1]<<"\n";

   if(flag==0)
   {
      cout<< "inside flag==0 loop"<<"\n";
      for(k=0;k<p.returnexcess.size();k++)
      {
        p.active[p.returnexcess[k]]=1;
        cout<<"\n active values of node " <<p.returnexcess[k]<<" is "<<p.active[p.returnexcess[k]];
        privQ.push(p.returnexcess[k]);
      }
      flag=1;
   }

   else                               //second push and lift operation
   {
      Qsize = privQ.size();
      cout <<"PrivQ.size inside the 'else' case of for loop is "<<Qsize<<"\n";
      for (l=0;l<Qsize;l++)
      {
        if(p.excess[privQ.front()]<=0)          //if the node's excess is less than 0.
        {
          cout << "Excess value is <= 0 value of privQ.front is "<< privQ.front()<<"\n";
          Qbuffer = privQ.front();
          privQ.pop();
          p.active[Qbuffer]=0;
          cout << "privQ's size before a push operation : "<<privQ.size()<<" privQ's front "<< privQ.front()<<"\n";
          if(!p.active[Qbuffer])
           {
            p.active[Qbuffer]=1;
            privQ.push(Qbuffer);
           }
           cout << "privQ's size after a push operation : "<<privQ.size()<<" privQ's front "<< privQ.front()<<"\n";
        }
        else              //  not the first push operation and excess[node] is greater than 0
        {
          cout<<"In thread "<<u<<"this is not the first push operation and excess[privQ.front] is greater than 0 ";
          cout << "Entered Discharge fn for node "<<privQ.front()<<"\n";

          LL amt;
          LL excess_t=p.excess[privQ.front()];

          cout << "Excess of node "<<privQ.front()<< " is "<<excess_t<<"\n";
          int smallVindex, smallestHtVertex, dist2,i;
          int dist_t=2147483647;

          // at this point we have to find, adjacent vertex which has the least height, to direct the next push operation.

          for (i=0; i<p.G[privQ.front()].size(); i++)  
            {
            if ((p.G[privQ.front()][i].cap)<=0) continue;
            cout << "\nEntered for loop when i = " <<i<<"\n";
              dist2 = p.dist[p.G[privQ.front()][i].to];    // h" is dist2
            if (dist2 < dist_t)
            {
            cout <<"\n entering 'if' loop to check dist2 < dist_t\n";
            smallestHtVertex=p.G[privQ.front()][i].to;    // v' is smallestHtVertex
            smallVindex=i;
            cout << "\n ------- Value of smallest ht vertex is "<<smallestHtVertex<<"\n";
            dist_t=dist2;                   // height of the lowest height vertex
            }
            }

            // do the push for (u, lowest v chosen by the above for loop)

            if(p.dist[privQ.front()]>dist_t)
            {
              /* Clarify */
              cout << "\nGoing to execute push-----Entering loop to check if dist[u] > dist_t\n";
              cout<<"excess is "<<excess_t<<"\n";
              amt = min(excess_t,(long long)p.G[privQ.front()][smallVindex].cap);
              cout << "\nMin amount is " << amt<<"\n";
              p.G[privQ.front()][smallVindex].cap  =  p.G[privQ.front()][smallVindex].cap-amt;

              p.G[smallestHtVertex][p.G[privQ.front()][smallVindex].index].cap  =  p.G[smallestHtVertex][p.G[privQ.front()][smallVindex].index].cap+amt;
              p.excess[privQ.front()] = p.excess[privQ.front()]-amt;
              p.excess[smallestHtVertex] = p.excess[smallestHtVertex]+amt;

              satsat = privQ.front();
               privQ.pop();
               p.active[satsat]=0;

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
                cout << "\nDistance of node "<<bbb<<" is " << p.dist[bbb]<<"\n";
                if(!p.active[bbb])
                {
                  p.active[bbb]=1;
                  privQ.push(bbb);
                }
            }
            cout<<"\n Size of privQ is " <<privQ.size();
            //break;
        }
        //flag=1;
        //break;

      }
        master_count++;
      if(master_count==36)
      {
      break;
      }
   }
  flag=1;
}
pthread_exit(NULL);
cout<<"exiting thread"<<"\n";
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

//p.print_index();z

cout<<"inverse coming up";

thread_parameters args;
 

cout<<"after args pass\n";


parallel_initialize(1,6);
cout << "\nReturned after initialization";
        cout<<"\n return excesses are "<<p.returnexcess[0]<<" "<<p.returnexcess[1]<<"\n";

for(int i=1;i<=NoOfThreads;i++)
{
 
      args.x = 1;
    args.y = 6;
  args.z=i;
  cout<<"creation for loop\n";
  cout<<args.z<<"\n";
  pthread_create(&t[i], NULL, thread,(void *)&args);

}
 pthread_join(t[1], NULL);

  cout << "\n\nThreads terminated.. Back to main ";
for(int i=1;i<6;i++)
{
  //cout<<p.dist[i]<<"\n";
}
cout<<"\nExcess of source is "<<p.excess[1];
         cout<<"\n Distance of [3] is "<<p.dist[3];
         cout<<"\n Distance of [2] is "<<p.dist[2];

return 0;

 pthread_exit(NULL);
}



/*
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
  */