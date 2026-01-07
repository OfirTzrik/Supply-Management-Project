#ifndef ITEM_H
#define ITEM_H
#include <iostream>
#include <exception>
using namespace std;

class Item
{
private:
    int id;
    string name ;
    bool isBorrowed ;
    string borrowedBY;
public:
    Item(int id , const string& name);
    int getId();
    string& getName();
    bool isAvailable();
    bool isAvailable();
    string& getBorrower();
    void Borrow(const string& username);
    void returnBack(const string& username);
    ~Item();
};


#endif