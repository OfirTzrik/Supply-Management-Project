Supply Management Project (Rent It) by OfirTzrik and OmerAzrai.

Compile and run server with:
```bash
g++ server.cpp InventoryManager.cpp Item.cpp -o server
./server 5555
```
Compile and run the client with:
```bash
g++ client.cpp -o client
./client 127.0.0.1 5555
```
