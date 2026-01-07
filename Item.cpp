#include "Item.h"
Item::Item(int id ,const string& name):
    id(id) ,name(name) , isBorrowed(false) , borrowedBY(nullptr){}

int Item::getId(){
    return id;
}
string& Item::getName(){
    return name;
}
bool Item::isAvailable(){
    return isBorrowed;
}
string& Item::getBorrower(){
    return borrowedBY;
}

void Item::borrow(const string& username){
    if (isBorrowed) throw runtime_error("Item is allready borrowed");
    if (username.empty()) throw invalid_argument("username is in valide");
    isBorrowed = true;
    borrowedBY = username;
}
void Item::returnBack(const string& username){
    if (isBorrowed == false) throw runtime_error("The Item is not borrowed");
    if (username != borrowedBY) throw invalid_argument("you are not the one how borrowed it");
    isBorrowed = false;
    borrowedBY = nullptr;
}
string Item::toString(){
    string s1 = to_string(id);
    if (isBorrowed){
        return "ID: " + s1 + " Borrowed By: " + borrowedBY;
    } 
    return "ID: " + s1 +  "Item is free";
}