#include "create_images.h"

// Linear interpolation 
double lerp(double a, double b, double t){
  return a*(1-t) + b*t;
}

void print_progress(size_t current, size_t total, int bar_width = 50) {
  double progress = static_cast<double>(current) / total;
  int pos = static_cast<int>(bar_width * progress);

  std::cout << "[";
  for (int i = 0; i < bar_width; ++i) {
    if (i < pos) {
      std::cout << "=";
    } else if (i == pos) {
      std::cout << ">";
    } else {
      std::cout << " ";
    }
  }
  std::cout << "] " << static_cast<int>(progress * 100.0) << " %\r";
  std::cout.flush();
}

void remove_existing_images(const std::string &output_folder, const std::string &variable_alias) {
  std::regex image_pattern(variable_alias + "_\\d{2}\\.jpg");
  try {
    for (const auto &entry : std::filesystem::directory_iterator(output_folder)) {
      if (entry.is_regular_file() && std::regex_match(entry.path().filename().string(), image_pattern)) {
        std::filesystem::remove(entry.path());
      }
    }
  } catch (const std::filesystem::filesystem_error &e) {
    std::cerr << "Error while cleaning up images: " << e.what() << std::endl;
  }
}


std::vector<std::vector<int>> generate_colormap(const std::vector<std::vector<int>> &base_colormap, int size){
  std::vector<std::vector<int>> colormap(size);
  int base_size = base_colormap.size();
  for (int i = 0; i < size; ++i){
    double t = static_cast<double>(i) / (size - 1);
    int idx = static_cast<int>(t * (base_size - 1));
    double t_col = (t * (base_size - 1)) - idx;

    std::vector<int> color(3);
    for (int j = 0; j < 3; ++j){
      if (idx + 1 < base_size) {
        color[j] = static_cast<int>(lerp(base_colormap[idx][j], base_colormap[idx + 1][j], t_col));
      } else {
        color[j] = static_cast<int>(base_colormap[idx][j]);
      }
    }
    colormap[i] = color;
  }
  return colormap;
}

std::vector<std::vector<int>> get_base_colormap(const std::string& variable_alias) {
  if (variable_alias == "relative_humidity") {
    return {
      { 247, 251, 255 },
      { 221, 234, 246 },
      { 197, 218, 238 },
      { 157, 201, 224 },
      { 106, 173, 213 },
      { 65, 145, 197 },
      { 32, 112, 180 },
      { 8, 80, 154 },
      { 8, 48, 107 },
    };

  } else if (variable_alias == "precipitatoin") {
    return {
      { 247, 251, 255 },
      { 221, 234, 246 },
      { 197, 218, 238 },
      { 157, 201, 224 },
      { 106, 173, 213 },
      { 65, 145, 197 },
      { 32, 112, 180 },
      { 8, 80, 154 },
      { 8, 48, 107 },
    };

  }else if(variable_alias == "air_pressure") {
    return {
      { 5, 48, 97 },
      { 41, 113, 177 },
      { 107, 172, 208 },
      { 194, 221, 235 },
      { 247, 246, 246 },
      { 250, 204, 180 },
      { 228, 128, 101 },
      { 185, 39, 50 },
      { 103, 0, 31 },
    };
  } else if (variable_alias == "radiation") {
    return {
      { 5, 48, 97 },
      { 41, 113, 177 },
      { 107, 172, 208 },
      { 194, 221, 235 },
      { 247, 246, 246 },
      { 250, 204, 180 },
      { 228, 128, 101 },
      { 185, 39, 50 },
      { 103, 0, 31 },
    };

  } else if (variable_alias == "wind_direction") {
      return {
        { 225, 216, 226 },
        { 148, 180, 198 },
        { 97, 117, 186 },
        { 89, 42, 143 },
        { 47, 20, 54 },
        { 115, 29, 78 },
        { 178, 86, 82 },
        { 204, 163, 137 },
        { 225, 216, 225 },
      };

  } else if (variable_alias == "wind_speed") {
      return {
        { 255, 255, 255 },
        { 223, 223, 223 },
        { 191, 191, 191 },
        { 159, 159, 159 },
        { 127, 127, 127 },
        { 95, 95, 95 },
        { 63, 63, 63 },
        { 31, 31, 31 },
        { 0, 0, 0 },
      };
  } else if (variable_alias == "wind_gust") {
      return {
        { 255, 255, 255 },
        { 223, 223, 223 },
        { 191, 191, 191 },
        { 159, 159, 159 },
        { 127, 127, 127 },
        { 95, 95, 95 },
        { 63, 63, 63 },
        { 31, 31, 31 },
        { 0, 0, 0 },
      };
  } else if (variable_alias == "temperature") {
      return {
        { 94, 79, 162 },
        { 63, 150, 182 },
        { 137, 207, 164 },
        { 216, 239, 154 },
        { 254, 254, 189 },
        { 253, 210, 127 },
        { 248, 139, 81 },
        { 219, 72, 76 },
        { 158, 1, 66 },
      };
  } else if (variable_alias == "cloud_cover"){
    return {
      { 247, 251, 255 },
      { 221, 234, 246 },
      { 197, 218, 238 },
      { 157, 201, 224 },
      { 106, 173, 213 },
      { 65, 145, 197 },
      { 32, 112, 180 },
      { 8, 80, 154 },
      { 8, 48, 107 },
    };
  }
  // default color map: "viridis"
  return  {
    {68, 1, 84},
    {72, 34, 115},
    {64, 67, 135},
    {52, 94, 141},
    {41, 120, 142},
    {32, 144, 140},
    {34, 167, 132},
    {68, 190, 112},
    {121, 209, 81},
    {189, 222, 38},
    {253, 231, 36}
  };
}


std::pair<float, float> get_variable_range(const NcVar &var, size_t nTime, size_t nLat, size_t nLon, float min_threshold = -1e-10, float max_threshold = 1e20) {
  std::vector<size_t> start(var.getDimCount(), 0);
  std::vector<size_t> count = {nTime, nLat, nLon};
  std::vector<float> data(nTime * nLat * nLon);

  // Load the entire variable into memory
  var.getVar(start, count, data.data());

  float globalMin = max_threshold;
  float globalMax = min_threshold;

  std::cout << "Finding min/max values" << std::endl;
  for (size_t t = 0; t < nTime; t++) {
    for (size_t lat = 0; lat < nLat; lat++) {
      for (size_t lon = 0; lon < nLon; lon++) {
        float value = data[t * nLat * nLon + lat * nLon + lon];

        if (value >= min_threshold && value <= max_threshold) {
          globalMin = std::min(globalMin, value);
          globalMax = std::max(globalMax, value);
        }
      }
    }
    print_progress(t + 1, nTime);
  }
  std::cout << std::endl;

  return std::make_pair(globalMin, globalMax);
}

void create_gif(const std::string &input_pattern, const std::string &output_filename, int delay) {
  std::ostringstream command;
  std::cout << "Converting to GIF" << std::endl;
  command << "convert -delay " << delay << " -loop 0 " << input_pattern << " " << output_filename;

  int result = system(command.str().c_str());
  if (result != 0) {
    std::cerr << "Error: Could not create the output GIF file: " << output_filename << std::endl;
  } else {
    std::cout << "GIF created successfully: " << output_filename << std::endl;
  }
}

std::vector<std::vector<int>> load_colormap(const std::vector<std::vector<int>> &base_colormap, int size) {
  return generate_colormap(base_colormap, size);
}

Mat create_image_for_time_step(const NcVar &var, size_t t, size_t nLat, size_t nLon, const std::vector<std::vector<int>> &colormap, float minVar, float maxVar) {
  std::vector<size_t> start(var.getDimCount(), 0);
  std::vector<size_t> count = {1, nLat, nLon};
  std::vector<float> tempSlice(nLat * nLon);

  start[0] = t;
  start[1] = 0;
  start[2] = 0;
  count[0] = 1;
  count[1] = nLat;
  count[2] = nLon;
  var.getVar(start, count, tempSlice.data());
  // float normFactor = 1.0f/(maxVar - minVar);
  Mat img(nLat, nLon, CV_8UC3);
  ImageFillParallel imageFillParallel(tempSlice, colormap, minVar, maxVar, img, nLat, nLon);
  cv::parallel_for_(cv::Range(0, nLat), imageFillParallel);

  return img;
}


std::tuple<NcVar, size_t, size_t, size_t> load_netcdf_variable(NcFile &dataFile, const std::string &variable_name) {
  NcVar var = dataFile.getVar(variable_name);

  NcVar latVar = dataFile.getVar("y");
  NcVar lonVar = dataFile.getVar("x");
  NcVar timeVar = dataFile.getVar("time");

  size_t nLat = latVar.getDim(0).getSize();
  size_t nLon = lonVar.getDim(0).getSize();
  size_t nTime = timeVar.getDim(0).getSize();
  return std::make_tuple(var, nTime, nLat, nLon);
}

void create_images(const std::string &filename, const std::string &variable_name, const std::string &variable_alias, const std::string &output_folder) {

  std::cout << "Cleaning up existing images for " << variable_alias << std::endl;
  remove_existing_images(output_folder, variable_alias);

  std::cout << "Creating images for " << variable_alias << std::endl;

  try {
    NcFile dataFile(filename, NcFile::read);
    NcVar var;
    size_t nTime, nLat, nLon;
    std::tie(var, nTime, nLat, nLon) = load_netcdf_variable(dataFile, variable_name);
    
    int colormap_size = 256;
    std::vector<std::vector<int>> base_colormap = get_base_colormap(variable_alias);
    std::vector<std::vector<int>> viridis = load_colormap(base_colormap, colormap_size);
    float min_threshold = -1e10;
    float max_threshold = 1e10;
    if (variable_alias == "precipitation"){
        min_threshold = -0.01;
    }
    std::pair<float, float> varRange = get_variable_range(var, nTime, nLat, nLon, min_threshold, max_threshold);
    std::cout << "Found value range of (" << varRange.first << ", " << varRange.second << ")" << std::endl;
    float minVar = varRange.first;
    float maxVar = varRange.second;
    std::cout << "Time loop" << std::endl;
    for (size_t t = 0; t < nTime; t++) {
      Mat img = create_image_for_time_step(var, t, nLat, nLon, viridis, minVar, maxVar);

      // Downscale the image
      double scale_factor = 0.3333; // Change this value to adjust the scaling factor
      int new_width = static_cast<int>(img.cols * scale_factor);
      int new_height = static_cast<int>(img.rows * scale_factor);
      Size new_size(new_width, new_height);
      Mat downscaled_img;
      resize(img, downscaled_img, new_size, 0, 0, INTER_LINEAR);

      // Save image to disk
      std::ostringstream filenameStream;
      filenameStream << output_folder << "/" << variable_alias << "_" << std::setw(2) << std::setfill('0') << t << ".jpg";
      std::string output_filename = filenameStream.str();
      imwrite(output_filename, downscaled_img);

      print_progress(t + 1, nTime);
    }
    std::cout << std::endl;
  } catch (const NcException &e) {
    std::cerr << "Error: " << e.what() << std::endl;
  }
}
