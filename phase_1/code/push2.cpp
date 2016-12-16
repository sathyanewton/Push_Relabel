#include <time.h>
#include <iostream>
#include <cmath>
#include <vector>
#include <iostream>
#include <queue>
#include <pthread.h>

#define NoOfNodes 7

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

  PushRelabel(int N) : N(N), G(N), excess(N), dist(N), active(N){}

  void AddEdge(int from, int to, int cap) {
    G[from].push_back(Edge(from, to, cap, 0, G[to].size()));
    if (from == to) G[from].back().index++;
    G[to].push_back(Edge(to, from, 0, 0, G[from].size() - 1));
  }

void Enqueue(int v) {
    if (!active[v] && excess[v] > 0) { active[v] = true; Q.push(v); }
  }

  void Push(Edge &e) {
    int amt = int(min(excess[e.from], LL(e.cap - e.flow)));
    if (dist[e.from] <= dist[e.to] || amt == 0) return;
    e.flow += amt;
    G[e.to][e.index].flow -= amt;
    excess[e.to] += amt;
    excess[e.from] -= amt;
    Enqueue(e.to);
  }

  void Relabel(int v) {
    //count[dist[v]]--;
    dist[v] = 2*N;
    for (int i = 0; i < G[v].size(); i++)
      if (G[v][i].cap - G[v][i].flow > 0)
	dist[v] = min(dist[v], dist[G[v][i].to] + 1);
    //count[dist[v]]++;
    Enqueue(v);
  }

  void Discharge(int v) {
    for (int i = 0; excess[v] > 0 && i < G[v].size(); i++) Push(G[v][i]);
    if (excess[v] > 0) {
	Relabel(v);
    }
}

  int GetMaxFlow(int s, int t) {
  	int a=0;
   // count[0] = N-1;
    // count[N] = 1;
    dist[s] = N;
    active[s] = active[t] = true;
    for (int i = 0; i < G[s].size(); i++) {
      excess[s] += G[s][i].cap;
      Push(G[s][i]);
    }

    while (!Q.empty()) {
      int v = Q.front();
      Q.pop();
      active[v] = false;
      Discharge(v);
    }

    for (int i = 0; i < G[s].size(); i++) 
	{
    	a += G[s][i].flow;
  }
    	return a;
}

 void print_graph(int s,int t)
  {
      for(int i=s;i<t;i++)
      {
        cout<<i;
        for(int j=0;j<G[i].size();j++)
        {
          cout<<"->"<<G[i][j].to;
        }
        cout<<"\n";
      }
  }
};
  
int main()
{
	int a;

  PushRelabel p(7);

	p.AddEdge(1,2,15);
	p.AddEdge(1,4,4);
	p.AddEdge(2,3,12); 
	p.AddEdge(4,5,10);
	p.AddEdge(2,5,5);
	p.AddEdge(4,3,3);
	p.AddEdge(3,6,7);
	p.AddEdge(5,6,10);

	a=p.GetMaxFlow(1,6);
  p.print_graph(1,6);

    cout << "\n Max flow is " << a <<"\n";

    //cout<<"g.size"<<G[0][0].index<<"\n";

	return 0;
}

  
