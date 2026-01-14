#include "InventoryManager.h"

// Constructor for InventoryManager with 15 different items
InventoryManager::InventoryManager() {
    items.emplace_back(10, "Coffee mug");
    items.emplace_back(20, "Laptop");
    items.emplace_back(30, "Compass");
    items.emplace_back(40, "Notebook");
    items.emplace_back(50, "Desk lamp");
    items.emplace_back(60, "Chainsaw");
    items.emplace_back(70, "Watermelon");
    items.emplace_back(80, "Phone charger");
    items.emplace_back(90, "Chair");
    items.emplace_back(100, "Rubber duck");
    items.emplace_back(110, "Rubik's cube");
    items.emplace_back(120, "Coke zero");
    items.emplace_back(130, "Shawarma");
    items.emplace_back(140, "Keyboard");
    items.emplace_back(150, "Microwave");
}

// Get a list of the items in the inventory and their current status
string InventoryManager::listItems() const {
    string curr_inv = "OK LIST " + to_string(items.size()) + "\n";

    // Update the list with each and every item
    for (Item item : items) {
        curr_inv += item.toString() + "\n";
    }

    return curr_inv;
}

// Borrow an item from the inventory if it exists and not already borrowed
void InventoryManager::borrowItem(int itemId, const string& username) {
    // Search for the item with ID itemId in the inventory
    try {
        Item& itm = findItemById(itemId);
        mtx.lock();
        itm.borrow(username);
        mtx.unlock();
        return;
    } catch (const runtime_error& msg) {
        cerr << msg.what() << "\n";
        mtx.unlock();
        return;
    }

    // Item with ID itemId does not exist
    throw runtime_error("Item with ID=" + to_string(itemId) + " does not exist.");
}

// Return a borrwed item back to the inventory
void InventoryManager::returnItem(int itemId, const string& username) {
    // Search for the item with ID itemId in the inventory
    try {
        Item& itm = findItemById(itemId);
        mtx.lock();
        itm.returnBack(username);
        mtx.unlock();
        return;
    } catch (const runtime_error& msg) {
        cerr << msg.what() << "\n";
        mtx.unlock();
        return;
    }

    // Item with ID itemId does not exist
    throw runtime_error("Item with ID=" + to_string(itemId) + " does not exist.");
}

// Keep the client waiting until an item is available again
void InventoryManager::waitUntilAvailable(int itemId, const string& username) {
    try {
        Item& itm = findItemById(itemId);

        // Client will keep waiting until available
        unique_lock<mutex> ulock(mtx);

        while(!itm.isAvailable()) {
            cv.wait(ulock);
        }

    } catch (const runtime_error& msg) {
        cerr << msg.what() << "\n";
        return;
    }

    
}

// Search for an item given its ID in the inventory
Item& InventoryManager::findItemById(int itemId) {
    for (Item& item : items) {
        if (item.getId() == itemId) {
            return item;
        }
    }

    throw runtime_error("ERR NOT_FOUND item");
}