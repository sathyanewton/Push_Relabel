// #include <asm/atomic.h>
#define _GNU_SOURCE
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
//#include "denseacyclicgraph.h"
 
#undef max
using namespace std;
typedef long long LL;
#define NoOfNodes 1001
#define NoOfThreads 1
#define handle_error_en(en, msg) \
        do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)
FILE *fp,*fp2;
            int operation[5];
            int edgeCount[3];
            int nodeCount[3];

            int var1[3],var2[3];
//	int cpuvar = 0;
struct AtomicExcess {
    std::atomic<int> value;

    AtomicExcess() :value(){}

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

struct AtomicCap {
    std::atomic<int> value;

    AtomicCap(const AtomicCap& ac) : value(ac.value.load()) {}; // user-defined copy ctor

    AtomicCap(int cap) : value(cap) {}

    int get(){
        return value.load();
    }
};


struct AtomicCpuvar {
    std::atomic<int> value;

    AtomicCpuvar(const AtomicCpuvar& ac) : value(ac.value.load()) {}; // user-defined copy ctor

    AtomicCpuvar(int cpuvar) : value(cpuvar) {}

    int get(){
        return value.load();
    }
};

AtomicCpuvar cpuvar = 0;

struct Edge  {
  AtomicCap cap;
  int from, to, flow, index;
  Edge(int from, int to, int cap , int flow, int index) :
     from(from), to(to), cap(cap), flow(flow), index(index) {}
};

//pthread_mutex_t lock;
struct PushRelabel {
  int N; 
//	int cpuvar = 0;
  vector<vector<Edge> > G;
  //vector<vector<AtomicCounter> > G;

  vector<AtomicExcess> excess;
  //vector<LL> excess;

  vector<int> dist, active, count;
  queue<int> Q;
  vector<vector<int>>returnexcess;
  vector<int> row;

  PushRelabel(int N) : N(N), G(N), excess(N), dist(N), active(N), count(2*N), returnexcess(N), row(N) {}

  void AddEdge(int from, int to, int cap) {
     // cout << "\nAddedge called";
        G[from].push_back(Edge(from, to, cap, 0, G[to].size()));
        if (from == to) G[from].back().index++;
        G[to].push_back(Edge(to, from, 0, 0, G[from].size() - 1));
    }

  void * initialization(int s, int t)
  {// static int r[10];
    // cout <<"\n Inside initialization";
      int i,j,l,m,k,r;
            dist[s]=N;
           active[s]=active[t]=true;

      for(i=1;i<N;i++)
               {
               excess[i].value =    0;
               }
           for (j=1;j<N;j++)
           {    if(j!=s)
                {dist[j]=0;
                }
                      }
        
           for(m=0; m<NoOfThreads; m++)
           {
                      returnexcess.push_back(row);
           }

           for(l=0; l<G[s].size(); l++)
        {
          //  cout<<"\n inside main for loop";
            G[s][l].flow=G[s][l].cap.get();
            G[s][l].cap.value=0;
            G[G[s][l].to][G[s][l].index].cap.value = G[s][l].flow;
            excess[G[s][l].to].value = G[s][l].flow;
            excess[s].value=excess[s].value-G[s][l].flow;
            Enqueue (G[s][l].to);

          //  returnexcess[0].push_back(10);
		if (((G[s][l].to)!=s) && ((G[s][l].to)!=t))
            returnexcess[l%NoOfThreads].push_back(G[s][l].to);

            //cout << "\nValues in return excess ["<<l%2 <<"] are "<<returnexcess[l%2][l];
                    }
        
for(k=0;k<NoOfThreads;k++)
{returnexcess[k].resize(returnexcess[k].size());}
       
for(r=0;r<NoOfThreads;r++)
{
       returnexcess[r].push_back(returnexcess[r].size());
}
       
  }
 void Enqueue(int v) {
      if (!active[v] && excess[v].get() > 0) { active[v] = true; Q.push(v); } //actually each thread should check this
     // so that the vertex may be active in other thread due to a push operation as well
    }

};
PushRelabel p(NoOfNodes);

void *mythread(void *arg)
  {
//int cpuvar;
//	pthread_mutex_t lock;
	
    //cout << "\n Entered mythread";
    int flag = 0;  // this is to make sure values from return excess are copied into privQ only the first time thread executes
    int i=0,j=0,k=0,l=0,s,sk;
    int cat;
    int *ant=(int*)arg;
    
  std::queue<int> privQ;
cpu_set_t cpuset1, cpuset2 ;
	pthread_t thread;
	thread = pthread_self(); 
	CPU_ZERO(&cpuset1);
	CPU_ZERO(&cpuset2);

//pthread_mutex_lock(&lock);
CPU_SET((cpuvar.get()%4), &cpuset1);
int one =1;
std::atomic_fetch_add(&(cpuvar.value), one );
//cpuvar.value++;


//pthread_mutex_unlock(&lock);


	s = pthread_setaffinity_np(thread, sizeof(cpu_set_t), &cpuset1);
	if (s != 0)
        handle_error_en(s, "pthread_setaffinity_np");
/*	 s = pthread_getaffinity_np(thread, sizeof(cpu_set_t), &cpuset2);
    if (s != 0)
        handle_error_en(s, "pthread_getaffinity_np");
    printf("\nSet returned by pthread_getaffinity_np() contained:\n");
   for (sk = 0; sk < CPU_SETSIZE; sk++)
        if (CPU_ISSET(sk, &cpuset2))
            printf("    CPU %d for %u \n", sk,thread);
*/	
        while (((p.excess[1].value)+(p.excess[*nodeCount].value))<0)
// for (j=0;j<=45;j++)
  {
   // cout << "\n while";

  int sharmila, bbb;
      if (flag==0)
             {
        // cout <<"\nEntering flag = 0 once";
      for(k=1; k<=*ant;k++)
       { //cout<< "\nhi, inside flag==0 loop, val of *ant is "<<*ant;

        p.active[*(ant-k)]=1;
        //cout<<"\n active values of node " <<p.returnexcess[k]<<" is "<<p.active[p.returnexcess[k]];
        privQ.push(*(ant-k));
        //cout << "\n value in privQ front is "<<privQ.front();
       }
          }
  else
  {cat = privQ.size();
 //cout <<"\n PrivQ.size inside the 'else' case of for loop is "<<cat;
     for (l=0;l<cat;l++)  // this is to make the excess check for all nodes in the privQ
       { //cout<<"\n---------Excess of 6 is "<<threadObj->excess[6];
           if(p.excess[privQ.front()].get()<=0)   //if excess value is equal to zero
          {
   //      cout << "\n\nInitial value of privQ when value of privQ.front is "<< privQ.front();
            int a = privQ.front();
          // receiveQ.push(privQ.front());
           privQ.pop();
           p.active[a]=0;
     //      cout << "\nSize of privQ when value of privQ.front is "<< privQ.front() << ": "<<privQ.size();
           if(!p.active[a])
           {p.active[a]=1;
           privQ.push(a);
           }
       //    cout << "\nSize of privQ when value of privQ.front is "<<privQ.front()<< ": "<<privQ.size();
           //continue;
           }
           else            //if excess value is greater than 0
           {
         //      cout << "\n value in privQ is " << privQ.front();
                    //COPIED PART BELOW
        //cout << "\nEntered Discharge fn for node "<<privQ.front();
     // long long amt;
        int amt;

        LL excess_t=p.excess[privQ.front()].get();//  e' is excess_t
        //cout << "\n Excess of node "<<privQ.front()<< " is "<<excess_t;
                int smallVindex, smallestHtVertex, dist2,i;
               int dist_t=std::numeric_limits<int>::max();        // h' is dist_t
//int dist_t=5000;
        //cout << "\n** ** ** During first iteration dist_t value is ** ** **" << dist_t;
          //  cout <<"\n privQ.front is "<<privQ.front();
        for (i=0; i<p.G[privQ.front()].size(); i++)   // to get the lowest vertex among all vertices connected to u //One condition missing here
            {
            if ((p.G[privQ.front()][i].cap.value)<=0) continue;
            //cout << "\nEntered for loop when i = " <<i;
              dist2 = p.dist[p.G[privQ.front()][i].to];    // h" is dist2
            if (dist2 < dist_t)
            {//cout <<"\n entering 'if' loop to check dist2 < dist_t";
            smallestHtVertex=p.G[privQ.front()][i].to;    // v' is smallestHtVertex
            smallVindex=i;
            //cout << "\n ------- Value of smallest ht vertex is "<<smallestHtVertex;
            dist_t=dist2;
            }
            }
        // do the push for (u, lowest v chosen by the above for loop)

    if (p.dist[privQ.front()]>dist_t)
    { //cout << "\nGoing to execute push-----Entering loop to check if dist[u] > dist_t";
    //cout << "\n** \t** \t**\t** Value in G[privQ.front()][1].cap is " <<(long long)threadObj->G[privQ.front()][1].value;
           //   cout<<"\n #########   excess of t is "<<excess_t;

                 amt = min(excess_t,(long long)p.G[privQ.front()][smallVindex].cap.value);
            //   cout << "\nMin amount is " << amt;


               // pthread_mutex_lock(&lock);
//int *address = &p.G[privQ.front()][smallVindex].value ;
      //std::atomic<long long> a = ATOMIC_VAR_INIT(p.G[privQ.front()][smallVindex].value);
                 //const long long N = p.G[privQ.front()][smallVindex].cap;
/*
               std::atomic <struct> Edge  {
                   int from, to, cap, flow, index;
                   Edge(int from, int to, int cap, int flow, int index) :
                      from(from), to(to), cap(cap), flow(flow), index(index) {}
                 };

  */
                // std::atomic<long long>G[privQ.front()][smallVindex].cap  = ATOMIC_VAR_INIT(N);


                 //std::atomic_fetch_add(10, 2);
                  // std::atomic_fetch_add(&a, -amt);
             //   p.G[privQ.front()][smallVindex].cap  =  p.G[privQ.front()][smallVindex].cap-amt;
                //cout<<"\nResidual capacity in the edge "<< privQ.front() <<" to "<<smallestHtVertex<<" is "<<p.G[privQ.front()][smallVindex].value;
      //std::atomic<long long> p.G[smallestHtVertex][p.G[privQ.front()][smallVindex].index].cap  = ATOMIC_VAR_INIT(p.G[smallestHtVertex][p.G[privQ.front()][smallVindex].index].value);
      //std::atomic_fetch_add(&p.G[smallestHtVertex][p.G[privQ.front()][smallVindex].index].cap, amt);
               // p.G[smallestHtVertex][p.G[privQ.front()][smallVindex].index].cap  =  p.G[smallestHtVertex][p.G[privQ.front()][smallVindex].index].cap+amt;

       //         p.excess[privQ.front()] = p.excess[privQ.front()]-amt;
         //       p.excess[smallestHtVertex] = p.excess[smallestHtVertex]+amt;

                //pthread_mutex_unlock(&lock);

                // p.G[privQ.front()][smallVindex].cap.value  =  p.G[privQ.front()][smallVindex].cap.value-amt;
                 std::atomic_fetch_add(&(p.G[privQ.front()][smallVindex].cap.value), -amt);
                                 //cout<<"\nResidual capacity in the edge "<< privQ.front() <<" to "<<smallestHtVertex<<" is "<<p.G[privQ.front()][smallVindex].cap;
                 //p.G[smallestHtVertex][p.G[privQ.front()][smallVindex].index].cap.value  =  p.G[smallestHtVertex][p.G[privQ.front()][smallVindex].index].cap.value+amt;
                 std::atomic_fetch_add(&(p.G[smallestHtVertex][p.G[privQ.front()][smallVindex].index].cap.value), amt);


                 //p.excess[privQ.front()].value = p.excess[privQ.front()].get()-amt;
                 //p.excess[smallestHtVertex].value = p.excess[smallestHtVertex].get()+amt;


                 //p.excess[privQ.front()] = p.excess[privQ.front()]-amt;
                 std::atomic_fetch_add(&(p.excess[privQ.front()].value), -amt);
                 //p.excess[smallestHtVertex] = p.excess[smallestHtVertex]+amt;
                 std::atomic_fetch_add(&(p.excess[smallestHtVertex].value), amt);

             //   cout << "\nExcess updated inside Push";
              //  cout <<"\n---******((---Excess value of New overflowing vertex ["<<smallestHtVertex<<"] is "<< p.excess[smallestHtVertex].get();
                sharmila = privQ.front();
               privQ.pop();
               p.active[sharmila]=0;
                 // privQ.push(privQ.front());
                if(((p.G[sharmila][smallVindex].to)!=1)&((p.G[sharmila][smallVindex].to)!=*nodeCount)&(!p.active[p.G[sharmila][smallVindex].to]))
                {
                    p.active[p.G[sharmila][smallVindex].to]=1 ;
                    privQ.push(p.G[sharmila][smallVindex].to);
                }
                if(!p.active[sharmila])
                                           {p.active[sharmila]=1;
                                           privQ.push(sharmila);
                                           }
    }
    else
    { //if ((smallestHtVertex!=1)&(smallestHtVertex!=6))
        {    bbb=privQ.front();
            privQ.pop();
            p.active[bbb]=0;
            p.dist[bbb] =    dist_t + 1;
    //cout << "\nDistance of node "<<bbb<<" is " << p.dist[bbb];
    if(!p.active[bbb])
    {p.active[bbb]=1;
    privQ.push(bbb);
    }
        }
    }
     //cout<<"\n Size of privQ is " <<privQ.size();
           }
       }
  }
      //cout<<"\n ---- SETTING FLAG TO 1";
  flag=1;
 // cout<<"\n -----^^^----Excess value of e(t) is "<<p.excess[*nodeCount].get();
  }
// cout <<"\n thread terminatessssssss";
  pthread_exit(NULL);
  }


int main(int argc, char *argv[])
{  //    cout <<"Program starting";
int z,u,s,j,s1,j1;
/*	cpu_set_t cpuset1, cpuset2 ;
	pthread_t thread;
	thread = pthread_self(); 
	CPU_ZERO(&cpuset1);
	CPU_ZERO(&cpuset2);
*/
cout <<"\n No Of Threads is "<<NoOfThreads;
    //PushRelabel p(7);
//densegraph();
/*
        p.AddEdge( 1,2 ,15) ; p.AddEdge( 2,3 ,12) ; p.AddEdge( 3,6 ,7) ; p.AddEdge( 1,4 ,4) ;
        p.AddEdge( 4,5,10) ; p.AddEdge( 5,6,10) ; p.AddEdge( 2,5,5) ;
        p.AddEdge( 3,4,3) ; // DIRECTION OF EDGES DETERMINES THE MAXIMUM FLOW


        p.AddEdge(1,7,12);  //Newly added edges for testing - Node 7 added
           p.AddEdge(7,6,5);   //Newly added edges for testing
        p.AddEdge(1,8,4);    //Newly added edges for testing - Node 8 added
        p.AddEdge(8,6,9);    //Newly added edges for testing

//*/   
// cout <<"\n returned from densegraph() call";
	fp = fopen("out.txt", "r");
            if (fp == NULL) {
                    printf("Can't open input file!\n");
                    exit(1);
                }
            if(fscanf(fp, "%d %d", &nodeCount, &edgeCount) != EOF)
               {
               printf("\n Node count and Edge count are %d %d", *nodeCount, *edgeCount);
               }
            while (fscanf(fp, "%d %d %d", &operation,&var1,&var2) != EOF)
               {    //  printf("\nThe values are %d %d %d", *operation, *var1, *var2);
                             p.AddEdge(*operation, *var1, *var2);

               }


        //    pthread_t p1, p2;
	pthread_t t[NoOfThreads];
 /*s1 = pthread_getaffinity_np(thread, sizeof(cpu_set_t), &cpuset2);
    if (s1 != 0)
        handle_error_en(s1, "pthread_getaffinity_np");
    printf("\nSet returned by pthread_getaffinity_np() contained:\n");
   for (j1 = 0; j1 < CPU_SETSIZE; j1++)
        if (CPU_ISSET(j1, &cpuset2))
            printf("    CPU %d\n", j1);
   */
        //p.GetMaxFlow(1,6);
   //     cout << "\nAbout to call initialization";
	cout << "\n*nodecount value is "<<*nodeCount;
        p.initialization(1,*nodeCount);
     //   cout << "\nReturned after initialization";
       // cout<<"\n return excesses are "<<p.returnexcess[0]<<" "<<p.returnexcess[1];
//cout << "\n-------------------value of &p.returnexcess[0] " <<(void *)&p.returnexcess[0];
        //cout << "\nthreads declared " <<"\n";
         //gettimeofday();
            struct timeval tim;
            
	for(z=0; z<NoOfThreads; z++)
       {
    pthread_create(&t[z], NULL, mythread,(void *)&p.returnexcess[z][(p.returnexcess[z].size())-1]);
       }

//for (j = 0; j < 8; j++)
    
/*	CPU_SET(2, &cpuset1);
	s = pthread_setaffinity_np(thread, sizeof(cpu_set_t), &cpuset1);
	if (s != 0)
        handle_error_en(s, "pthread_setaffinity_np");
	 s = pthread_getaffinity_np(thread, sizeof(cpu_set_t), &cpuset2);
    if (s != 0)
        handle_error_en(s, "pthread_getaffinity_np");
    printf("\nSet returned by pthread_getaffinity_np() contained:\n");
   for (j = 0; j < CPU_SETSIZE; j++)
        if (CPU_ISSET(j, &cpuset2))
            printf("    CPU %d\n", j);
    //exit(EXIT_SUCCESS);

*/
		gettimeofday(&tim, NULL);
        	double t1=tim.tv_sec+(tim.tv_usec/1000000.0);
     
            for(u=0; u<NoOfThreads; u++)
            {
            pthread_join(t[u], NULL);
            }

            		gettimeofday(&tim, NULL);
                        double t2=tim.tv_sec+(tim.tv_usec/1000000.0);
                        cout<<"\n Seconds elapsed "<< t2-t1;
       //     pthread_join(t[u], NULL);

  cout << "\n\nThreads terminated.. Back to main ";
      //  cout<<"\nThank you\n";
        //cout <<"\n the maximum value is "<<p.excess[6];// cout<<"\n The cap from 1 to 2 at termination is "<<p.G[1][0].flow;
        //cout<<"\n The cap from 1 to 4 at termination is "<<p.G[1][1].flow;
        cout<<"\nExcess of sink is "<<p.excess[*nodeCount].get();
         //cout<<"\n Distance of [3] is "<<p.dist[3];
         //cout<<"\n Distance of [2] is "<<p.dist[2];
         cout<<"\n Distance of [1] is "<<p.dist[1]<<"\n";
       //  cout<<"\nExcess of source is "<<p.excess[1].get();

        return 0;

        pthread_exit(NULL);
}

trial.cpp
Displaying trial.cpp.

