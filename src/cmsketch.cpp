#include <iostream>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <limits>

#include "cmsketch.hpp"

void CountMinSketch::genab(int **h, int i)
{
    h[i][0] = int(float(rand())*float(LONG_PRIME)/float(RAND_MAX) + 1);
    h[i][1] = int(float(rand())*float(LONG_PRIME)/float(RAND_MAX) + 1);
}

CountMinSketch::CountMinSketch(int w, int d)
{
    this->w = w;
    this->d = d;

    this->eps = 2 / (double) this->w;
    this->gamma = 1 - (1 / std::pow(2, this->d));

    this->total = 0;

    this->c = new int *[d];
    unsigned int i, j;
    for (i = 0; i < this->d; i++)
    {
        this->c[i] = new int[w];
        for (j = 0; j < this->w; j++)
        {
            this->c[i][j] = 0;
        }
    }

    std::srand(std::time(NULL));
    this->h = new int *[d];
    for (i = 0; i < this->d; i++)
    {
        this->h[i] = new int[2];
        genab(this->h, i);
    }
}

void CountMinSketch::update(int item, int c)
{
    unsigned int hashval = 0;
    for (unsigned int i = 0; i < this->d; i++)
    {
        hashval = ((long)(this->h[i][0] * item + this->h[i][1])) % LONG_PRIME % this->w;
        // std::cout << h[i][0] << "," << h[i][1] << std::endl;
        // std::cout << hashval << std::endl;
        this->c[i][hashval] += c;
    }
}

unsigned int CountMinSketch::estimate(int item)
{
    int minval = std::numeric_limits<int>::max();
    unsigned int hashval = 0;
    for (unsigned int i = 0; i < this->d; i++)
    {
        hashval = ((long)this->h[i][0] * item + this->h[i][1]) % LONG_PRIME % this->w;
        // std::cout << hashval << std::endl;
        minval = std::min(minval, this->c[i][hashval]);
    }
    return minval;
}

std::string CountMinSketch::space()
{
    std::string ans = std::to_string(this->w) + "*" + std::to_string(this->d);
    return ans;
}

CountMinSketch::~CountMinSketch()
{
    unsigned int i;
    for (i = 0; i < d; i++)
    {
        delete[] this->c[i];
    }

    delete[] this->c;

    for (i = 0; i < d; i++)
    {
        delete[] this->h[i];
    }
    delete[] this->h;
}
