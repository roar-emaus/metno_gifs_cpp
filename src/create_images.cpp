#include <iostream>
#include <iomanip>
#include <sstream>
#include <netcdf>
#include <vector>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

using namespace cv;
using namespace netCDF;
using namespace netCDF::exceptions;


// Linear interpolation 
double lerp(double a, double b, double t){
    return a*(1-t) + b*t;
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
    std::vector<size_t> start(var.getDimCount(), 0);
    std::vector<size_t> count = {1, nLat, nLon};
    std::vector<float> slice(nLat * nLon);

    float globalMin = std::numeric_limits<float>::max();
    float globalMax = std::numeric_limits<float>::lowest();

    // Process the data in time slices
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
    }

    return std::make_pair(globalMin, globalMax);
}

void visualize_variable(const std::string &filename, const std::string &variable_name, const std::string &output_folder) {
    try {
        NcFile dataFile(filename, NcFile::read);
        NcVar var = dataFile.getVar(variable_name);

        if (var.isNull()) {
            std::cerr << "Error: '" << variable_name << "' variable not found in the NetCDF file." << std::endl;
            return;
        }
        //
        // Get the dimensions of the temperature variable.
        NcVar latVar = dataFile.getVar("y");
        NcVar lonVar = dataFile.getVar("x");
        NcVar timeVar = dataFile.getVar("time");

        size_t nLat = latVar.getDim(0).getSize();
        size_t nLon = lonVar.getDim(0).getSize();
        size_t nTime = timeVar.getDim(0).getSize();

        std::vector<size_t> start(var.getDimCount(), 0);
        std::vector<size_t> count = {1, nLat, nLon};

        std::vector<float> tempSlice(nLat * nLon);

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
        std::vector<std::vector<int>> viridis = generate_colormap(viridis_base, colormap_size);

        std::pair<float, float> varRange = get_variable_range(var, nTime, nLat, nLon);
        float minVar = varRange.first;
        float maxVar = varRange.second;
        float varDiff = maxVar - minVar;
        //
        // Create an OpenCV Mat object to hold the first time slice of the RGB data
        Mat img(nLat, nLon, CV_8UC3);

        // Process the data in time slices
        for (size_t t = 0; t < nTime; t++) {
            start[0] = t;
            start[1] = 0;
            start[2] = 0;
            count[0] = 1;
            count[1] = nLat;
            count[2] = nLon;
            var.getVar(start, count, tempSlice.data());
        
            for (size_t y = 0; y < nLat; y++) {
                for (size_t x = 0; x < nLon; x++) {
                    // Map the temperature value to a color index
                    int colorIndex = static_cast<int>((tempSlice[y * nLon + x] - minVar) / varDiff* (viridis.size() - 1));
        
                    // Clamp the colorIndex to the valid range
                    colorIndex = std::max(0, std::min(colorIndex, static_cast<int>(viridis.size() - 1)));
        
                    // Set the RGB value of the pixel
                    Vec3b color(viridis[colorIndex][0], viridis[colorIndex][1], viridis[colorIndex][2]);
                    img.at<Vec3b>(nLat - y - 1, x) = color;
                }
            }

            // JPEG FILE
            // Set the compression parameters for the JPEG format
            std::vector<int> compression_params;
            compression_params.push_back(cv::IMWRITE_JPEG_QUALITY);
            compression_params.push_back(25); // Quality (0-100), lower value means more compression but lower quality

            // Save image to disk
            std::ostringstream filenameStream;
            filenameStream << output_folder << "/" << variable_name << "_" << std::setw(2) << std::setfill('0') << t << ".jpg";
            std::string output_filename = filenameStream.str();
            imwrite(output_filename, img, compression_params);

        }
    } catch (const NcException &e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

int main() {
    std::string input_filename = "met_forecast_1_0km_nordic_latest.nc";
    std::string output_folder = "output/temperature";
    std::string variable_name = "air_temperature_2m";
    //std::string output_folder = "output/radiation";
    //std::string variable_name = "integral_of_surface_downwelling_shortwave_flux_in_air_wrt_time";
    visualize_variable(input_filename, variable_name, output_folder);

    return 0;
}
