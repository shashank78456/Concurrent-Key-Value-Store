#pragma once

#include "entry.hpp"

#include <optional>
#include <string>
#include <unordered_map>
#include <mutex>

class KVStore {
private:
    unordered_map<string, Entry> data;
    mutable mutex _mutex;

public:
    void set(const string &key, const string &value);

    optional<string> get(const string &key);

    bool remove(const string &key);
};