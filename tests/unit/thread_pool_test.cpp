#include "thread_pool.hpp"
#include "utils.hpp"

#include <atomic>

using namespace std;

int main() {
    int failures = 0;
    atomic<int> completed{0};

    {
        ThreadPool pool(4);
        for(int i=0; i<1000; i++) {
            pool.enqueue(
                [&completed] {
                    completed.fetch_add(1);
                }
            );
        }
    }

    check(completed==1000, "ThreadPool executes all queued tasks", failures);

    return ((failures==0) ? 0 : 1);
}