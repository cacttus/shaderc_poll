# shaderc_poll
Compile shader files and poll for changes. Continuous integration shader compiler.

## Description
```shaderc_poll``` streamlines the process of writing and compiling shaders. It searches a given directory for shader files, compiles them and places successfully compiled files into the specified output location. It provides syntax coloring for errors and warnings and the ability to match the output directory file tree. The program identifies shader types by their extension (.vs=vertex,.fs=fragment) this is also customizable.

## Usage
```
shaderc_poll shaderc_path input_dir output_dir [-ms=500] [-mt] [-nobanner] 
[-errors | -warnings | -woff] [-colors] [-recursive] [-showcmd] [-force] 
[-cpd=on|off] [-outext=***] [-ex**=**] [-- [shaderc_arg1] [shaderc_arg2]]
```

#### Arguments

| Command | Desciption |
| :---: | :---: | 
| shaderc_path | path to shaderc | 
| input_dir | input file root directory | 
| output_dir | output file root directory | 

#### Optional arguments (No '=' spaces)
| Command | Desciption |
| :---: | :---: | 
| -ms | Milliseconds between polls. (default 250ms). | 
| -threaded=on/off | multithreade66d compilation (default on). | 
| -batchsize=* | Batch size for multithreaded compilation (default 50). | 
| -force |  Force compilation of all files at start. (default false). | 
| -banner=on/off | Show/hide the banner. (default off) | 
| -errors | Emit errors only. | 
| -warnings |  Emit errors & warnings (default). | 
| -woff |  Do not emit errors and warnings. | 
| -colors=on/off |  Colored output (default on). | 
| -stats=on/off |  Print stats (default off). | 
| -recursive |  Search ```input_dir``` recursively. Dump output in ```output_dir.``` | 
| -showcmd=on/off |  Print the shaderc compile command (default off). | 
| -copydirs=on/off |  Match folder layout of ```input_dir```. Subfolders in ```output_dir``` will match the subfolders in ```input_dir```. ```shader_poll``` will create new folders as necessary. If disabled files may be overwritten. (default on). | 
| -makeoutputdir |  Automatically create ```output_dir``` if it does not exist. (default off). | 
| --ex**=** |  _Custom file extensions (syntax: -exg=.gs -exv=.vs)_ (see below) | 
| -- | Additional commands sent to shaderc... e.g. -- --target-env=vulkan1.2, --target-env=opengl4.5, etc. Must come last. | 

#### Custom Shader Type Extensions
| Command | Desciption |
| :---: | :---: |
| -exg | Geometry shader extension (default .gs). |
| -exv | Vertex shader extension (default .vs). |
| -exf | Fragment shader extension (default .fs). |
| -extc | Tessellation control shader extension (default .tcs). |
| -exte | Tessellation eval shader extension (default .tes). |
| -exc | Compute eval shader extension (default .cs). |

#### Example 
'../shaderc/glslc/Debug/glslc.exe' './test/test_input' './test/test_output' -warnings -ms=250 -recursive -showcmd=on -copydirs=on -stats=on -colors=on -threaded=on -batchsize=50 -mkod=on -force -- --target-env=vulkan1.2

## Installation
* Install shaderc (/shaderc_poll/shaderc)
* Install cmake
* Linux option #1
	* Compile shader_poll with ```cmake --build```
* Linux option #2 VSCode 
	* Install the following VSCode extensions inside of VSCode
		* CMake tools for VS Code, 
		* CMake for VS Code, 
		* C++ for VS Code 
		* Shader parsers for VSCode (nice to have)
	* Open shaderc_poll folder in VSCode. 
	* Ctrl+Shift+P type 'cmake: build'. F5.
* Windows
	* Run CMake --build
	* Cmake generates a Visual studio project in shaderc_poll/build. 
	* Open the Visual Studio project. 
	* Right click the ```shaderc_poll``` project -> set as startup project
	* Right click the ```shaderc_poll``` project -> properties -> C/C++ -> Additional Options.
	* Delete the $<1 $<0 there - this will cause errors.
	* Do this for both Debug and release configruations
	* Finally, within ```shaderc_poll``` project -> Debugging -> Command line and specify command parameters. If you don't you'll get an error 'invalid input directory'.
	* F5.

## Dependencies
* [Shaderc](https://github.com/google/shaderc)
* [CMake](https://cmake.org/download/)

## Bugs
* Windows
  * Compiling with CMake may cuase error "'$<0:': No such file or directory"
  * Open the .vcxproj and delete the $<0 $<1 under "additional options" in C++
  * If someone knows how to fix this would be nice.
* Automatically selecting the output file (with ${}) in VSCode c_cpp_properties.json. Now you have to manually set it.
* VSCode isn't debugging correctly on windows for some reason
* If the system date is later than the date of the input file then it won't compile. This can happen if you change your date to be later than the date of the file or you switch timezones, etc.

## Future Additions
* Allow users to specify multiple input folders.

## Screenshot
![Screenshot3](https://github.com/metalmario971/shaderc_poll/blob/master/screenshot3.png)
![Screenshot2](https://github.com/metalmario971/shaderc_poll/blob/master/screenshot2.png)
![Screenshot](https://github.com/metalmario971/shaderc_poll/blob/master/screenshot.png)

