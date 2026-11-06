## Concurrent KV Store

Concurrent in-memory key-value store with a sharded hash table, thread-safe operations, TCP server, and a fixed-size thread pool. WAL persistence planned.

1. Configure the project: `cmake -S . -B build`
2. Build `cmake --build build`
3. Start the server `./build/kvstore`
4. Connect to the server `nc localhost 6379`
   
### Commands
```
SET <key> <value>
GET <key>
DELETE <key>
```