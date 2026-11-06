#include <kv_store.hpp>
#include <utils.hpp>

#include <optional>
#include <string>

int main() {
    int failures = 0;
    KVStore store;

    store.set("name", "Test");
    auto value = store.get("name");
    check(value.has_value() && *value=="Test", "SET and GET", failures);

    store.set("name", "Again");
    value = store.get("name");
    check(value.has_value() && *value=="Again", "Update", failures);

    value = store.get("missing");
    check(!value.has_value(), "Missing Key", failures);

    check(store.remove("name"), "DELETE existing", failures);

    check(!store.get("name").has_value(), "GET after DELETE", failures);

    check(!store.remove("name"), "DELETE missing key", failures);

    return ((failures==0) ? 0 : 1);
}