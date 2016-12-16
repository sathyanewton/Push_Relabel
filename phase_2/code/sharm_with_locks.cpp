// #include <asm/atomic.h>
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
#define NoOfNodes 15
#define NoOfThreads 4
//int NoOfNodes;
//int NoOfThreads;
FILE *fp,*fp2;
            int operation[5];
            int edgeCount[3];
            int nodeCount[3];

            int var1[3],var2[3];
            unsigned long int countEmptyQ=0;
struct AtomicExcess {
    std::atomic<int> value;

    AtomicExcess() :value(){}

    AtomicExcess(const std::atomic<int> &a)
       :value(a.load()) {}

    /*AtomicExcess(const AtomicExcess &other)
      :value(other.value.load()) {}*/

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

/*
struct AtomicemptyQueueIndicator {
    std::atomic<int> value;

    AtomicemptyQueueIndicator() :value(){}

    AtomicemptyQueueIndicator(const std::atomic<int> &a)
       :value(a.load()) {}

    AtomicemptyQueueIndicator &operator=(const AtomicemptyQueueIndicator &other)
    {
        value.store(other.value.load());
    }

    AtomicemptyQueueIndicator(const AtomicemptyQueueIndicator& ae) : value(ae.value.load()) {}; // user-defined copy ctor

    AtomicemptyQueueIndicator(int emptyQueueIndicator) : value(emptyQueueIndicator) {}

    int get(){
        return value.load();
    }
};
*/
struct AtomicExchangeFlag {
    std::atomic<int> value;

    AtomicExchangeFlag() :value(){}

    AtomicExchangeFlag(const std::atomic<int> &a)
       :value(a.load()) {}

    AtomicExchangeFlag &operator=(const AtomicExchangeFlag &other)
    {
        value.store(other.value.load());
    }

    AtomicExchangeFlag(const AtomicExchangeFlag& ae) : value(ae.value.load()) {}; // user-defined copy ctor

    AtomicExchangeFlag(int exchangeFlag) : value(exchangeFlag) {}

    int get(){
        return value.load();
    }
};


struct Edge  {
  AtomicCap cap;
  int from, to, flow, index;
  Edge(int from, int to, int cap , int flow, int index) :
     from(from), to(to), cap(cap), flow(flow), index(index) {}
};

pthread_mutex_t lock;
struct PushRelabel {
  int N;
  vector<vector<Edge> > G;
  //vector<vector<AtomicCounter> > G;

  vector<AtomicExcess> excess;
  //vector<LL> excess;

  vector<int> dist, active, count;
  queue<int> Q;
  vector<vector<int>>returnexcess;
  vector<int> row;
//  vector<AtomicemptyQueueIndicator> emptyQueueIndicator;
  vector<unsigned long int> emptyQueueIndicator;
  vector<AtomicExchangeFlag>exchangeFlag;

  PushRelabel(int N) : N(N), G(N), excess(N), dist(N), active(N), count(2*N), returnexcess(N), row(N), emptyQueueIndicator(N), exchangeFlag(N) {}

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
        //   cout<<"\n Outside second for loop";
        /*for(k=0;k<G[s].size();k++)  { excess[s]=excess[s]-G[s][k].cap;  } */
      //  cout <<"\nExcess of s initially is "<<excess[s].get()<<"\n";
        /* for(g=0;    g<active.size();    g++)
                    {
                        cout << "\nActive values of "<<g<< " is "<<active[g];
                    }
                    */

         //  cout <<"\n About to enter main for loop of initialization";
          // cout <<"\nG[s}.size is "<< G[s].size();

           for(m=0; m<NoOfThreads; m++)
           {
                      returnexcess.push_back(row);

                  //    emptyQueueIndicator.push_back(column);
                     // exchangeFlag.push_back(row);
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

            returnexcess[l%NoOfThreads].push_back(G[s][l].to);

            //cout << "\nValues in return excess ["<<l%2 <<"] are "<<returnexcess[l%2][l];
        }
         //  cout<<"\n";
          /* int shar,ranju;
           for(shar=0;shar<returnexcess[1].size();shar++)
cout<<"\t Values in return excess of 1 are "<< returnexcess[1][shar];
           for(ranju=0;ranju<returnexcess[1].size();ranju++)
           cout << "\nValues in return excess of 0 are "<< returnexcess[0][ranju];
           */
           //cout <<"\n the value of returnexcess[0] is "<<returnexcess[0].size();
//cout <<"\n the value of returnexcess[1] is "<<returnexcess[1].size();
for(k=0;k<NoOfThreads;k++)
{returnexcess[k].resize(returnexcess[k].size());}
       // returnexcess[0].resize(returnexcess[0].size());
       // returnexcess[1].resize(returnexcess[1].size());
  //      cout <<"\n NEW: the value of returnexcess[0] is "<<returnexcess[0].size();
    //    cout <<"\n NEW: the value of returnexcess[1] is "<<returnexcess[1].size();
for(r=0;r<NoOfThreads;r++)
{
       returnexcess[r].push_back(returnexcess[r].size());
}
       //    returnexcess[0].push_back(returnexcess[0].size());
    //    returnexcess[1].push_back(returnexcess[1].size());
      //      cout << "\nSize of returnexcess inside initialization is "<<returnexcess[0].size();
        //    cout << "\nSize of returnexcess inside initialization is "<<returnexcess[1].size();
          //  cout<<" \n Value of returnexcess[0][[4] = "<<returnexcess[0][4];
           // cout<<" \n Value of returnexcess[1][[4] = "<<returnexcess[1][4];
     //   cout << "\n My mom name is ";
        //    cout << "\nValue of s after initialization is "<<excess[1].get();

  }
 void Enqueue(int v) {
      if (!active[v] && excess[v].get() > 0) { active[v] = true; Q.push(v); } //actually each thread should check this
     // so that the vertex may be active in other thread due to a push operation as well
    }

};
PushRelabel p(NoOfNodes);

void *mythread(void *arg)
  {
    unsigned long int threadid;
    //cout << "\n Entered mythread";
    int flag = 0;  // this is to make sure values from return excess are copied into privQ only the first time thread executes
    int i=0,j=0,k=0,l=0;
    int cat, queueCheck;
    int *ant=(int*)arg;
    int excessFlag = 0;
    //cout <<"\n value of arg is "<<arg;

  std::queue<int> privQ;
       // std::queue<int> receiveQ;
  //cout<<"\n p.excess[6] is replaced with *nodeCount: "<< *nodeCount;
    // cout <<"\n"<<p.excess[checkcount].get();
 while (((p.excess[1].value)+(p.excess[*nodeCount].value))<0)
// for (j=0;j<=45;j++)
  {int v;
    // cout <<"\n Excess of 1 is "<<p.excess[1].get();
  //cout << "\n  size of return excess is "<<threadObj->returnexcess.size();
//cout <<"\n Entering while";
  int count = 0;

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
  {
      cat = privQ.size();

      for (l=0;l<cat;l++)  // this is to make the excess check for all nodes in the privQ
       { //cout<<"\n---------Excess of 6 is "<<threadObj->excess[6];
           if(p.excess[privQ.front()].get()<=0)   //if excess value is equal to zero
          {
               int one = 1;
               count++;
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
           if(count==cat)
           {
        //       cout<<"\nVertexExchange is going to take place";

              // std::atomic_fetch_add(&(p.emptyQueueIndicator[countEmptyQ].value), one);

                      pthread_mutex_lock(&lock);
                   p.emptyQueueIndicator[countEmptyQ]=pthread_self();

                   p.emptyQueueIndicator[countEmptyQ+1]    =    1;
                //p.emptyQueueIndicator[countEmptyQ+1]=threadid;
                countEmptyQ++;

          //   cout<<"\n The value of threadid is " << p.emptyQueueIndicator[0];
            // cout<<"\n The value is "<<p.emptyQueueIndicator[1];

            // cout<<"\n Size of threadid is "<<sizeof(long int);
             pthread_mutex_unlock(&lock);
           }

           else
           {
             // cout<<"\nCount not equal to cat";
           }
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
  queueCheck = privQ.size();
  }
// cout <<"\n thread terminatessssssss";
  pthread_exit(NULL);
  }


int main(int argc, char *argv[])
{  //    cout <<"Program starting";

    int z,u;
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
*/
   cout <<"\n returned from densegraph() call";
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

        //p.GetMaxFlow(1,6);
   //     cout << "\nAbout to call initialization";
cout << "\n *nodecount value is "<<*nodeCount;
        p.initialization(1,*nodeCount);
     //   cout << "\nReturned after initialization";
       // cout<<"\n return excesses are "<<p.returnexcess[0]<<" "<<p.returnexcess[1];
//cout << "\n-------------------value of &p.returnexcess[0] " <<(void *)&p.returnexcess[0];
        //cout << "\nthreads declared " <<"\n";
         //gettimeofday();
       /* int jn;
        for (jn=0;jn<10;jn++)
        {
            cout<<"\nInitial Values of exchangeFlag "<<p.exchangeFlag[jn].value;
        }
      */

        struct timeval tim;
            gettimeofday(&tim, NULL);
            double t1=tim.tv_sec+(tim.tv_usec/1000000.0);
for(z=0; z<NoOfThreads; z++)
       {
    pthread_create(&t[z], NULL, mythread,(void *)&p.returnexcess[z][(p.returnexcess[z].size())-1]);
       }

      // pthread_create(&t[z], NULL, mythread, (void *)&p.returnexcess[1][(p.returnexcess[1].size())-1]);

  //     pthread_create(&p3, NULL, mythread, (void *)&p.returnexcess[2]);

//        pthread_create(&p4, NULL, mythread, (void *)&p.returnexcess[3]);


            for(u=0; u<NoOfThreads; u++)
            {
            pthread_join(t[u], NULL);
            }

                        gettimeofday(&tim, NULL);
                        double t2=tim.tv_sec+(tim.tv_usec/1000000.0);
                        cout<<"\n\nSeconds elapsed "<< t2-t1;
       //     pthread_join(t[u], NULL);

  cout << "\n\nThreads terminated.. Back to main ";
        cout<<"\nThank you\n";
        //cout <<"\n the maximum value is "<<p.excess[6];// cout<<"\n The cap from 1 to 2 at termination is "<<p.G[1][0].flow;
        //cout<<"\n The cap from 1 to 4 at termination is "<<p.G[1][1].flow;
        cout<<"\nExcess of sink is "<<p.excess[*nodeCount].get();
         //cout<<"\n Distance of [3] is "<<p.dist[3];
         //cout<<"\n Distance of [2] is "<<p.dist[2];
         cout<<"\n Distance of [1] is "<<p.dist[1];
       //  cout<<"\nExcess of source is "<<p.excess[1].get();

        return 0;

        pthread_exit(NULL);
}