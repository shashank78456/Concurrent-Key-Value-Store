#pragma once

#include <iostream>
#include <string>

using namespace std;

inline void check(bool condition, const string &name, int &failures) {
    if(condition) {
        cout << "PASS " << name << '\n';
    }
    else {
        cout << "FAILED " << name << '\n';
        failures;
    }
}