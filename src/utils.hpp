/*
 * This file contains a collection of useful functions
 */

#include <string>

#include "rapidcsv.h"
#include "tabulate/table.hpp"

#include "exporter.hpp"

#define N_BINS 8
#define N_BINS_SKETCH 64
#define N_SKETCH_WIDTH 512
#define N_SKETCH_DEPTH 10
#define N_QUERIES 100
#define EPSILON 0.01
#define GAMMA 0.2

std::string GetFilename(std::string path);

tabulate::Table GetColumnInfo(rapidcsv::Document &doc);

void DoExperiment(std::vector<std::string> const &fstCol, std::vector<std::string> const &sndCol, Exporter &expo);
