OpenUSM
======

OpenUSM is an open-source game engine that supports playing Ultimate Spider-Man by Treyarch. You need to own the game for OpenUSM to play Ultimate Spider-Man.

Current Status
--------------

The engine is not complete and is at the prototyping stage.

Requiments:
* Linux or Ubuntu on Windows 10
* MinGW-w64 compiler
* zlib1g
* assimp



How to build and install:
------------------------
* Copy `shaders` directory from project to the game's folder.
* Rename `binkw32.dll` in the game's folder to `binkw32_.dll`
* Download the repository to a directory convenient for you using the command:
`git clone https://gitlab.com/MrMartinIden/openusm.git` 
* `cd openusm`
* `cmake -B build` (a `build` directory and makefile will be created)
* `cd build`
* `make -jN`, where N - how many threads you want to allot for compiling.
* Copy the compiled `binkw32.dll` from `build` directory to the game's folder.
* use this command for install zlib1g: `sudo apt-get -y install libz-mingw-w64-dev`
* clone assimp on openusm folder with git clone https://github.com/assimp/assimp.git
* install DUSE_UPX: `cmake -DUSE_UPX=ON`


License
------------

The code should only be used for educational, documentation and modding purposes.
I do not encourage piracy or commercial use.
Please keep derivate work open source and give proper credit.
