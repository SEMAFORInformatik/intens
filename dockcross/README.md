# Dockcross

This folder contains the scripts and files required to cross-build INTENS for Windows.

To build, you need to first build the cross compile image by doing `docker buildx bake dockcross`.

> [!WARNING]
> Building the dockcross image takes a very long time and results in an image multiple gigabytes large.
> It is recommended to do a native Windows build if the environment is available.

After you have said image, modify the tag in the
DOCKCROSS_IMAGE variable of the `build-windows-release.sh` script and run it.
