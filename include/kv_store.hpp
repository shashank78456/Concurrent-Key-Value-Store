#pragma once

#include "entry.hpp"

#include <array>
#include <optional>
#include <string>
#include <unordered_map>
#include <mutex>

class KVStore {
private:
    static constexpr size_t NUM_SHARDS = 64;

    struct Shard {
        unordered_map<string, Entry> data;
        mutable mutex mtx;
    };

    array<Shard, NUM_SHARDS> shards;

    size_t get_shard_idx(const string&key) const;

public:
    void set(const string &key, const string &value);

    optional<string> get(const string &key) const;

    bool remove(const string &key);
};