#include <kv_store.hpp>

size_t KVStore::get_shard_idx(const string &key) const {
    return hash<string>{}(key)%NUM_SHARDS;
}

void KVStore::set(const string &key, const string &value) {
    Shard &shard = shards[get_shard_idx(key)];
    lock_guard<mutex> lock(shard.mtx);

    shard.data[key] = Entry(value);
}

optional<string> KVStore::get(const string &key) const {
    const Shard &shard = shards[get_shard_idx(key)];
    lock_guard<mutex> lock(shard.mtx);

    auto it = shard.data.find(key);

    if(it==shard.data.end()) {
        return nullopt;
    }
    
    return it->second.value;
}

bool KVStore::remove(const string &key) {
    Shard &shard = shards[get_shard_idx(key)];
    lock_guard<mutex> lock(shard.mtx);

    return shard.data.erase(key) > 0;
}