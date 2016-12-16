#include <time.h>
#include <iostream>
#include <cmath>
#include <vector>
#include <iostream>
#include <queue>
//#include "denseacyclicgraph.h"
using namespace std;
typedef long long LL;

#define NoOfNodes 7
FILE *fp,*fp2;
            int operation[5];
            int edgeCount[3];
            int nodeCount[3];
            int var1[3],var2[3];

  struct Edge  {
  int from, to, cap, flow, index;
  Edge(int from, int to, int cap, int flow, int index) :
     from(from), to(to), cap(cap), flow(flow), index(index) {}

};

struct PushRelabel
{
  int N;
  vector<vector<Edge> > G;
  vector<LL> excess;
  vector<int> dist, active, count;
  queue<int> Q;

  PushRelabel(int N) : N(N), G(N), excess(N), dist(N), active(N), count(2*N) {}

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
void Display(Edge &e)
  {

if (e.flow>0)
    cout<< " \n The flow in edge (" << e.from<< ","<<e.to<<") is "<<e.flow ;

  }

  void Relabel(int v) {
    count[dist[v]]--;
    dist[v] = 2*N;
    for (int i = 0; i < G[v].size(); i++)
      if (G[v][i].cap - G[v][i].flow > 0)
	dist[v] = min(dist[v], dist[G[v][i].to] + 1);
    count[dist[v]]++;
    Enqueue(v);
  }

  void Discharge(int v) {
    for (int i = 0; excess[v] > 0 && i < G[v].size(); i++) Push(G[v][i]);
    if (excess[v] > 0) {
	Relabel(v);
    }
  }

  LL GetMaxFlow(int s, int t) {
    count[0] = N-1;
    count[N] = 1;
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

    LL totflow = 0;
    for (int i = 0; i < G[s].size(); i++) totflow += G[s][i].flow;
    return totflow;


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
/* run this program using the console pauser or add your own getch, system("pause") or input loop */
int main() {
	


	PushRelabel p(NoOfNodes);
	p.AddEdge(1,2,15);
	p.AddEdge(1,4,4);  //Newly added edges for testing
	p.AddEdge(2,3,12);   //Newly added edges for testing
	p.AddEdge(4,5,10);	//Newly added edges for testing
	p.AddEdge(2,5,5);
	p.AddEdge(4,3,3);
	p.AddEdge(3,6,7) ;
	p.AddEdge(5,6,10);

  p.print_graph(1,6);

	int a = p.GetMaxFlow(1,6);

	cout <<"\n returned from densegraph() call"<<"\n";
	cout <<"size of Edge struct"<<" "<<sizeof(Edge)<<"\n";
	cout <<"size of PushRelabel struct"<<" "<<sizeof(p)<<"\n";
	cout<<"size of graph"<<" "<<sizeof(p.G)<<"\n";

	          

//	cout <<"\n Excess at t is "<< p.excess[*nodeCount]<<"\n";
    cout << "\n Max flow is " << a <<"\n";


	return 0;

}