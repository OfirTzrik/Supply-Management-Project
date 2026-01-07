#pragma once

#include <string>
#include <vector>
#include <mutex>
#include <exception>
#include <condition_variable>
#include "Item.h"

using namespace std;

class InventoryManager {
private:
    vector<Item> items {};
    mutex mtx;
    condition_variable cv;

public:
    InventoryManager();
    
    string listItems() const;
    
    void borrowItem(int itemId, const string& username);

    void returnItem(int itemId, const string& username);

    void waitUntilAvailable(int itemId, const string& username);

    Item& findItemById(int itemId);
};