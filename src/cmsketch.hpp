#pragma once

#define LONG_PRIME 4294967311l
#include <string>

class CountMinSketch {
private:
    // The width and depth of CW matrix
    unsigned int w, d;

    // ε
    // This parameter determines the error rate
    float eps;

    // γ
    // This parameter determines the probability for accuracy 
    float gamma;

    // These parameter are used to generate hash functions.
    unsigned int aj, bj;

    // The total count of elements
    unsigned int total;

    // The array of arrays of counters
    int **c;

    // Array of hash values for a particular item
    int **h;

    // Function for generating new aj and bj
    void genab(int **h, int i);

public:
    // Constructor
    CountMinSketch(int w, int d);
    
    // Update the item by count c
    void update(int item, int c);

    // Estimate the counting of item
    unsigned int estimate(int item);

    // Return the shape of the CW matrix
    std::string space();

    // Destructor
    ~CountMinSketch();
};