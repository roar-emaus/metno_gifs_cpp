# MetNO GIF Generator

This project is a command-line tool for generating GIFs from MetNO weather data in NetCDF format. It supports multiple weather variables such as temperature, radiation, wind direction, wind speed, wind gust, cloud cover, air pressure, and relative humidity.
## Table of Contents

- Requirements
- Installation
- Usage
- License

## Requirements

- Arch Linux (tested with base-20230319.0.135218)
- OpenCV
- HDF5
- NetCDF-CXX
- ImageMagick
- Qt5-base
- fmt
- GLEW
- VTK
- jsoncpp

## Installation

1. Clone the repository:
```
git clone https://github.com/yourusername/metno_gif_generator.git
cd metno_gif_generator
```
2. Build the project using the provided script:
```
./build_two_stage_image.sh
```

## Usage

Run the tool with the following command:
```
metno_gif --input <input_file> --var <variable> --output <output_folder>
```
- <input_file>: The path to the input NetCDF file.
- <variable>: The name of the weather variable to generate the GIF for. Supported variables are:
    * temperature
    * radiation
    * wind_direction
    * wind_speed
    * wind_gust
    * cloud_cover
    * air_pressure
    * relative_humidity
- <output_folder>: The path to the output folder where the generated GIF will be saved. The folder will be created if it does not exist.

If no variable is provided, the tool will generate GIFs for all supported variables.

## Data Source
The data used in this project is provided by the [https://www.met.no/en](Norwegian Meteorological Institute (MET Norway)).
lease ensure that you follow their terms of service and provide proper attribution when using the data ([https://thredds.met.no/thredds/metno.html](MET Norway Numerical Weather Prediction products)).

## License

This project is licensed under the MIT License - see the [https://mit-license.org/](LICENSE) file for details.
