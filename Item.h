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
    string& getBorrower();
    void borrow(const string& username);
    void returnBack(const string& username);
    string toString();
    ~Item() = default;
};


#endif