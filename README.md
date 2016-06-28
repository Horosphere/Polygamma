# Polygamma

An open source audio editing software.

## Building

###### Dependencies
Polygamma dependes on Qt 5.6, Python 3.5, Boost 1.61, avcodec, avformat,
avutil, and swscale. Boost.Python must be compiled with Python 3 support.

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

###### Conventions and scripts

The script `update.py` is used to introduce new source files or new QObject
headers to `CMakeLists.txt`.

All QObject headers must lie in the directory `src/ui/`.

Polygamma's source codes are formatted according to the following astyle
configuration:
```
--style=allman
--align-pointer=type	
--close-templates
--convert-tabs
--indent-preproc-block
--indent-preproc-define
--indent=tab=2
--pad-header
--unpad-paren
```
Polygamma's include guards are formatted according to the `.formatGen` file in
the root directory.

###### Structure

Polygamma usually run in two threads. The GUI/main thread contains all GUI
interactions managed by Qt, and the Kernel thread which contains the function
`void Kernel::start()`. The Kernel must be halted with `void Kernel::halt()`,
otherwise segmentation faults occur upon application exit.

Kernel communicates with the GUI through two channels. Kernel sends message
to the GUI through `Qt::QueuedConnection`s relayed by lambda functions in
`class MainWindow`. Conversely, GUI sends Kernel commands through
`boost::lockfree:spsc_queue`s.

