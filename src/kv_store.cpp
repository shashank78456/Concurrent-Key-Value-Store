#include <kv_store.hpp>

void KVStore::set(const string &key, const string &value) {
    lock_guard<mutex> lock(_mutex);

    data[key] = Entry(value);
}

optional<string> KVStore::get(const string &key) {
    lock_guard<mutex> lock(_mutex);

    auto it = data.find(key);

    if(it==data.end()) {
        return nullopt;
    }
    
    return it->second.value;
}

bool KVStore::remove(const string &key) {
    lock_guard<mutex> lock(_mutex);

    return data.erase(key) > 0;
}