#include "InventoryManager.h"

// Constructor for InventoryManager
InventoryManager::InventoryManager() {
    items.emplace_back(Item(10, "Coffee mug"));
    items.emplace_back(Item(20, "Laptop"));
    items.emplace_back(Item(30, "Compass"));
    items.emplace_back(Item(40, "Notebook"));
    items.emplace_back(Item(50, "Desk lamp"));
    items.emplace_back(Item(60, "Chainsaw"));
    items.emplace_back(Item(70, "Watermelon"));
    items.emplace_back(Item(80, "Phone charger"));
    items.emplace_back(Item(90, "Chair"));
    items.emplace_back(Item(100, "Rubber duck"));
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
        for (Item& item : items) {
            if (item.getId() == itemId) {
                mtx.lock();
                item.borrow(username);
                mtx.unlock();
                return;
            }
        }
    } catch (const char* msg) {
        cerr << msg << "\n";
        mtx.unlock();
        return;
    }

    // Item with ID itemId does not exist
    throw std::runtime_error("Item with ID=" + to_string(itemId) + " does not exist.");
}