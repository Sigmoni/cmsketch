#include <string>
#include <iostream>

#include "rapidcsv.h"
#include "tabulate/table.hpp"

#include "utils.hpp"
#include "exporter.hpp"

int main(int argc, char *argv[])
{
    rapidcsv::Document doc(argv[1], rapidcsv::LabelParams(), rapidcsv::SeparatorParams(';'));
    std::string filename = GetFilename(argv[1]);

    std::cout << "Read file: " << filename << ".csv" << std::endl;
    int cols = doc.GetColumnCount();
    int rows = doc.GetRowCount();
    std::cout << "Shape(Cols * Rows): " << cols << "*" << rows << std::endl;
    std::cout << std::endl;

    std::cout << "Column Information" << std::endl;
    tabulate::Table colInfoTable = GetColumnInfo(doc);
    std::cout << colInfoTable << std::endl;
    std::cout << std::endl;

    std::srand(std::time(NULL));

    Exporter expo("results_" + std::to_string(N_SKETCH_WIDTH) + "*" + std::to_string(N_SKETCH_DEPTH) + "_" + filename + ".csv");

    for (int i = 0; i < cols - 1; i++)
    {
        std::string type_fst = colInfoTable[i + 1][1].get_text();
        if (type_fst != "Literal")
        {
            for (int j = i + 1; j < cols; j++)
            {
                std::string type_snd = colInfoTable[j + 1][1].get_text();
                if (type_snd != "Literal") {
                    std::string columnset_name =
                        doc.GetColumnName(i) + " * " + doc.GetColumnName(j);
                    std::cout << "Experiments on " << columnset_name << std::endl;

                    std::vector<std::string> col_fst = doc.GetColumn<std::string>(i);
                    std::vector<std::string> col_snd = doc.GetColumn<std::string>(j);

                    expo.SetExperimentName(columnset_name);
                    DoExperiment(col_fst, col_snd, expo);
                }
            }
        }
    }
}