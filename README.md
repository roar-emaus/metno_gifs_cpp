# MetNO GIF Generator

This project is a command-line tool for generating GIFs from MetNO weather data in NetCDF format.
It supports the [MET post-processed products](https://thredds.met.no/thredds/metno.html) with weather variables
temperature, radiation, wind direction, wind speed, wind gust, cloud cover, air pressure, and relative humidity.

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
git clone https://github.com/yourusername/metno_gifs_cpp.git metno_gif_generator
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
The data used in this project is provided by the [Norwegian Meteorological Institute (MET Norway)](https://www.met.no/en).
Please ensure that you follow their terms of service and provide proper attribution when using the data ([MET Norway Numerical Weather Prediction products](https://thredds.met.no/thredds/metno.html)).

## License

This project is licensed under the MIT License - see the [LICENSE](https://mit-license.org/) file for details.
