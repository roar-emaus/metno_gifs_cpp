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
std::pair<float, float> get_temperature_range(const NcVar &tempVar, size_t nTime, size_t nLat, size_t nLon) {
    std::vector<size_t> start(tempVar.getDimCount(), 0);
    std::vector<size_t> count = {1, nLat, nLon};
    std::vector<float> tempSlice(nLat * nLon);

    float globalMinTemp = std::numeric_limits<float>::max();
    float globalMaxTemp = std::numeric_limits<float>::lowest();

    // Process the data in time slices
    for (size_t t = 0; t < nTime; t++) {
        start[0] = t;
        start[1] = 0;
        start[2] = 0;
        count[0] = 1;
        count[1] = nLat;
        count[2] = nLon;
        tempVar.getVar(start, count, tempSlice.data());

        float sliceMinTemp = *std::min_element(tempSlice.begin(), tempSlice.end());
        float sliceMaxTemp = *std::max_element(tempSlice.begin(), tempSlice.end());

        globalMinTemp = std::min(globalMinTemp, sliceMinTemp);
        globalMaxTemp = std::max(globalMaxTemp, sliceMaxTemp);
    }

    return std::make_pair(globalMinTemp, globalMaxTemp);
}

int main() {
    try {
        // Open the NetCDF file.
        NcFile dataFile("met_forecast_1_0km_nordic_latest.nc", NcFile::read);

        // Get the temperature variable.
        NcVar tempVar = dataFile.getVar("air_temperature_2m");
        if (tempVar.isNull()) {
            std::cerr << "Error: 'temperature' variable not found in the NetCDF file." << std::endl;
            return 1;
        }

        // Get the dimensions of the temperature variable.
        NcVar latVar = dataFile.getVar("y");
        NcVar lonVar = dataFile.getVar("x");
        NcVar timeVar = dataFile.getVar("time");

        size_t nLat = latVar.getDim(0).getSize();
        size_t nLon = lonVar.getDim(0).getSize();
        size_t nTime = timeVar.getDim(0).getSize();

        std::vector<size_t> start(tempVar.getDimCount(), 0);
        std::vector<size_t> count = {1, nLat, nLon};

        std::vector<float> tempSlice(nLat * nLon);

        //std::vector<std::vector<int>> jet = {
        //    {0, 0, 128}, {0, 0, 255}, {0, 128, 255}, {0, 255, 255},
        //    {128, 255, 128}, {255, 255, 0}, {255, 128, 0}, {255, 0, 0}
        //};
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

        // Define the temperature range and the corresponding color indices
        std::pair<float, float> tempRange = get_temperature_range(tempVar, nTime, nLat, nLon);
        
        float minTemp = tempRange.first;
        float maxTemp = tempRange.second;
        float tempDiff = maxTemp - minTemp;

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
            tempVar.getVar(start, count, tempSlice.data());
            start[0] = t;
            tempVar.getVar(start, count, tempSlice.data());
        
            for (size_t y = 0; y < nLat; y++) {
                for (size_t x = 0; x < nLon; x++) {
                    // Map the temperature value to a color index
                    int colorIndex = static_cast<int>((tempSlice[y * nLon + x] - minTemp) / tempDiff* (viridis.size() - 1));
        
                    // Clamp the colorIndex to the valid range
                    colorIndex = std::max(0, std::min(colorIndex, static_cast<int>(viridis.size() - 1)));
        
                    // Set the RGB value of the pixel
                    Vec3b color(viridis[colorIndex][0], viridis[colorIndex][1], viridis[colorIndex][2]);
                    img.at<Vec3b>(nLat - y - 1, x) = color;
                }
            }

            // PNG FILE
            // Save the image to disk with compression
            //std::ostringstream filenameStream;
            //filenameStream << "output/temperature_timestep_" << std::setw(2) << std::setfill('0') << t << ".png";
            //std::string filename = filenameStream.str();
            //
            //// Set the compression parameters for the PNG format
            //std::vector<int> compression_params;
            //compression_params.push_back(cv::IMWRITE_PNG_COMPRESSION);
            //compression_params.push_back(0); // Compression level (0-9), higher value means more compression but slower

            //imwrite(filename, img, compression_params);

            // JPEG FILE
            // Save the image to disk with compression
            std::ostringstream filenameStream;
            filenameStream << "output/temperature_timestep_" << std::setw(2) << std::setfill('0') << t << ".jpg";
            std::string filename = filenameStream.str();
            
            // Set the compression parameters for the JPEG format
            std::vector<int> compression_params;
            compression_params.push_back(cv::IMWRITE_JPEG_QUALITY);
            compression_params.push_back(75); // Quality (0-100), lower value means more compression but lower quality
            imwrite(filename, img, compression_params);

        }
        //
        // Display the image
        //namedWindow("Temperature Visualization", WINDOW_KEEPRATIO | WINDOW_GUI_EXPANDED);
        //resizeWindow("Temperature Visualization", nLon, nLat); // Set window size
        //imshow("Temperature Visualization", img);
        //
        //// Wait for a key press and close the window
        //waitKey(0);
        //destroyAllWindows();


    } catch (const NcException &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
