#include "create_images.h"
#include "ThreadPool.h"

bool parse_arguments(int argc, char *argv[], std::string& input_file, std::string& variable_or_threads, std::string& output_folder) {
    for (int i = 1; i < argc; i += 2) {
        if (i + 1 >= argc) {
            std::cerr << "Error: Missing value for argument " << argv[i] << std::endl;
            return false;
        }
        if (strcmp(argv[i], "--input") == 0) {
            input_file = argv[i + 1];
        } else if (strcmp(argv[i], "--var_or_threads") == 0) {
            variable_or_threads = argv[i + 1];
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

    if (variable_or_threads.empty()) {
        std::cerr << "Error: Missing required argument --var_or_threads" << std::endl;
        return false;
    }

    return true;
}

void process_single_variable(const std::string& input_file, const std::string& variable, const std::string& alias, const std::string& output_folder) {
    std::filesystem::create_directories(output_folder);
    visualize_variable(input_file, variable, alias, output_folder);
    const std::string& input_pattern = output_folder + "/" + "*.jpg";
    const std::string& output_filename = output_folder + "/" + alias + ".gif";
    create_gif(input_pattern, output_filename, 10);
}

void process_variables_threaded(int num_threads, const std::map<std::string, std::string>& variable_aliases, const std::string& input_file, const std::string& output_folder) {
    ThreadPool thread_pool(num_threads);
    for (const auto& alias_var_pair : variable_aliases) {
        const std::string& alias = alias_var_pair.first;
        const std::string& variable = alias_var_pair.second;
        const std::string& variable_output_folder = output_folder + "/" + alias;
        thread_pool.enqueue([input_file, variable, alias, variable_output_folder]() {
            process_single_variable(input_file, variable, alias, variable_output_folder);
        });
    }
}

int main(int argc, char *argv[]) {
    std::string input_file;
    std::string variable_or_threads;
    std::string output_folder = "output";

    if (!parse_arguments(argc, argv, input_file, variable_or_threads, output_folder)) {
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

    try {
        int num_threads = std::stoi(variable_or_threads);
        if (num_threads > 0) {
            process_variables_threaded(num_threads, variable_aliases, input_file, output_folder);
        } else {
            std::cerr << "Error: Invalid number of threads." << std::endl;
            return 1;
        }
    } catch (const std::invalid_argument& e) {
        auto it = variable_aliases.find(variable_or_threads);
        if (it != variable_aliases.end()) {
            const std::string& variable_output_folder = output_folder + "/" + it->first;
            process_single_variable(input_file, it->second, it->first, variable_output_folder);
        } else {
            std::cerr << "Error: Invalid variable name or number of threads provided." << std::endl;
            return 1;
        }
    }

    return 0;
}

