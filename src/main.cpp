#include "kv_store.hpp"

#include <iostream>

int main() {
    KVStore kvstore;

    kvstore.set("name", "Shashank");
    kvstore.set("language", "C++");

    auto name = kvstore.get("name");

    if(name.has_value()) {
        cout << "name: " << name.value() << '\n';
    }

    kvstore.remove("name");
    if(!kvstore.get("name").has_value()) {
        cout << "name was removed" << '\n';
    }

    return 0;
}