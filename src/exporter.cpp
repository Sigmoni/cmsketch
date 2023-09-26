#include "exporter.hpp"

Exporter::Exporter(std::string path)
{
    this->os.open(path, std::ios::out | std::ios::trunc);
    os << "\"exp_name\";\"mse_hist\";\"mse_sketch\"" << std::endl;
}

void Exporter::SetExperimentName(std::string name)
{
    this->exp_name = name;
}

void Exporter::AddData(float mse_hist, float mse_sketch)
{
    os << this->exp_name << ";" << std::to_string(mse_hist) << ";" << std::to_string(mse_sketch) << std::endl;
}

Exporter::~Exporter()
{
    os.close();
}
