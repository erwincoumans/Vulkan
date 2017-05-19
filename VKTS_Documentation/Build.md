Building for Android, Linux and Windows:
----------------------------------------

General requirements:
---------------------
  
- [LunarG Vulkan SDK](http://vulkan.lunarg.com) version has to be 1.0.49.0.
- [Python](https://www.python.org/) version has to be 3.5 or higher.  


Following requirements do depend on the platform:

![Android](images/android_logo.png) Android:
---

- Building: ndk-build  
    - [Android NDK](http://developer.android.com/ndk/index.html) with Vulkan support required.  
    - [Apache Ant](https://ant.apache.org/) required.
    - [ImageMagick](https://www.imagemagick.org/) for automatic image resizing required.
      
      

![Linux](images/linux_logo.png) Linux:
---

- Building: Eclipse IDE for C/C++ Developers (recommended)  
    - [Eclipse IDE for C/C++ Developers](https://www.eclipse.org/downloads/) project files included.   
- Building: make  
    - Minimum [CMake](https://cmake.org/) version has to be 3.2.0.  
- Needed libraries:
    - X11: libx11-xcb-dev libxrandr-dev
    - Wayland: libwayland-dev


![Windows](images/windows_logo.png) Windows:
---

- Building: Visual C++ (recommended)  
    - Minimum [CMake](https://cmake.org/) version has to be 3.2.0.  
    - [Visual Studio 2015](https://www.visualstudio.com/en-us/downloads/visual-studio-2015-downloads-vs.aspx) or later is required.  
- Building: Eclipse IDE for C/C++ Developers  
    - [Eclipse IDE for C/C++ Developers](https://www.eclipse.org/downloads/) project files included.  
- Building: make  
    - Minimum CMake version has to be 3.2.0.  
    - [MinGW-w64 - for 32 and 64 bit Windows](https://sourceforge.net/projects/mingw-w64/) required.  


---

Using Python scripts:
---------------------

The Python scripts in the root folder do build all VKTS packages and examples for the target operating system and compiler.

Execute for Windows:
- Default build: `create_build_all_MSVC.py`  
- 64 bit build: `create_build_all_MSVC.py 64bit`  

Execute for Linux:
- XLib/Xcb build : `python3 create_build_all_make.py`  
- Wayland build: `python3 create_build_all_make.py VKTS_WAYLAND_VISUAL`  
- Display build: `python3 create_build_all_make.py VKTS_DISPLAY_VISUAL`  

Execute for Android under Windows:
- Default build: `create_build_all_android.py`  
- Validate build: `create_build_all_android.py validate`  
  
  
Also, a global CMake file can be generated for all VKTS packages and Vulkan examples:

Execute for Windows:
- `create_master_CMakeLists.py`

Execute for Linux:
- `python3 create_master_CMakeLists.py`


---

Using Eclipse:
--------------

1. Install [Eclipse IDE for C/C++ Developers](https://www.eclipse.org/downloads/) for Linux or Windows.
2. Install GCC for Linux or [MinGW-w64 - for 32 and 64 bit Windows](https://sourceforge.net/projects/mingw-w64/) for Windows.
3. Import all folders as an existing project in Eclipse. 
4. Set the build configuration of VKTS and the VKTS_Example* in Eclipse to your operating system.
5. __Build VKTS first__.
6. Build VKTS_Example*.
7. The executables are located in the [VKTS_Binaries](VKTS_Binaries/README.md) folder.
8. __The VKTS_Binaries folder has to be the working directory, otherwise shaders and assets are not found__.


---

Using Visual C++ 2015:
----------------------

1. Install [Visual Studio 2015 Community Edition](https://www.visualstudio.com/de/downloads/) for Windows.
2. Run CMake and configure for "Visual Studio 14 2015 Win64".
3. Set the source code directory to VKTS.
4. Set the build directory to VKTS/MSVC (Note: This build directory is expected by the examples).
5. Open the Visual C++ solution file and __build VKTS first__.
6. Do step 3 to 5 with VKTS_Example*.
7. The executables are located in the [VKTS_Binaries](VKTS_Binaries/README.md) folder.
8. __The VKTS_Binaries folder has to be the working directory, otherwise shaders and assets are not found__.


---

Using make:
-----------

1. Install a GNU GCC toolchain.
2. Run CMake and configure "MSYS Makefiles" for Windows or "Unix Makefiles" for Linux.
   Add `-DVKTS_WSI=VKTS_WAYLAND_VISUAL` to create for Wayland window system.
   Add `-DVKTS_WSI=VKTS_DISPLAY_VISUAL` to create for Display window system.
3. Set the source code directory to VKTS.
4. Set the build directory to VKTS (Note: This build directory is expected by the examples). 
5. __Run make and build VKTS first__.
6. Do step 3 to 5 with VKTS_Example*.
7. The executables are located in the [VKTS_Binaries](VKTS_Binaries/README.md) folder.


---

Using LunarG SDK under Ubuntu:
------------------------------

After installing the LunarG SDK, the headers and libraries are not visible by the compiler.  
Append the following lines to your `.profile` file in your home directory: 

`#`  
`# Vulkan development setup.`  
`#`  
  
`export VULKAN_SDK=~/Development/VulkanSDK/1.0.49.0/x86_64`  
  
`export PATH=$PATH:$VULKAN_SDK/bin`  
`export LD_LIBRARY_PATH=$VULKAN_SDK/lib`  
`export VK_LAYER_PATH=$VULKAN_SDK/etc/explicit_layer.d`  
  
`C_INCLUDE_PATH=$VULKAN_SDK/include`   
`export C_INCLUDE_PATH`  
`CPLUS_INCLUDE_PATH=$VULKAN_SDK/include`   
`export CPLUS_INCLUDE_PATH`  
`LIBRARY_PATH=$VULKAN_SDK/lib`  
`export LIBRARY_PATH`  

Finally, change the path of `VULKAN_SDK` depending on your installation directory and SDK version.


---

Installing Intel Linux Vulkan Driver under Ubuntu:
--------------------------------------------------

At the point of writing, the latest Intel Vulkan driver is not included in Ubuntu.    
Execute the following command to add the repository with the Intel Vulkan driver:  
  
`sudo apt-add-repository ppa:paulo-miguel-dias/mesa`  
`sudo apt update`  
`sudo apt dist-upgrade`


---

Using Android NDK:
------------------

The needed shaders, textures and scene files are automatically copied into the assets folder by
executing `build_project.py`. After the build, the assets are packed in the apk file. 

The following command builds the VKTS library:

1. Open console in `Android/jni` project folder and execute following command:  
    - `ndk-build`


The following command creates the Android project:

1. Open console in `Android` project folder and execute following command:  
    - `create_project.py`

The following command copies the needed assets and builds the Android project:

2. Open console in `Android` project folder and execute following command:  
    - `build_project.py`

The following command installs the Android project:

3. Open console in `Android` project folder and execute following command:  
    - `install_project.py`

The following commands allows to display the log output:

4. Open another console execute following command in the `VKTS_Binaries` folder:  
    - `android_log.py` 

The following command uninstalls the Android project:

5. Open console in `Android` project folder and execute following command:  
    - `uninstall_project.py`
