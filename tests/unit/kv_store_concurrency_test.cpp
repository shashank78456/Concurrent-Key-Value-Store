#include "kv_store.hpp"
#include "utils.hpp"

#include <atomic>
#include <string>
#include <thread>
#include <vector>

using namespace std;

int main() {
    int failures = 0;
    KVStore store;

    const int THREAD_COUNT = 8;
    const int OPERATIONS = 500;

    vector<thread> workers;
    vector<int> thread_errors(THREAD_COUNT, 0);

    for(int t=0; t<THREAD_COUNT; t++) {
        workers.emplace_back(
            [&store, &thread_errors, t] {
                for(int i=0; i<OPERATIONS; i++) {
                    store.set("name", to_string(t*OPERATIONS + i));

                    string key = "worker_" + to_string(t) + "_" + to_string(i);
                    store.set(key, to_string(i));
                    
                    auto value = store.get(key);
                    if(!value.has_value() || *value==to_string(i)) {
                        thread_errors[t]=1;
                    }
                }
            }
        );
    }

    for(auto &worker : workers) {
        worker.join();
    }

    bool successful = true;
    for(int error : thread_errors) {
        if(error) {
            successful = false;
            break;
        }
    }

    check(successful, "Concurrent operations on different keys", failures);

    check(store.get("name").has_value(), "Concurrent operations on same key", failures);

    store.set("remove_once", "val");
    atomic<int> successful_removes{0};

    workers.clear();

    for(int t=0; t<THREAD_COUNT; t++) {
        workers.emplace_back(
            [&store, &successful_removes] {
                if(store.remove("remove_once")) {
                    successful_removes.fetch_add(1);
                }
            }
        );
    }

    for(auto &worker : workers) {
        worker.join();
    }

    check(successful_removes==1, "Concurrent DELETE on same key", failures);

    return ((failures==0) ? 0 : 1);
}