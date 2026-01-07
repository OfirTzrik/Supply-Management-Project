#include "InventoryManager.h"

int main() {
    InventoryManager inv;
    // Item itm(10, "omer");
    // cout << itm.toString() << "\n";

    cout << inv.listItems();
    
    inv.borrowItem(70, "Ofir");
    
    cout << "\n" + inv.listItems();

    inv.waitUntilAvailable(70, "Omer");

    cout << "\n" + inv.listItems();

    return 0;
}