#pragma once

#include <regex>
#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <netcdf>
#include <vector>
#include <cstdlib>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <filesystem>

using namespace cv;
using namespace netCDF;
using namespace netCDF::exceptions;

double lerp(double v0, double v1, double t);
void print_progress(unsigned long current, unsigned long total, int bar_width);
std::vector<std::vector<int>> generate_colormap(const std::vector<std::vector<double>>& data, int num_colors);
std::pair<float, float> get_variable_range(const netCDF::NcVar& variable, unsigned long start, unsigned long count, unsigned long stride, float min_threshold, float max_threshold);
void create_gif(const std::string& input_filename, const std::string& output_filename, int delay);
void create_images(const std::string& input_filename, const std::string& variable_name, const std::string& output_dir, const std::string& output_format);
std::tuple<NcVar, size_t, size_t, size_t> load_netcdf_variable(NcFile &dataFile, const std::string &variable_name);
std::vector<std::vector<int>> load_colormap(const std::vector<std::vector<double>> &base_colormap, int size);
Mat create_image_for_time_step(const NcVar &var, size_t t, size_t nLat, size_t nLon, const std::vector<std::vector<int>> &colormap, float minVar, float maxVar);
void remove_existing_images(const std::string &output_folder, const std::string &variable_alias);
std::vector<std::vector<int>> get_base_colormap(const std::string& variable_alias);
