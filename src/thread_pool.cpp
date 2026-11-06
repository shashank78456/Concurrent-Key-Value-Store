#include "thread_pool.hpp"

ThreadPool::ThreadPool(size_t num_threads) {
    for(size_t i=0; i<num_threads; i++) {
        workers.emplace_back([this]() {
            while(true) {
                function<void()> task;

                {
                    unique_lock<mutex> lock(mtx);

                    condition.wait(lock, [this]() {
                        return stop || !tasks.empty();
                    });

                    if(stop && tasks.empty())
                        return;
                    
                    task = std::move(tasks.front());
                    tasks.pop();
                }

                task();
            }
        });
    }
}

void ThreadPool::enqueue(function<void()> task){
    {
        lock_guard<mutex> lock(mtx);
        if(stop)
            return;

        tasks.push(std::move(task));
    }

    condition.notify_one();
}

ThreadPool::~ThreadPool() {
    {
        lock_guard<mutex> lock(mtx);
        stop = true;
    }

    condition.notify_all();

    for(auto &worker : workers) {
        worker.join();
    }
}