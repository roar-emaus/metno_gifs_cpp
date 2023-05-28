#include <map>
#include <chrono>
#include "create_images.h"
#include "download.h"

bool parse_arguments(int argc, char *argv[], std::string& input_file, std::string& variable, std::string& output_folder, bool& no_download) {
    for (int i = 1; i < argc; i += 2) {
        if (i + 1 >= argc) {
            std::cerr << "Error: Missing value for argument " << argv[i] << std::endl;
            return false;
        }
        if (strcmp(argv[i], "--input") == 0) {
            input_file = argv[i + 1];
        } else if (strcmp(argv[i], "--var") == 0) {
            variable = argv[i + 1];
        } else if (strcmp(argv[i], "--output") == 0) {
            output_folder = argv[i + 1];
        } else if (strcmp(argv[i], "--no_download") == 0) {
            no_download = false;
            i--;
        } else {
            std::cerr << "Error: Unknown argument " << argv[i] << std::endl;
            return false;
        }
    }

    if (input_file.empty()) {
        input_file = "/input/metno_pp.nc";
    }

    if (output_folder.empty()) {
        output_folder = "/output";
    }

    return true;
}

void create_variable_images(const std::string& input_file, const std::string& variable, const std::string& alias, const std::filesystem::path& output_folder) {
    std::filesystem::create_directories(output_folder);
    create_images(input_file, variable, alias, output_folder.string());
}

void create_variable_gif(const std::string& alias, const std::filesystem::path& output_folder){
    const std::filesystem::path input_pattern = output_folder / "*.jpg";
    const std::filesystem::path output_filename = output_folder / (alias + ".gif");
    create_gif(input_pattern.string(), output_filename.string(), 10);
}


int main(int argc, char *argv[]) {
    std::string input_file;
    std::string variable;
    std::string output_folder;
    bool no_download = false;

    if (!parse_arguments(argc, argv, input_file, variable, output_folder, no_download)) {
        return 1;
    }

    std::map<std::string, std::string> variable_aliases = {
        {"temperature", "air_temperature_2m"},
        {"radiation", "integral_of_surface_downwelling_shortwave_flux_in_air_wrt_time"},
        {"wind_direction", "wind_direction_10m"},
        {"wind_speed", "wind_speed_10m"},
        {"wind_gust", "wind_speed_of_gust"},
        {"cloud_cover", "cloud_area_fraction"},
        {"air_pressure", "air_pressure_at_sea_level"},
        {"relative_humidity", "relative_humidity_2m"},
        {"precipitation", "precipitation_amount"}
    };

    if (!no_download) {
        // Capture the start time
        auto download_start_time = std::chrono::high_resolution_clock::now();
        if (download_if_newer(input_file)) {
            std::cout << "Downloaded a newer version of the dataset." << std::endl;
        } else {
            std::cout << "Local dataset is already up-to-date." << std::endl;
        }
        auto download_end_time = std::chrono::high_resolution_clock::now();
        auto download_duration = std::chrono::duration<double>(download_end_time - download_start_time).count();
        std::cout << std::fixed << std::setprecision(3) << "Download time: " << download_duration << " m" << std::endl;
    }
    if (!variable.empty()) {
        auto start_time = std::chrono::high_resolution_clock::now();
        auto it = variable_aliases.find(variable);
        if (it != variable_aliases.end()) {
            std::filesystem::path variable_output_folder = std::filesystem::path(output_folder) / it->first;
            create_variable_images(input_file, it->second, it->first, variable_output_folder);
            create_variable_gif(it->first, variable_output_folder);
        } else {
            std::cerr << "Error: Invalid variable name provided." << std::endl;
            return 1;
        }
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double>(end_time - start_time).count();
        std::cout << std::fixed << std::setprecision(3)<< "Execution time: " << duration << " s" << std::endl;
    } else {
        for (const auto& alias_pair : variable_aliases) {
            auto start_time = std::chrono::high_resolution_clock::now();
            const std::string& variable_name = alias_pair.first;
            const std::string& variable_alias = alias_pair.second;
            std::filesystem::path variable_output_folder = std::filesystem::path(output_folder) / variable_name;
            create_variable_images(input_file, variable_alias, variable_name, variable_output_folder);
            create_variable_gif(alias_pair.first, variable_output_folder);
            auto end_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration<double>(end_time - start_time).count();
            std::cout << std::fixed << std::setprecision(3) << "Execution time: " << duration << " s" << std::endl;
        } 
    }
    return 0;
}

