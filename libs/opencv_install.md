```
 git clone https://github.com/opencv/opencv.git
```
```
 cd opencv
 mkdir build
 cd build
```
remember to set c++ version 17
```
 cmake -D CMAKE_BUILD_TYPE=Release -D CMAKE_INSTALL_PREFIX=../local-install ..
```
```
make -j$(nproc)
make install
```

