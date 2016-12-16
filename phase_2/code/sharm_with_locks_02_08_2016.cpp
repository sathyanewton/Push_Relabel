#include <iostream>
#include <pthread.h>
#include <vector>
#include <cmath>
#include <queue>
#include <assert.h>
#include <algorithm>
#include <limits>

using namespace std;
typedef long long LL;

struct Edge  {
  int from, to, cap, flow, index;
  Edge(int from, int to, int cap, int flow, int index) :
     from(from), to(to), cap(cap), flow(flow), index(index) {}

};

struct PushRelabel {
  int N;
  vector<vector<Edge> > G;
  vector<LL> excess;
  vector<int> dist, active, count;
  queue<int> Q;
  vector<int> returnexcess;

  PushRelabel(int N) : N(N), G(N), excess(N), dist(N), active(N), count(2*N), returnexcess(N) {}

  void AddEdge(int from, int to, int cap) {
    G[from].push_back(Edge(from, to, cap, 0, G[to].size()));
    if (from == to) G[from].back().index++;
    G[to].push_back(Edge(to, from, 0, 0, G[from].size() - 1));
  }

  void initialization(int s, int t)
  {
      int i,j,k,l,m;
      for(i=1;i<N;i++)
               {
               excess[i] =    0;
               }
           for (j=1;j<N;j++)
           {    if(j!=s)
                {dist[j]=0;
                }
          }
        /*for(k=0;k<G[s].size();k++)  {
           excess[s]=excess[s]-G[s][k].cap;  } */
        cout <<"\nExcess of s initially is "<<excess[s]<<"\n";

        for(l=0; l<G[s].size(); l++) //this push from s to neighboring vertices happens just once - initially
        {
        //    cout<< "\n entering l loop";
            G[s][l].flow=G[s][l].cap;
            //G[s][l].cap=0;
            //cout << "\n Exiting loop ";
            G[G[s][l].to][G[s][l].index].flow=G[s][l].flow;
            //excess[G[s][l].to] =excess[G[s][l].to]+;
            //cout << "\n Wow done ";
            excess[G[s][l].to] = G[s][l].cap;
            excess[s]=excess[s]-G[s][l].cap;
            Enqueue (G[s][l].to);
            returnexcess[l]=G[s][l].to;
            cout << "\nValues in returnexcess are "<<returnexcess[l];
        }
        cout << "\nExcess of s value is "<<excess[s];
/*          for(m=0;m<G[s].size();m++)
          {     cout << "\nflow from s to node "<<G[s][m].to<<" is "<<G[s][m].flow;
              cout << "\ncapacity from s to node "<<G[s][m].to<<" is "<<G[s][m].cap;
              cout << "\nflow from " <<  G[s][m].to << " to node s is "<< G[G[s][m].to][s].cap << "\n";
              cout << "\nflow from " <<  G[s][m].to << " to node s is "<< G[G[s][m].to][G[s][m].index].flow << "\n";     } */
    //return (returnexcess);

  }
  void Enqueue(int v) {
      if (!active[v] && excess[v] > 0) { active[v] = true; Q.push(v); } //actually each thread should check this
     // so that the vertex may be active in other thread due to a push operation as well
    }
  void Discharge(int u)
{   cout << "\nEntered Discharge fn for node "<<u;
    long long amt;
     //   long long cap1=(long long)e.cap;
       // cap=17;
    LL excess_t=excess[u];
    int i, smallestHtVertex, dist2;
    int dist_t=std::numeric_limits<int>::max();
    for (i=0; i<G[u].size(); i++)                    //One condition missing here
        { cout << "\nEntered for loop when i = " <<i;
        // cout << "\nG[u][i] is "<<G[u][i].to;
        // cout << "\ndistance of variables is "<<dist[G[u][i].to];
        dist2 = dist[G[u][i].to];
        if (dist2 < dist_t)
        {cout <<"\n entering 'if' loop to check dist2 < dist_t";
        smallestHtVertex=G[u][i].to;
        dist_t=dist2;
        }
        }
    // do the push for (u, lowest v chosen by the above for loop)
if (dist[u]>dist_t)
{ cout << "\n Entering loop to check if dist[u] > dist_t";
    amt = min(excess_t,(long long)G[u][smallestHtVertex].cap);
                    cout << "\n min amount is " << amt;
            G[u][smallestHtVertex].cap=G[u][smallestHtVertex].cap-amt;
            cout<<"New capacity in the edge u "<< u <<" to "<<smallestHtVertex<<" is "<<G[u][smallestHtVertex].cap;
            G[smallestHtVertex][u].cap=G[smallestHtVertex][u].cap+amt;
            excess[u]=excess[u]-amt;
            excess[smallestHtVertex]=excess[smallestHtVertex]+amt;
}
else
    dist[u]=dist_t + 1;
cout << "\nDistance of node "<<u<<" is " << dist[u];
}

  LL GetMaxFlow(int s, int t)
  {
      cout << "\nEntered Getmax flow fn";
      int i=0;
     dist[s]=N;
     active[s]=active[t]=true;
     int v[10];
     //excess[s]=0;
   initialization(1,6);
  cout << "\n Returned after initialization";
  while (!Q.empty()) {
         v[i] = Q.front();
           Q.pop();
        active[v[i]] = false;  //this will
        cout << "\nAbout to call Discharge fn";
        cout << "\n value of active  of v["<<i<<"] is "<<v[i];
      i++;
          } return (*v);
 }
};
int n=5;

void *mythread(void *arg)
{
    int i;
    int *a=(int*)arg;
    cout << "In new thread\n";
    for(i=0;i<7;i++)
    {
        //a=a+4;
        a[i]=i;
        cout<<a[i]<<"\n";
    }
    //cout <<"trying to print the argument " <<arg<<"\n";
    //cout << n <<"\n";
    return NULL;
}

int a[10];
int main(int argc, char *argv[])
{  cout <<"Program starting";
    PushRelabel p(7);
        p.AddEdge( 1,2 ,15) ; p.AddEdge( 2,3 ,12) ;
        p.AddEdge( 3,6 ,7) ; p.AddEdge( 1,4 ,4) ;
        p.AddEdge( 4,5,10) ; p.AddEdge( 5,6,10) ;
        p.AddEdge( 2,5,5) ; p.AddEdge( 4,3,3) ; // DIRECTION OF EDGES DETERMINES THE MAXIMUM FLOW
        pthread_t p1;
//        int vertexarg[9];
        //char a='s';
        p.GetMaxFlow(1,6); //Initialization inside GetMaxFlow
                cout << "\n returned from GetMaxFlow, in main";
        cout << "\nthreads declared " <<"\n";
    pthread_create(&p1, NULL, mythread,(void *)a);
        //pthread_create(&p2, NULL, mythread, 0);
        pthread_join(p1, NULL);
//* Initialization of h(u), e(u), f(u,v)

        cout<<"\nExcess node ids after first push from s are "<<p.returnexcess[0]<<" "<<p.returnexcess[1];
        cout << "\nThank you";
        return 0;
}