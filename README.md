# Capture Camera Image
This is a project to trigger images and reading out image data from an industrial camera (GigeVision). Also to create a PGM file with the image acquired.

There are two posibilities to build, using Docker or native Linux machine.

## Using Docker
### Project Build

 * Use ```./scripts/build-docker.sh``` to create a Docker image with all dependencies.
 * Use ```./scripts/build-in-docker.sh``` to build the source code on Docker image. Binary file is create on _./build/check-defects_.

### Project Run

 * Use ```./scripts/run-in-docker.sh``` to run the application on Docker image.

### Project Clean

 * Use ```./scripts/clean-docker.sh``` to clean a build files on Docker image.
 * Use ```./scripts/delete-docker.sh``` to delete Docker image.

### Extra

 * Use ```./scripts/run-docker.sh``` to run Docker image and run commands manually. (Debug purpose)

### Usage example

```$ ./scripts/build-docker.sh && ./scripts/build-in-docker.sh && ./scripts/run-in-docker.sh```

Binary file is create on _./build/check-defects_.

PMG file is create on _./build/output.pgm_.

## Using native Linux
There is a script _generate.sh_ to help you to compile and run the application.

_NOTE: This application was testes using Linux Ubuntu 16.04._

### Project Build

 * Use ```./generate.sh -p``` to prepare the enviroment. __(requires sudo)__
 * Use ```./generate.sh -C``` to create build files.
 * Use ```./generate.sh -b``` to to build the project. Binary file is create on _./build/check-defects_.

### Project Run

 * Use ```./generate.sh -r``` to run the application.

### Project Clean

 * Use ```./generate.sh -c``` to clean the build files.
 * Use ```./generate.sh -d``` to clean all temporary files.

### Extra
 * Use ```./generate.sh -k``` to run a static code analysis.

### Usage example

```$ ./generate.sh -pCbr```

Binary file is create on _./build/check-defects_.

PMG file is create on _./build/output.pgm_.
