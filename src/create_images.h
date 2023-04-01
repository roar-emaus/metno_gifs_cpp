#pragma once

#include <string>
#include <map>
#include <thread>
#include <future>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <netcdf>
#include <vector>
#include <cstdlib>
#include <filesystem>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>


double lerp(double v0, double v1, double t);
void print_progress(unsigned long current, unsigned long total, int bar_width);
std::vector<std::vector<int>> generate_colormap(const std::vector<std::vector<double>>& data, int num_colors);
std::pair<float, float> get_variable_range(const netCDF::NcVar& variable, unsigned long start, unsigned long count, unsigned long stride);
void create_gif(const std::string& input_filename, const std::string& output_filename, int delay);
void visualize_variable(const std::string& input_filename, const std::string& variable_name, const std::string& output_dir, const std::string& output_format);

