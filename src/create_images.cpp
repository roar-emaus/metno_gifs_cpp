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

std::vector<std::vector<int>> generate_colormap(const std::vector<std::vector<double>> &base_colormap, int size){
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

std::pair<float, float> get_variable_range(const NcVar &var, size_t nTime, size_t nLat, size_t nLon) {
  size_t num_dims = var.getDimCount();
  std::vector<size_t> start(var.getDimCount(), 0);
  std::vector<size_t> count = {1, nLat, nLon};
  std::vector<float> slice(nLat * nLon);
  float globalMin = std::numeric_limits<float>::max();
  float globalMax = std::numeric_limits<float>::lowest();

  std::cout << "Finding min/max values" << std::endl;
  for (size_t t = 0; t < nTime; t++) {
    start[0] = t;
    start[1] = 0;
    start[2] = 0;
    count[0] = 1;
    count[1] = nLat;
    count[2] = nLon;
    var.getVar(start, count, slice.data());

    float sliceMin = *std::min_element(slice.begin(), slice.end());
    float sliceMax = *std::max_element(slice.begin(), slice.end());

    globalMin = std::min(globalMin, sliceMin);
    globalMax = std::max(globalMax, sliceMax);

    print_progress(t+1, nTime);
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

std::vector<std::vector<int>> load_colormap(const std::vector<std::vector<double>> &base_colormap, int size) {
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

  Mat img(nLat, nLon, CV_8UC3);
  for (size_t y = 0; y < nLat; y++) {
    for (size_t x = 0; x < nLon; x++) {
      int colorIndex = static_cast<int>((tempSlice[y * nLon + x] - minVar) / (maxVar - minVar) * (colormap.size() - 1));
      colorIndex = std::max(0, std::min(colorIndex, static_cast<int>(colormap.size() - 1)));
      Vec3b color(colormap[colorIndex][0], colormap[colorIndex][1], colormap[colorIndex][2]);
      img.at<Vec3b>(nLat - y - 1, x) = color;
    }
  }

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

void visualize_variable(const std::string &filename, const std::string &variable_name, const std::string &variable_alias, const std::string &output_folder) {
  std::cout << "Creating images for " << variable_alias << std::endl;

  try {
    NcFile dataFile(filename, NcFile::read);
    NcVar var;
    size_t nTime, nLat, nLon;
    std::tie(var, nTime, nLat, nLon) = load_netcdf_variable(dataFile, variable_name);
    
    std::vector<std::vector<double>> viridis_base = {
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

    int colormap_size = 256;
    std::vector<std::vector<int>> viridis = load_colormap(viridis_base, colormap_size);

    std::pair<float, float> varRange = get_variable_range(var, nTime, nLat, nLon);
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
