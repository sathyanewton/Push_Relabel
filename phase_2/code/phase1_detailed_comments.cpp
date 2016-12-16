
/* sathya's two thread implementation with the hard-coded graph input dated 01/24/2016 */

/*	input graph 

	format of the adjoining table,

	source node || destination node || connecting edge capacity  

	 	1					2						15
	  	1					4						4
	  	2					3						12 
	  	4					5						10
	  	2					5						5
	  	4					3						3
	  	3					6						7
	 	5					6						10

	given above is the input graph that is used for this program. 		

 */

/* Header files */

#include <time.h>
#include <iostream>
#include <cmath>
#include <vector>
#include <queue>
#include <pthread.h>
#include <limits>

#define NoOfNodes 7
#define NoOfThreads 2

using namespace std;

typedef long long LL;

/* edge struct */

struct Edge  {
int from, to, cap, flow, index;								/* names of each parameters are self explanatory */
Edge(int from, int to, int cap, int flow, int index) :
from(from), to(to), cap(cap), flow(flow), index(index) {}	/* constructors for initialization */
};

struct PushRelabel{
	int N;

  vector<vector<Edge> > G;  /* graph data structure, it is a double dimensional array of edge structures; from toplevel : it is a adjacancy list of nodes */
  vector<LL> excess;		/* excess array-> excess of each node, is stored in a array which is indexed using the number representing that node */
  vector<int> dist, active;	/* dist array : holds the height of each node, indexed using the number representing that node */
							/* active array : holds 1(active) or 0(not active) indexed using the number representing that node */
  /* TO BE CLARIFIED */  queue<int> Q;				/* USED INSIDE enqueue function -> TO BE CLARIFIED */
	/*
		This is a queue of active nodes
	*/

  /* TO BE CLARIFIED */  vector<int> returnexcess;	/* returnexcess array : MAJOR CLARIFICATION REQUIRED -> I think it holds the difference between capacity for an edge and the actual flow through an edge of a graph */
  
  /*
		returnexcess is a vector of active nodes at any given point in time. 

  */

  PushRelabel(int N) : N(N), G(N), excess(N), dist(N), active(N), returnexcess(N) {} /* constructor for the PushRelabel structure*/

/* 	Funtion name : AddEdge
	Parameters  : int from(source of the edge), int to(destination of the edge), int cap(capacity of the edge)

	This function populates the graph data structure. 
	It does a push_back after setting parameters for the edge structure.
	the function also inserts a reverse-edge with capacity=0 , flow=0 and index=(G[i].size-1). 
	if there is an edge that starts from one edge and goes to the same edge, we increment the index of that edge. 
	
	==> special case :(inserting an edge that starts from a node and goes to the same node) <==

	The index of an edge that starts from a node and goes to the same node is incremented as the result of the if statement that is there. 
	This is done because, we shall be inserting a redundant edge(element) in the graph array its index shall be the array's current size-1.
	if the from and to are two distinct nodes, then these elements of the forward(cap=capacity input) and the reverse edge(cap=0 initially) will lie in different y-index postions or rows. 
	But if the from and to nodes are the same, we will have forward and reverse edge being inserted in the same y-index(row), hence they cannot have the same index values.  
	G[to].size(before push back) will be equal to (G[from].size-1)(after push_back) as from=to.
	so what we do is we push an edge(Say(2,2)) into G[from]. effectively push edge(2,2,18(random cap),0,G[2].size()(say=1))(G[2].index=1).
	then we increment G[from].index(now G[2].index=2). 
	then we push edge(to,from,0,0,G[from.size()-1])(another edge(2,2) with index=G[2].size()-1(2-1=1))(G[2].index=1)


	say the following is the 2nd row of the graph data structure. 

	G[2] => (2,1).index=0		=> the state of the data structure before inserting the edge(2,2).

	G[2] => (2,1).index=0,	(2,2).index=1	=> state after the first push_back

	if(from==to)==>yes------->(2,2).index++--------->(2,2).index=2	==> state after the if statement. 

	G[2] => (2,1).index=0,	(2,2).index=2,	(2,2).index=1	=> after inserting the reverse edge. 

	from, to and cap is set from the input of the function.
	The flow value is set to 0 initially. 
	index value is set to G[to].size(). (index value signifies the x-index of the edge)

	
*/

  void AddEdge(int from, int to, int cap) {		
    G[from].push_back(Edge(from, to, cap, 0, G[to].size()));
    if (from == to) G[from].back().index++;
    G[to].push_back(Edge(to, from, 0, 0, G[from].size() - 1));
  }

/*	the graph data strucure 

		  index-->		0		||		1		||		2
	from
	  1					(1,2)		  (1,4)				
	  2					(2,1)		  (2,3)			(2,5)
	  3					(3,2)		  (3,4)			(3,6)
	  4					(4,1)		  (4,5)			(4,3)
	  5					(5,4)		  (5,2)			(5,6)
	  6					(6,3)		  (6,5)

	forward edges : (1,2),(1,4),(2,3),(2,5),(4,3),(4,5),(3,6),(5,6)

*/

/*	Funtion name : print_graph
	Parameters 	 : source , sink 

	2 for loops, iterating over the graph data structure and print the from-nodes and to-nodes of each of the edge. 
*/

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

/*	Function name 	: print_adj_list
	Parameters 		: source , sink 

	2 for loops, iterating over the graph data structure and print the from-nodes.
	Prints the adj_list.
*/

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

/*	Function name 	: Enqueue 
	Parameters 		: vertex 


	if the input vertex is not active and has an excess.
	it makes the input vertex active
	puts the given input vertex inside the global queue. 

*/

  void Enqueue(int v) {
    if (!active[v] && excess[v] > 0) 
      { 
        active[v] = true; 
        Q.push(v); 
      }
  }
};

/*	thread variable initialization */

  pthread_t p1,p2;				//thread varaible 
  PushRelabel p(NoOfNodes);		//initialize object of PushRelabel 
  pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;	//initialize mutex that we shall use to lock critical section of the code 


 /*	Function name 	: Parallel_initialzie
	Parameters 		: source , sink 

	It performs the first push. 
									
									"The source push" 

	1.We set the height of the source node to the total number of nodes in the graph. 

	2.Source and sink  becomes active. 

	The algorithm, by the virtue of design it does the initial pushes(of all the adjacent nodes from the source) in this function.



 */



	void parallel_initialize(int s, int t)
	{
  		int m,n,d,i,j,l;                                    //index adjustment
		cout<<"inside initialize"<<"\n";
  		p.dist[s]=NoOfNodes;                                //setting height of source as no.of nodes 
		for(i=s;i<=p.active.size();i++)
  		{
    		p.active[i]=0;									// initializing the active array to 0 
  		}
  		p.active[s]=p.active[t]=true;                       // set s and t to be active. 
  		p.excess[s]=0;										// set the excess of s to 0
  		p.returnexcess[s]=0;								// set the returnexcess of s to 0

		for(i=(s+1);i<t;i++)								// from (s+1) to t
		{				
		  	p.dist[i]=0;									// setting these elements of dist array to 0
		    p.excess[i]=0;									//  setting these elements of excess array to 0
		    p.returnexcess[i]=0;							//  setting these elements of returnexcess array to 0
		}

		for(l=0;l<p.G[s].size();l++)						//	from 0 to G[s].size() ==> number of adjacent edges to the source node.
		{
			p.G[s][l].flow = p.G[s][l].cap;					// flow of the edges from source is same as its capacity. 
            p.G[p.G[s][l].to][p.G[s][l].index].cap=p.G[s][l].flow;	// uses the "index" varilable to set the capacity of the reverse-edge as the flow             
            p.excess[p.G[s][l].to] = p.G[s][l].flow;			//	excess value of the nodes adjacent to the source is set to the flow through these edges
            p.excess[s] = (p.excess[s]-p.G[s][l].flow);			//	excess of source is old excess-flow through this iteration
            p.Enqueue (p.G[s][l].to);							// equeue the active edges
            p.returnexcess[l]=p.G[s][l].to; 					// returnexcess array holds the destination node of each push, 

            /* The returnexcess for the current graph, which has only 2 neighbouring nodes for the source vertex
				
				source is 1. 
				neighbouring nodes of 1 are nodes 2 and 4.

				hence : returnexcess[0] = 2
						returnexcess[1] = 4

            */ 
        }

        p.returnexcess.resize(p.G[s].size());				// resizes the size of the returnexcess array.

 /* 		At this point we have all the overflowing nodes insided the return excess array
			or : we have all the active/enqued nodes inside the return excess array
		
		The idea is to maintain a table of arrays which is indexed using the node number. 


 */ 

/* basic round robin scheduler ======> More about scheduling <========

      if (((p.G[s][i].to)!=s) && ((p.G[s][i].to)!=t))
            buffer[i%NoOfThreads].push_back(G[s][i].to); */
}

/* thread parameters : We shall be try to get rid of this. Let's see */

	struct thread_parameters
	{
	  int x,y,z;
	};


/*	function name : *thread 
	parameters    : void *tp (void pointer as allowed by pthreads) 

	The basic idea of this implementation is that we spawn one thread for each and every one of the neighbours of the source vertex. 
	essentially, we have one returnexcess variable for each thread. (in this case 2 threads)

	we have to make returnexcess[that thread number] --> as a thread private variable
*/

/* main thread function */

void *thread(void *tp)
{

	/* variable declaration */
	thread_parameters *tp1;
	tp1= (thread_parameters *)tp;
	int s,t,u,Qbuffer;
  	int Qsize=0;
  	s=tp1->x;        	//x = s 	source node number 
	t=tp1->y;       	//y = t 	sink node number
	u=tp1->z;      		//thread number 
	int e,v,h1,h2,d;
  	int flag=0;			//variable used to differentiate between the first push operation inside the thread 
  	int count;

  	pthread_t tid = pthread_self();		//thread-id creating  
  	std::queue<int> privQ;           	//private queue data structure  
  	int i,j,k,l;     					//iteratrors

/*	main while loop */


	while((p.excess[s]+p.excess[t])<0)
  	{
  		cout<<"excess value of s "<<p.excess[s]<<"\n";
  		cout<<"excess value of t "<<p.excess[t]<<"\n";

  		int satsat,bbb;					// =========> to be clarified of its purpose <============
  		count++;
  		/*	flag==0 means the part of the code has to be executed only if 
  		it is the first push operation inside the while loop(by the thread) */
  		cout<<"inside while"<<"\n";
  		cout<<"count "<<count<<"\n";
  		if(flag==0)
   		{
	     	pthread_mutex_lock(&mutex);			//returnexcess is a global array, so is the active array. We have to lock any updtaes to the global data structure 
	      	for(k=0;k<p.returnexcess.size();k++)	//iterate over the returnexcess array
	      	{
			    p.active[p.returnexcess[k]]=1;		//name all the nodes in the returnexcess array active 
			    privQ.push(p.returnexcess[k]);		//push the active node inside the privQ
	      	}
	      	pthread_mutex_unlock(&mutex);

		}
		else                               		// not the first push operation  
  		{
  			Qsize = privQ.size(); 				// store the size of the privQ in the Qsize variable
  			for (l=0;l<Qsize;l++)
      		{
		        if(p.excess[privQ.front()]<=0)          //if the node's(privQ.front()) excess is less than or equal to 0. 
		        {
		          Qbuffer = privQ.front();				//	Qbuffer-->stores the first element out of the privQ
		          privQ.pop();							//	remove the first element of the privQ
		          p.active[Qbuffer]=0;					//	active[qbuffer] is set to 0
		          if(!p.active[Qbuffer])				//	active[buffer] is checked to be equal to 0(always true)
		           {
		            p.active[Qbuffer]=1;				//	active[buffer]	is set to 1-->node is activated
		            privQ.push(Qbuffer);				//	the activated node is inserted inside the Qbuffer
		           }
		        }
		        else              						//  if the excess of the node in the privQ.front is greater than 0
		        {
		        	LL amt;									//	long long amt--->i think the amount we are going to push 
          			LL excess_t=p.excess[privQ.front()];	// excess_t-->holds the excess value of the node 

          			int smallVindex, smallestHtVertex, dist2;	// variables used to store the index and height of the vertex with the smallest height
                	int dist_t=std::numeric_limits<int>::max();        // dist_t= some large number(used to find the max height)

                	/*	the below for loop iterates over the column of graph(effectively going through each and every neighbouring-vertex
                		of the current node(privQ.front()))*/

					for (i=0; i<p.G[privQ.front()].size(); i++)   
            		{
		                  	if ((p.G[privQ.front()][i].cap)<=0) continue;
		                    dist2 = p.dist[p.G[privQ.front()][i].to];    // height of the neighbouring vertex stored in dist2
		                      if (dist2 < dist_t)						//	dist2 is always lesser than intial-dist_t			
		                      {
		                          smallestHtVertex=p.G[privQ.front()][i].to;    // smallestHtVertex stores the node-number of the smallest height
		                          smallVindex=i;
		                          dist_t=dist2;                   // height of the lowest height vertex
		                      }
            		}

		           /*	At this point, we have the neighbouring-node with the smallest vertex in the smallestHtVertex and its 
						height in dist_t 
		           */
					/*	to check if the height of the current edge is greater than the height neighbouring node with minimum height(dist_t)	*/

					if(p.dist[privQ.front()]>dist_t)	
            		{
            			/*At this point I think I understand why we have one of the most important difference between
            			sequential push-relabel and parallel push relabel.

      					The difference is, if there are multiple adjacent vertex attached to a single node, we can only 
      					perform a push operation to the node with least height, out of all the neighbouring nodes that are there.  
            			
            			we check if the excess of the chosen overflowing vertex or the capacity 
            			of the edge we are going to push is smaller.*/
            			amt = min(excess_t,(long long)p.G[privQ.front()][smallVindex].cap);

            			pthread_mutex_lock(&mutex);        			//start of atomic part 

            			/*decrement the capacity of the edge as we would be pushing the flow through the edge, the amount to be pushed is stored in amt*/
            			p.G[privQ.front()][smallVindex].cap  =  p.G[privQ.front()][smallVindex].cap-amt;

            			/*increment the capacity of the reverese edge*/
			            p.G[smallestHtVertex][p.G[privQ.front()][smallVindex].index].cap  =  p.G[smallestHtVertex][p.G[privQ.front()][smallVindex].index].cap+amt;

			            /*decrement the excess of the source-node*/
			            p.excess[privQ.front()] = p.excess[privQ.front()]-amt;

			            /*increment the excess of the destination-node*/
			            p.excess[smallestHtVertex] = p.excess[smallestHtVertex]+amt;
    
    					pthread_mutex_unlock(&mutex);              //end of atomic part 


    					/*first time we actually take a value out of the privQ*/
    					satsat = privQ.front();
    					privQ.pop();				// the current node is popped
               			p.active[satsat]=0;			// the popped vertex's active value is set as 0

               			/*if the current-node is not the source(1) or the sink(6)*/

               			if(((p.G[satsat][smallVindex].to)!=1)&((p.G[satsat][smallVindex].to)!=6)&(!p.active[p.G[satsat][smallVindex].to]))
		                {


		                    p.active[p.G[satsat][smallVindex].to]=1;		//active[destination node]=1
		                    privQ.push(p.G[satsat][smallVindex].to);		//push destination node in the privQ
		                }
				                /* if the node is not active*/
				        if(!p.active[satsat])
				        {
				            p.active[satsat]=1;	/* set to as active*/
				            privQ.push(satsat);	/* push the activated node in the privQ */
				        } 
				         		
					}
					/*	this part of the algorithm is aa part where if the current node's height is lesser than the lowest-height 
						node. We keep increasing it's height.
					*/
					else     				//if the height of the current-node is lesser than the lowest-height node     
					{
					  	bbb=privQ.front();	/*	pop the first element of privQ*/
					    privQ.pop();
					    p.active[bbb]=0;	//set it to 0. Just in case	
					    p.dist[bbb] = dist_t + 1;	/* increment the height of the current-node and probably queue it(this keeps happening till the
					    								height of this node gets greater than the lowest-height node*/
					    if(!p.active[bbb])
					    {
					        p.active[bbb]=1;		// make it active, set active to 1
					        privQ.push(bbb);		//	enqueue the current node
					    }
					}
				}
			}
			
		}
		flag=1;
		
		
	}
	cout<<"exiting threads"<<"\n";
		pthread_exit(NULL);
}


int main()
{
  int a,ret,i;


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

thread_parameters args;
  
parallel_initialize(1,6);
cout << "\nReturned after initialization";
cout<<"\n return excesses are "<<p.returnexcess[0]<<" "<<p.returnexcess[1]<<"\n";


  args.x = 1;
  args.y = 6;
  args.z=1;
  

pthread_create(&p1, NULL,thread,(void *)&args);

args.z=2;

pthread_create(&p2, NULL,thread,(void *)&args);

pthread_join(p1, NULL);

pthread_join(p2, NULL);

  cout << "\n\nThreads terminated.. Back to main ";
for(i=1;i<=6;i++)
{
  cout<<"dist loop : "<<p.dist[i]<<"\n";
  cout<<"excess loop : "<<p.excess[i]<<"\n";
}
cout<<"\nExcess of source is "<<p.excess[1]<<"\n";
cout<<"\nExcess of sink is "<<p.excess[6]<<"\n";
cout<<"\n";

return 0;

 pthread_exit(NULL);
}


