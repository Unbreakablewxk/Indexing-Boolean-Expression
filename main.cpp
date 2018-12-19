//
//  main.cpp
//  index_boolean_expression
//
//  Created by 王鑫凯 on 2018/12/16.
//  Copyright © 2018 王鑫凯. All rights reserved.
//

#include <iostream>
#include <vector>
#include "ibe.h"

int main(int argc, const char * argv[]) {
    char *filename="dnf.txt";
    vector<int> result;
    DNF test(filename);
    test.traverse();
    result=test.Find("assignment.txt");
    for (int i=0; i<result.size(); i++) {
        cout <<result[i]<< endl;
    }
}
