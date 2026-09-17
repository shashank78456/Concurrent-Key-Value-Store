#pragma once

#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>
using namespace std;

class ThreadPool {
private:
    vector<thread> workers;
    queue<function<void()>> tasks;
    
    mutex mtx;
    condition_variable condition;
    bool stop = false;

public:
    ThreadPool(size_t num_threads);

    void enqueue(function<void()> task);

    ~ThreadPool();
};