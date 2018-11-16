#ifndef SA_H
#define SA_H

#include <iostream>
#include <vector>
#include <cmath>
using namespace std;

template <class T>
ostream& operator<<(ostream& os, vector<T> &vec){
    os << '{';
    for(int i = 0; i < vec.size() - 1; i++){
            os << vec[i];
            os << ',';
    }
        os << vec[vec.size()-1];
        os << '}';

    return os;
}

template <class T>
ostream& operator<<(ostream& os, const vector<T> &vec){
    os << '{';
    for(int i = 0; i < vec.size() - 1; i++){
            os << vec[i];
            os << ',';
    }
        os << vec[vec.size()-1];
        os << '}';

    return os;
}
#endif