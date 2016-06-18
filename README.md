# Polygamma

An open source audio editing software.

## Building

###### Dependencies
Polygamma dependes on `Qt5.6.0, avcodec, avformat, avutil, swscale, boost`.

###### Compilation
It is best to do an out of source build at `build/` directory. Create a
subdirectory `build` and run
```
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make
```
If `cmake` is unable to find `Qt5Widgets`, try feeding it the following option:
```
-DQt5Widgets_DIR = <directory containing Qt5WidgetsConfig.cmake>
```
## Developing

The script `update.py` is used to introduce new source files or new QObject
headers to `CMakeLists.txt`.
