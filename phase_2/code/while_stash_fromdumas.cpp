   /*
  main chunk inside the thread   

  returnexcess became a double demensional array

  */

    int flag = 0;  // this is to make sure values from return excess are copied into privQ only the first time thread executes
    int i=0,j=0,k=0,l=0,s,sk;
    int cat;
    int *ant=(int*)arg;     //thread local argument, i think arg is the value of returnexcess. 
                            // ant =(void *)&p.returnexcess[z][(p.returnexcess[z].size())-1]
    std::queue<int> privQ;  //private queue for a thread
    thread = pthread_self();
    p.cpuvar++;             //for thread binding 

 while (((p.excess[1].value)+(p.excess[*nodeCount].value))<0) //assumption, the sink is the final node count        
  {
  int sharmila, bbb;        
  if (flag==0)        //first push inside the thread
  {
      for(k=1; k<=*ant;k++)     // from 1 to (void *)&p.returnexcess[z][(p.returnexcess[z].size())-1] where z is thread number
       {                        //
        p.active[*(ant-k)]=1;   // push 
        privQ.push(*(ant-k));
       }
  }
  else                          //flag = 1
  {
    cat = privQ.size();
     for (l=0;l<cat;l++)  // this is to make the excess check for all nodes in the privQ
       { 
          if(p.excess[privQ.front()].get()<=0)   //if excess value is equal to zero
          {
              int a = privQ.front();
              privQ.pop();
              p.active[a]=0;
          if(!p.active[a])
           {
              p.active[a]=1;
              privQ.push(a);
           }
          }
        else            
          {
                int amt;
                LL excess_t=p.excess[privQ.front()].get();//  e' is excess_t
                int smallVindex, smallestHtVertex, dist2,i;
                int dist_t=std::numeric_limits<int>::max();        // h' is dist_t
          for (i=0; i<p.G[privQ.front()].size(); i++)   
          {
                if ((p.G[privQ.front()][i].cap.value)<=0) continue;
                dist2 = p.dist[p.G[privQ.front()][i].to];    
            if (dist2 < dist_t)
            {
                smallestHtVertex=p.G[privQ.front()][i].to;   
                smallVindex=i;
                dist_t=dist2;
            }
            }
    if (p.dist[privQ.front()]>dist_t)
    { 
                   amt = min(excess_t,(long long)p.G[privQ.front()][smallVindex].cap.value);
                   std::atomic_fetch_add(&(p.G[privQ.front()][smallVindex].cap.value), -amt);
                   std::atomic_fetch_add(&(p.G[smallestHtVertex][p.G[privQ.front()][smallVindex].index].cap.value), amt);
                   std::atomic_fetch_add(&(p.excess[privQ.front()].value), -amt);
                   std::atomic_fetch_add(&(p.excess[smallestHtVertex].value), amt);
                  sharmila = privQ.front();
                  privQ.pop();
                  p.active[sharmila]=0;
                  if(((p.G[sharmila][smallVindex].to)!=1)&((p.G[sharmila][smallVindex].to)!=*nodeCount)&(!p.active[p.G[sharmila][smallVindex].to]))
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
    else  //if ((smallestHtVertex!=1)&(smallestHtVertex!=6))
    { 
        {     bbb=privQ.front();
              privQ.pop();
              p.active[bbb]=0;
              p.dist[bbb] =    dist_t + 1;
        if(!p.active[bbb])
        {
              p.active[bbb]=1;
              privQ.push(bbb);
        }
        }
    }
}
}
}
  flag=1;
  }
  pthread_exit(NULL);
