# easyfpga-sdk-cpp
This is the easyFPGA software development kit for C++. You want to take
advantage of configurable hardware and avoid the effort of becoming
acquainted with hardware description languages? Then easyFPGA
is exactly what you are looking for! It will highly simplify your way to
success the design of FPGA-based systems.

The "easyFPGA toolkit" is an open source project consisting of:
- an easyFPGA development board and
- an C++-SDK (called framework).

The easyFPGA framework allows you accessing different hardware components
within your C++ applications and is currenty meant to be used in
conjunction with the easyFPGA board. The SDK is licensed as open source
and can be arbitrarily modified for an non-commercial use, why it can be
freely customized to run with self-selected hardware. Only if you intend
to use it commercially, please request for an alternative license model
with the contact form can be found on http://easyfpga.de.

You can find detailed information on how to install and use the SDK in
the documentation.

## Cloning
In order to get the SDK please clone this repository. Since there is the
submodule soc already included in this repository containing all
neccessary hardware description files for generating FPGA binaries, you
have to clone it recursively:

```
$ git clone --recursive https://github.com/os-cillation/easyfpga-sdk-cpp.git
```

## Documentation
The documentation is included into the source files and contains all
information about the SDK you need. You are able to generate a html
documentation for example using doxygen. For that, please make sure you
have installed the package doxygen on your system.

```
$ cd easyfpga-sdk-cpp/sdk
$ make doc
```

Then use your favorite webbrowser to read the generated documentation:

```
$ WEBBROSWER sdk/doc/html/index.html
```

## Changelog

#### v1.0 (2015-09-14)
* Initial check-in
