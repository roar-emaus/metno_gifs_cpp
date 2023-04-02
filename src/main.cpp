#include "create_images.h"
#include <map>
#include <filesystem>

bool parse_arguments(int argc, char *argv[], std::string& input_file, std::string& variable, std::string& output_folder) {
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
        } else {
            std::cerr << "Error: Unknown argument " << argv[i] << std::endl;
            return false;
        }
    }

    if (input_file.empty()) {
        std::cerr << "Error: Missing required argument --input" << std::endl;
        return false;
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
    std::string output_folder = "output";

    if (!parse_arguments(argc, argv, input_file, variable, output_folder)) {
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
        {"relative_humidity", "relative_humidity_2m"}
    };

    if (!variable.empty()) {
        auto it = variable_aliases.find(variable);
        if (it != variable_aliases.end()) {
            std::filesystem::path variable_output_folder = std::filesystem::path(output_folder) / it->first;
            create_variable_images(input_file, it->second, it->first, variable_output_folder);
            create_variable_gif(it->first, variable_output_folder);
        } else {
            std::cerr << "Error: Invalid variable name provided." << std::endl;
            return 1;
        }
    } else {
        for (const auto& alias_pair : variable_aliases) {
            const std::string& variable_name = alias_pair.first;
            const std::string& variable_alias = alias_pair.second;
            std::filesystem::path variable_output_folder = std::filesystem::path(output_folder) / variable_name;
            create_variable_images(input_file, variable_alias, variable_name, variable_output_folder);
            create_variable_gif(alias_pair.first, variable_output_folder);
        } 
    }
    return 0;
}

