







        p.G[privQ.front()][smallVindex].cap  =  p.G[privQ.front()][smallVindex].cap-amt;
 		p.G[smallestHtVertex][p.G[privQ.front()][smallVindex].index].cap  =  p.G[smallestHtVertex][p.G[privQ.front()][smallVindex].index].cap+amt;
        p.excess[privQ.front()] = p.excess[privQ.front()]-amt;
        p.excess[smallestHtVertex] = p.excess[smallestHtVertex]+amt;



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

/*  void increment(){
        ++value;
    }

    void decrement(){
        --value;
    }
*/

    int get(){
        return value.load();
    }
};