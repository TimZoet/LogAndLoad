# Build Instructions

## Dependencies

The following Conan packages are always required:

1. [pyreq](https://github.com/TimZoet/pyreq)
2. [common](https://github.com/TimZoet/common)
3. [dot](https://github.com/TimZoet/dot)

Depending on enabled options, you may also need:

## Cloning

This project uses git submodules. Cloning therefore requires an additional flag:

```cmd
git clone git@github.com:TimZoet/LogAndLoad.git source --recurse-submodules
```

## Exporting to Conan

To export the `logandload` package to your local Conan cache:

```cmd
cd source
conan export . user/channel
```

## Building and Installing Locally

Generating project files can be done using e.g.:

```cmd
mkdir build && cd build
cmake -G "Visual Studio 16 2019" -A x64 -DCMAKE_INSTALL_PREFIX=..\install ..\source
```

Then build and install using your preferred method.
