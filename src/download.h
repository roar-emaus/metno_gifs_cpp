#pragma once

#include <sys/stat.h>
#include <curl/curl.h>
#include <ctime>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <chrono>
#include <string>

bool download_if_newer(const std::string &output_path);
