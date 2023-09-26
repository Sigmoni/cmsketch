#include "utils.hpp"
#include "count_min_sketch.h"
#include <sstream>
#include <iostream>

std::string GetFilename(std::string path)
{
    std::stringstream ss(path);
    std::string str;

    while (std::getline(ss, str, '/'))
        ;

    std::stringstream sss(str);
    std::string filename;
    getline(sss, filename, '.');

    return filename;
}

bool isInt(std::string const &str)
{
    return !str.empty() && std::all_of(str.begin(), str.end(), ::isdigit);
}

bool isFloat(std::string const &str)
{
    char *p;
    strtod(str.c_str(), &p);
    return *p == 0;
}

tabulate::Table GetColumnInfo(rapidcsv::Document &doc)
{
    tabulate::Table columnInfoTable;

    columnInfoTable.add_row({"Name", "Type", "#DV"});
    for (int i = 0; i < doc.GetColumnCount(); i++)
    {
        std::string colName = doc.GetColumnNames()[i];
        std::string sampleValue = doc.GetCell<std::string>(i, 0);
        std::string colType;
        if (isInt(sampleValue))
            colType = "Integer";
        else if (isFloat(sampleValue))
            colType = "Float";
        else
            colType = "Literal";

        std::vector<std::string> col = doc.GetColumn<std::string>(colName);
        std::sort(col.begin(), col.end());
        int dv = 1;
        for (int j = 1; j < col.size(); j++)
        {
            if (col[j] != col[j - 1])
                dv++;
        }

        columnInfoTable.add_row({colName, colType, std::to_string(dv)});
    }
    columnInfoTable.row(0).format().font_color(tabulate::Color::yellow);

    return columnInfoTable;
}

float GetSumHist(std::vector<std::vector<int>> const &dist, float qx, float qy, int total)
{
    int px = std::trunc(qx * N_BINS);
    int py = std::trunc(qy * N_BINS);
    float rx = qx * N_BINS - px;
    float ry = qy * N_BINS - py;

    float cnt = 0;
    for (int i = 0; i < px; i++)
    {
        for (int j = 0; j < py; j++)
        {
            cnt += dist[i][j];
        }
    }

    for (int i = 0; i < px; i++)
    {
        cnt += float(dist[i][py]) * ry;
    }

    for (int i = 0; i < py; i++)
    {
        cnt += float(dist[px][i]) * rx;
    }

    cnt += dist[px][py] * rx * ry;

    return cnt / float(total);
}

float GetSketchAns(CountMinSketch *cms, float qx, float qy, int total)
{
    int px = std::trunc(qx * N_BINS_SKETCH);
    int py = std::trunc(qy * N_BINS_SKETCH);
    float rx = qx * N_BINS_SKETCH - px;
    float ry = qy * N_BINS_SKETCH - py;

    float cnt = 0;
    for (int i = 0; i < px; i++)
    {
        for (int j = 0; j < py; j++)
        {
            cnt += cms_check(cms, std::to_string(i * N_BINS_SKETCH + j).c_str());
        }
    }

    for (int i = 0; i < px; i++)
    {
        cnt += cms_check(cms, std::to_string(i * N_BINS_SKETCH + py).c_str()) * ry;
    }

    for (int i = 0; i < py; i++)
    {
        cnt += cms_check(cms, std::to_string(px * N_BINS_SKETCH + i).c_str()) * rx;
    }

    cnt += cms_check(cms, std::to_string(px * N_BINS_SKETCH + py).c_str()) * rx * ry;

    return cnt / float(total);
}

float GetTrueAns(std::vector<float> val_fst, std::vector<float> val_snd, float qx, float qy, int total)
{
    int cnt = 0;
    for (int i = 0; i < total; i++)
    {
        if (val_fst[i] <= qx && val_snd[i] <= qy)
            cnt++;
    }

    return cnt / float(total);
}

void DoExperiment(std::vector<std::string> const &col_fst, std::vector<std::string> const &col_snd, Exporter &expo)
{
    float max_fst, min_fst, max_snd, min_snd;
    std::vector<float> val_fst;
    std::vector<float> val_snd;
    for (int i = 0; i < col_fst.size(); i++)
    {
        try
        {
            float tmp_fst = std::stof(col_fst[i]);
            float tmp_snd = std::stof(col_snd[i]);
            val_fst.push_back(tmp_fst);
            val_snd.push_back(tmp_snd);
            max_fst = (i == 0 || max_fst < tmp_fst) ? tmp_fst : max_fst;
            min_fst = (i == 0 || min_fst > tmp_fst) ? tmp_fst : min_fst;
            max_snd = (i == 0 || max_snd < tmp_snd) ? tmp_snd : max_snd;
            min_snd = (i == 0 || min_snd > tmp_snd) ? tmp_snd : min_snd;
        }
        catch (std::exception &err)
        {
            continue;
        }
    }
    int total = val_fst.size();

    for (int i = 0; i < total; i++)
    {
        val_fst[i] = (val_fst[i] - min_fst) / (max_fst - min_fst);
        val_snd[i] = (val_snd[i] - min_snd) / (max_snd - min_snd);
    }

    std::vector<std::vector<int>> dist(N_BINS, std::vector<int>(N_BINS, 0));
    CountMinSketch cms;
    cms_init(&cms, N_SKETCH_WIDTH, N_SKETCH_DEPTH);

    std::cout << "The size of CW matrix: " << N_SKETCH_WIDTH << "*" << N_SKETCH_DEPTH << std::endl;
    for (int i = 0; i < total; i++)
    {
        int x = std::trunc(val_fst[i] * N_BINS_SKETCH);
        if (x == N_BINS_SKETCH)
            x--;
        int y = std::trunc(val_snd[i] * N_BINS_SKETCH);
        if (y == N_BINS_SKETCH)
            y--;
        cms_add(&cms, std::to_string(x * N_BINS_SKETCH + y).c_str());
    }

    for (int i = 0; i < total; i++)
    {
        int x = std::trunc(val_fst[i] * N_BINS);
        if (x == N_BINS)
            x--;
        int y = std::trunc(val_snd[i] * N_BINS);
        if (y == N_BINS)
            y--;
        // std::cout << x << ", " << y << std::endl;
        dist[x][y]++;
    }
    std::cout << "The size of 2D Histogram: " << N_BINS << "*" << N_BINS << std::endl;

    float mse_hist = 0;
    float mse_sketch = 0;
    for (int i = 0; i < N_QUERIES; i++)
    {
        float qx = std::rand() / (double)RAND_MAX;
        float qy = std::rand() / (double)RAND_MAX;
        // std::cout << "Query(qx, qy) = (" << qx << ", " << qy << ")" << std::endl;

        float ans_hist = GetSumHist(dist, qx, qy, total);
        // std::cout << "Answer for 2D Histogram: " << ans_sum << std::endl;
        float ans_sketch = GetSketchAns(&cms, qx, qy, total);
        // std::cout << "Answer for CM Sketch: " << ans_sketch << std::endl;
        float ans_real = GetTrueAns(val_fst, val_snd, qx, qy, total);
        // std::cout << "Ground Truth: " << ans_real << std::endl;
        // std::cout << std::endl;

        float tmp1 = (ans_hist - ans_real) * (ans_hist - ans_real);
        float tmp2 = (ans_sketch - ans_real) * (ans_sketch - ans_real);

        expo.AddData(tmp1, tmp2);
        mse_hist += tmp1;
        mse_sketch += tmp2;
    }

    mse_hist /= N_QUERIES;
    mse_sketch /= N_QUERIES;

    std::cout << "The MSE loss of Sketch: " << mse_sketch << std::endl;
    std::cout << "The MSE loss of 2D Histogram: " << mse_hist << std::endl;
    std::cout << std::endl;

    cms_destroy(&cms);
}
