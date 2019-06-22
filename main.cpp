//
// Created by shirogiro on 22.06.19.
//

#include <iostream>
#include "list.h"
int main() {
    list<int> l1;
    l1.push_back(2);
    l1.clear();
    std::cout << l1.empty() << std::endl;
    return 0;
}