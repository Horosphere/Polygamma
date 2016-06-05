# Polygamma

An open source audio editing software.

## Building

###### Dependencies
Polygamma uses Qt 5.6.0 and QMake 3.0. Check that it is the case using `qmake --version`.

###### Compilation
Using QMake,
```
qmake .
make release
```
## Developing

The `update.py` script is used to add source and header files in `src/` to `Polygamma.pro`.
