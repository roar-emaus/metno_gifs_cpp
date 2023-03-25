- Download from https://github.com/Unidata/netcdf-cxx4/releases
- unzip file

```
  cd /path/to/netcdf-cxx4-X.Y.Z    # Replace with the actual path and version
  mkdir build && cd build
  cmake -DCMAKE_INSTALL_PREFIX:PATH=../local-install -DNetCDF_DIR=../../netcdf-c-X.Y.Z/local-install/lib/cmake ..
  make
  make install
```
