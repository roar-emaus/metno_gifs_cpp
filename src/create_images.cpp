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


using namespace cv;
using namespace netCDF;
using namespace netCDF::exceptions;


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


void visualize_variable(const std::string &filename, const std::string &variable_name, const std::string &variable_alias, const std::string &output_folder) {
    std::cout << "Creating images for " << variable_alias << std::endl;
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
            //std::vector<int> compression_params;
            //compression_params.push_back(cv::IMWRITE_JPEG_QUALITY);
            //compression_params.push_back(25); // Quality (0-100), lower value means more compression but lower quality
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
            imwrite(output_filename, downscaled_img);//, compression_params);

	    print_progress(t + 1, nTime);
        }
	std::cout << std::endl;
    } catch (const NcException &e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <input_filename> <variable_alias>" << std::endl;
        return 1;
    }

    std::string input_filename = argv[1];

    std::map<std::string, std::string> variable_aliases = {
        {"temperature", "air_temperature_2m"},
        {"radiation", "integral_of_surface_downwelling_shortwave_flux_in_air_wrt_time"}
    };

    std::string variable_alias = argv[2];
    if (variable_aliases.find(variable_alias) == variable_aliases.end()) {
        std::cerr << "Error: Invalid variable alias. Valid options are 'temperature' and 'radiation'." << std::endl;
        return 1;
    }

    std::string variable_name = variable_aliases[variable_alias];
    std::string output_folder = "output/" + variable_alias;

    visualize_variable(input_filename, variable_name, variable_alias, output_folder);
    create_gif(output_folder + "/*.jpg", output_folder + "/" + variable_alias + ".gif", 10);

    return 0;
}
