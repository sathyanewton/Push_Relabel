while((p.excess[s]+p.excess[t])<0)          // standard exit condition. excess is a global variable  
  {                                           // all threads will leave this while loop in fairly the same time. As exit condition is based on a global variable   
  int satsat=0,bbb=0;                         // satsat,bbb used to find the node that is of least adjacent to a given node.  
     if(flag==0)                              // if flag=0, this means that is the first push/relabel operation inside the while loop. 
   {                                          /*    [0][0]   2     [0][1]    8       [0][2]    25     */
        for(k=1; k<=*ant;k++)                 //  *ant points first to [0][3] and is decremented one by one(25--8--2).  
       {                                      //   each of these nodes are activated in the following statement
        p.active[*(ant-k)]=1;                 // we choose nodes one by one from the end of the row 
          // repetitve section of the code used to stop the sink from being activated.
          // ant is 0, if tp is NULL pointer   
          if(*(ant-k)==t)                       // mostly s or t wont come till this point as we have an if condition in the initialize function             
          {                                   
          cout<<"red alert!!!!!!!!!\n\n\n\n";
          while(1);
          }
        privQ.push(*(ant-k));                 // the aforementioned nodes are pushed inside the private queue
                                          //  at this point, we have active[all nodes adjacent to the source]=1;
       }                                    // all the neighbouring nodes to the source is pushed into the privQ.
   }                                      // for the above example we now have [2,4,5,7] in the privQ (say in thread 0,we have threads 0 to 3)
                                          // pushed in the order [7,5,4,2]   



// we absolutely have to have something inside the privQ by this point. If not, the thread keeps polling idolly.


   else                               //second or greater push and lift operation 
    //(else name eisenhower) 
  {
      Qsize = privQ.size();             // Qsize is 4   
      //  big for loop. essentilly most of the work gets done here.

      /* this loop functions if and only if there is atlease one element in the privQ */ 

      for(l=0;l<Qsize;l++)              // 0 to 4.(0,1,2,3)
      {
/*
    The following if condition checks if the node's excess value is less than or equal to 0.
    This if fails for the second iteration of the while loop, Because we have set up the excess values
    in the initialize function.

    if there is some stuff inside the privQ and it is not activated or if it is activated and it's 
    excess value is not greater 0. This if condtion becomes postive
    
    important point to note at this point is that, if there are a group of vertices in the privQ and 
    none of them have their excess values greater than 0.

    all these groups of nodes are activated by iterating oveer the queue and each of the active  
    values is set to 1 and again will be pushed back into the queue.

*/
        if(p.excess[privQ.front()]<=0)          //  if the node's excess is less than 0. 
        {
          Qbuffer = privQ.front();              //  
          privQ.pop();                          //  front is popped 
          p.active[Qbuffer]=0;                  //   active[popped element] is set to 0
          if(!p.active[Qbuffer])                //    always true function==> god knows its point 
          {
            p.active[Qbuffer]=1;                //  active[Qbuffer] is set to 1
            privQ.push(Qbuffer);                //  Qbuffer is pushed into private queue  
          }
        }
        else                      // else name kennedy           
        { 
         // from this point =========> point A 
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
            // to this point ====> point B. 
            // pure code to  find out least hight adjacent vertex

            if(p.dist[privQ.front()]>dist_t)
            {
              amt = min(excess_t,(long long)p.G[privQ.front()][smallVindex].cap);
              pthread_mutex_lock(&mutex);         //start of atomic part 
/*decrement the capacity of the edge as we would be pushing the flow through 
                  the edge, the amount to be pushed is stored in amt*/
              
                  /*increment the capacity of the reverese edge*/
                  p.G[smallestHtVertex][p.G[privQ.front()][smallVindex].index].cap  =  p.G[smallestHtVertex][p.G[privQ.front()][smallVindex].index].cap+amt;

                  /*decrement the excess of the source-node*/
                  p.excess[privQ.front()] = p.excess[privQ.front()]-amt;

                  /*increment the excess of the destination-node*/
                  p.excess[smallestHtVertex] = p.excess[smallestHtVertex]+amt;
    
              pthread_mutex_unlock(&mutex);              //end of atomic part 

              satsat = privQ.front();
               privQ.pop();
               p.active[satsat]=0;
               if(((p.G[satsat][smallVindex].to)!=1)&((p.G[satsat][smallVindex].to)!=t)&(!p.active[p.G[satsat][smallVindex].to]))
                {
                    /*check for sink*/
                  if(p.G[satsat][smallVindex].to==t)
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
                  if(satsat==t)
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
                  if(bbb==t)
                  {
                  cout<<"red alert!!!!!!!!!\n\n\n\n";
                  while(1);
                  }
                  privQ.push(bbb);
                }
            }

        }       // else inside big for (else name kennedy)

      }       // big for closing braces 
      
      master_count++;
      if(master_count==8)
      {
       // break;
      }

   }        //big else  (else name eisenhower) 
  flag=1;
pthread_mutex_lock(&mutex2);
//print_queue(privQ);.
if(privQ.empty()==true)
{
 cout<<"queue empty : thread id "<<thread_id<<"\n";
 print_queue(privQ);
  //  while(1);
}
master_count++;
pthread_mutex_unlock(&mutex2);
/*
if(master_count==500)
{
 // break;
}*/
}           //big while