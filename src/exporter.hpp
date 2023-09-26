#pragma once

#include <iostream>
#include <fstream>
#include <string>

class Exporter
{
private:
    std::ofstream os;
    std::string exp_name;

public:
    Exporter(std::string path);

    void SetExperimentName(std::string name);

    void AddData(float mse_hist, float mse_sketch);

    ~Exporter();
};