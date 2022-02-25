# asteroidsyncservice
A synchronization daemon for [AsteroidOS](http://asteroidos.org/) watches. Forked from [Rockpool](https://github.com/abranson/rockpool).

asteroidsyncserviced is a daemon relying on libasteroid to synchronize data from a given platform to a watch. It comes with a QML module named asteroidsyncservice. It is used by starship, a SailfishOS synchronization app and Telescope, an Ubuntu Touch synchronization app.

## Building

### Install the prerequisites

This software uses CMake and Qt, including Bluetooth support.  Installing these varies by system, but for example on many Linux distributions, one can install the latter with this command:

  - `sudo apt-get install qtconnectivity5-dev`

### Get the code
To build asteroidsyncservice, all submodules must be loaded.

  - `git submodule update --init`

### Prepare the build

The software is built using CMake.  Exactly one platform must be specified to build the project.  Currently the supported platforms are:

 - SAILFISHOS_PLATFORM
 - UBUNTU_TOUCH_PLATFORM
 - DESKTOP_PLATFORM

To prepare to build the software, if you are in the top level directory, one can use this command to prepare the build.

```
cmake -DCMAKE_BUILD_TYPE=Release -DDESKTOP_PLATFORM=ON -S . -B build
```

This will set up the software to create a Release version of the software (as contrasted with a Debug version) and put this in a newly created `build` subdirectory.

### Build the software

Again from the top level directory, one can use cmake to build the software.

```
cmake --build build -j
```

This will build the software in the build directory.  The `-j` suffix asks the build to be performed in parallel, which can make the build go much faster on a multicore build machine.  

### Install the software (optional)

Installing the software generally required root access and is presently supported only for the DESKTOP_PLATFORM option.  

```
sudo cmake --build build -j -t install
```

The `-t` option tells CMake the build target that we want.  In this case the target is named `install`. 

### Building developer documentation (optional)

To build the documentation for the project in either HTML or PDF format requires an additional option for the "pPrepare the build" step listed above.  The setting is `WITH_DOC` which is set to `OFF` by default, but can be turned on:

```
cmake -DCMAKE_BUILD_TYPE=Release -DWITH_DOC=ON -DDESKTOP_PLATFORM=ON -S . -B build
```

After the build is prepared, one can build the documentation from the top level directory:

```
cmake --build build -j -t doc
``` 

This builds the software documentation in html format, the software is built in the `build/doc/html` subdirectory.  So to view the documents using Firefox, for example, one can use the command `firefox build/doc/html/index.html` which will load the main page.  Of course any browser can be used, and not just Firefox.

One can also create a pdf file with the same content.  This is done using this command:

```
cmake --build build -j -t pdf
``` 

The PDF file is then created as `build/doc/latex/refman.pdf`.
