#include <iostream>
#include <pthread.h>
#include <vector>
#include <cmath>
#include <queue>
#include <assert.h>
#include <algorithm>
#include <limits>
//#include <atomic>

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

  void * initialization(int s, int t)
  {// static int r[10];
      int i,j,k,l,m,g;
            dist[s]=N;
           active[s]=active[t]=true;


      for(i=1;i<N;i++)
               {
               excess[i] =    0;
               }
           for (j=1;j<N;j++)
           {    if(j!=s)
                {dist[j]=0;}
           }
        /*for(k=0;k<G[s].size();k++)  { excess[s]=excess[s]-G[s][k].cap;  } */
        cout <<"\nExcess of s initially is "<<excess[s]<<"\n";
         for(g=0;    g<active.size();    g++)
          {
                        cout << "\nActive values of "<<g<< " is "<<active[g];
          }
        for(l=0; l<G[s].size(); l++) //this push from s to neighboring vertices happens just once - initially
        {
            cout << "\n Initializaiton happening, value of capacity is "<<G[s][l].cap;
            G[s][l].flow=G[s][l].cap;
            G[s][l].cap=0;
            G[G[s][l].to][G[s][l].index].cap=G[s][l].flow;      //????
            excess[G[s][l].to] = G[s][l].flow;
            excess[s]=excess[s]-G[s][l].flow;
            Enqueue (G[s][l].to);
            returnexcess[l]=G[s][l].to;

            cout << "\nValues in return excess are "<<returnexcess[l];
        }

        returnexcess.resize(G[s].size());
             cout << "\nsize of returnexcess inside initialization is "<<returnexcess.size();
        cout << "\nExcess of s value is "<<excess[s];
       

  }
 void Enqueue(int v) {
      if (!active[v] && excess[v] > 0) { active[v] = true; Q.push(v); } //actually each thread should check this
     // so that the vertex may be active in other thread due to a push operation as well
    }

};
PushRelabel p(7);
int n=5;

void *mythread(void *arg)
  {
    cout << "\n Entered mythread";
    int flag = 0;  // this is to make sure values from return excess are copied into privQ only the first time thread executes
    int i=0,j=0,k=0,l=0;
    int cat;
    int master_count;
  //struct PushRelabel *threadObj; //New object for struct is created
  //threadObj = (struct PushRelabel *)arg;//Object is of type arg passed to this thread
 // threadObj[1]=(struct PushRelabel *)arg;
  std::queue<int> privQ;
       // std::queue<int> receiveQ;



 while ((p.excess[1]+p.excess[6])<0)  //!!!!!!!!!!!!!!!!! while loop starts !!!!!!!!!!!!!!!!!!!!
// for (j=0;j<=45;j++)
  {
    cout <<"\n Excess of 1 is "<<p.excess[1];
  int sharmila, bbb;
if (flag==0)
    {
      for(k=0; k<p.returnexcess.size();k++)
       {
        cout<< "\nhi, inside flag==0 loop";
        p.active[p.returnexcess[k]]=1;
        cout<<"\n active values of node " <<p.returnexcess[k]<<" is "<<p.active[p.returnexcess[k]];
        privQ.push(p.returnexcess[k]);
        //cout << "\n value in privQ front is "<<privQ.front();
       }
    }
  else
  {
    cat = privQ.size();
    cout <<"\n PrivQ.size inside the 'else' case of for loop is "<<cat;
     for (l=0;l<cat;l++)  // this is to make the excess check for all nodes in the privQ
      {    //cout<<"\n---------Excess of 6 is "<<threadObj->excess[6];
           if(p.excess[privQ.front()]<=0)   //if excess value is equal to zero
          {
             cout << "\n\nInitial value of privQ when value of privQ.front is "<< privQ.front();
                int a = privQ.front();
              // receiveQ.push(privQ.front());
               privQ.pop();
               p.active[a]=0;
               cout << "\nSize of privQ when value of privQ.front is "<< privQ.front() << ": "<<privQ.size();
               if(!p.active[a])
               {
                    p.active[a]=1;
                   privQ.push(a);
               }
               cout << "\nSize of privQ when value of privQ.front is "<<privQ.front()<< ": "<<privQ.size();
               //continue;
           }
           else            //if excess value is greater than 0
           {
                    cout << "\n value in privQ is " << privQ.front();
                    //COPIED PART BELOW
                    cout << "\nEntered Discharge fn for node "<<privQ.front();
                    long long amt;
                    LL excess_t=p.excess[privQ.front()];//  e' is excess_t
                    cout << "\n Excess of node "<<privQ.front()<< " is "<<excess_t;
                    int smallVindex, smallestHtVertex, dist2,i;
                    int dist_t=std::numeric_limits<int>::max();        // h' is dist_t
                    //cout << "\n** ** ** During first iteration dist_t value is ** ** **" << dist_t;
                    cout <<"\n privQ.front is "<<privQ.front();
                for (i=0; i<p.G[privQ.front()].size(); i++)   // to get the lowest vertex among all vertices connected to u //One condition missing here
                {
                          if ((p.G[privQ.front()][i].cap)<=0) continue;
                          cout << "\nEntered for loop when i = " <<i;
                            dist2 = p.dist[p.G[privQ.front()][i].to];    // h" is dist2
                    if (dist2 < dist_t)
                    {
                        cout <<"\n entering 'if' loop to check dist2 < dist_t";
                        smallestHtVertex=p.G[privQ.front()][i].to;    // v' is smallestHtVertex
                        smallVindex=i;
                        cout << "\n ------- Value of smallest ht vertex is "<<smallestHtVertex;
                        dist_t=dist2;
                    }
                }
            // do the push for (u, lowest v chosen by the above for loop)
            if (p.dist[privQ.front()]>dist_t)   //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
            { 
                    cout << "\nGoing to execute push-----Entering loop to check if dist[u] > dist_t";
                  //cout << "\n** \t** \t**\t** Value in G[privQ.front()][1].cap is " <<(long long)threadObj->G[privQ.front()][1].cap;
                    cout<<"\n #########   excess is "<<excess_t;
                            amt = min(excess_t,(long long)p.G[privQ.front()][smallVindex].cap);
                    cout << "\nMin amount is " << amt;
                    p.G[privQ.front()][smallVindex].cap  =  p.G[privQ.front()][smallVindex].cap-amt;
                    cout<<"\nResidual capacity in the edge "<< privQ.front() <<" to "<<smallestHtVertex<<" is "<<p.G[privQ.front()][smallVindex].cap;
                    p.G[smallestHtVertex][p.G[privQ.front()][smallVindex].index].cap  =  p.G[smallestHtVertex][p.G[privQ.front()][smallVindex].index].cap+amt;
                    p.excess[privQ.front()] = p.excess[privQ.front()]-amt;
                    p.excess[smallestHtVertex] = p.excess[smallestHtVertex]+amt;
                    cout << "\nExcess updated inside Push";
                    cout <<"\n---******((---Excess value of New overflowing vertex ["<<smallestHtVertex<<"] is "<< p.excess[smallestHtVertex];
                    sharmila = privQ.front();
                     privQ.pop();
                     p.active[sharmila]=0;
                     // privQ.push(privQ.front());
                    if(((p.G[sharmila][smallVindex].to)!=1)&((p.G[sharmila][smallVindex].to)!=6)&(!p.active[p.G[sharmila][smallVindex].to]))
                    {
                        p.active[p.G[sharmila][smallVindex].to]=1 ;
                        privQ.push(p.G[sharmila][smallVindex].to);
                    }
                    if(!p.active[sharmila])
                    {   
                        p.active[sharmila]=1;
                        privQ.push(sharmila);
                    }
            }
          else
            { //if ((smallestHtVertex!=1)&(smallestHtVertex!=6))
                  
                    bbb=privQ.front();
                    privQ.pop();
                    p.active[bbb]=0;
                    p.dist[bbb] =    dist_t + 1;
                    cout << "\nDistance of node "<<bbb<<" is " << p.dist[bbb];
                if(!p.active[bbb])
                {
                  p.active[bbb]=1;
                  privQ.push(bbb);
                }
            
            }
            cout<<"\n Size of privQ is " <<privQ.size();        

      }
    }   //for loop braces
  }
  cout<<"\n ---- SETTING FLAG TO 1";
  flag=1;
  cout<<"\n -----^^^----Excess value of e(t) is "<<p.excess[6];
  master_count++;
  }                         //while loop braces
  cout<<"master_count : "<<master_count<<"\n";
  pthread_exit(NULL);
  }


int main(int argc, char *argv[])
{  cout <<"Program starting";
    //PushRelabel p(7);
        p.AddEdge( 1,2 ,15) ; p.AddEdge( 2,3 ,12) ; p.AddEdge( 3,6 ,7) ; p.AddEdge( 1,4 ,4) ;
        p.AddEdge( 4,5,10) ; p.AddEdge( 5,6,10) ; p.AddEdge( 2,5,5) ;
        p.AddEdge( 3,4,3) ; // DIRECTION OF EDGES DETERMINES THE MAXIMUM FLOW
        pthread_t p1, p2;

        //p.GetMaxFlow(1,6);
        cout << "\nAbout to call initialization";
        p.initialization(1,6);
        cout << "\nReturned after initialization";
        cout<<"\n return excesses are "<<p.returnexcess[0]<<" "<<p.returnexcess[1];

        cout << "\nthreads declared " <<"\n";
    pthread_create(&p1, NULL, mythread,(void *)&p.returnexcess[0]);

        pthread_create(&p2, NULL, mythread, (void *)&p.returnexcess[1]);

    pthread_join(p1, NULL);
        cout << "\n\nThreads terminated.. Back to main ";
        cout<<"\nThank you\n";
        //cout <<"\n the maximum value is "<<p.excess[6];
       // cout<<"\n The cap from 1 to 2 at termination is "<<p.G[1][0].flow;
        //cout<<"\n The cap from 1 to 4 at termination is "<<p.G[1][1].flow;
       cout<<"\nExcess of source is "<<p.excess[1];
       cout<<"\nExcess of sink is "<<p.excess[6];


       cout<<"\n Distance of [6] is "<<p.dist[6];
       cout<<"\n Distance of [5] is "<<p.dist[5];
       cout<<"\n Distance of [4] is "<<p.dist[4];
         cout<<"\n Distance of [3] is "<<p.dist[3];
         cout<<"\n Distance of [2] is "<<p.dist[2];
         cout<<"\n Distance of [1] is "<<p.dist[1];
        return 0;

        pthread_exit(NULL);
}
	
