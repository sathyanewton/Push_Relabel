
	thread function 

	 // this is to make sure values from return excess are copied into privQ only the first time thread executes
	int flag = 0; 
    int i=0,j=0,k=0,l=0,s,sk;
    int cat;
    int *ant=(int*)arg;			//input casting 

    queue<int> privQ;

    while :	exit criteron 

    	int sharmila, bbb;
      	if (flag==0)
        {
        
      		for(k=1; k<=*ant;k++)
       		{ 
        		p.active[*(ant-k)]=1;
       			privQ.push(*(ant-k));
        
       		}
       }
       else
  	{
  	cat = privQ.size();
     for (l=0;l<cat;l++)  
       { 
           if(p.excess[privQ.front()].get()<=0)   
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
				LL excess_t=p.excess[privQ.front()].get();
                int smallVindex, smallestHtVertex, dist2,i;
		               int dist_t=std::numeric_limits<int>::max();        
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
    else
    { 
        {    bbb=privQ.front();
            privQ.pop();
            p.active[bbb]=0;
            p.dist[bbb] =    dist_t + 1;
    if(!p.active[bbb])
    {p.active[bbb]=1;
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
  }



