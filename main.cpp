#include <iostream>
#include <ctime>
#include <stack>
#include <condition_variable>
#include <thread>
#include <mutex>
#include <unistd.h>

using namespace std;

mutex m;
condition_variable cv;
bool ready = false;
bool processed = false;

void foo(int &k)
{
    unique_lock<mutex> lk(m);
    cv.wait(lk, []{return ready;});
    srand(time(0));
    k+=rand()%50+10;
    processed=true;
    lk.unlock();
    cv.notify_one();
}


//RETURN DATA TO THE MAIN
void back()
{
    unique_lock<mutex> lk(m);
    cv.wait(lk, []{return processed;});
}

//SEND DATA TO A THREAD
void forward()
{
  unique_lock<mutex> lk(m);
  ready = true;
  cv.notify_one();
}

int main()
{
    int SIZE = 10;
    int k=0;
    int DELAY=5000;
    stack <int> collection;
    thread threads[SIZE];
    for (int i=0; i<SIZE; ++i)
    {
        threads[i] = thread(foo,ref(k));

        forward();
        back();

        usleep(DELAY);

        collection.push(k);
        cout<<"Thread "<<i<<": ";
        cout<<collection.top()<<endl;
    }

    for (auto& th : threads)
    {
        th.join();
    }

    return 0;
}
