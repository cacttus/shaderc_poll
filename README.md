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

### Arguments

| Command | Desciption |
| :---: | :---: | 
| shaderc_path | path to shaderc | 
| input_dir | input file root directory | 
| output_dir | output file root directory | 

### Optional arguments
| Command | Desciption |
| :---: | :---: | 
| -ms | Milliseconds between polls. (default 250ms). | 
| -mt | multithreaded compilation (default false). | 
| -nobanner | hide the banner. | 
| -errors | Emit errors only. | 
| -warnings |  Emit errors & warnings (default). | 
| -woff |  Do not emit errors and warnings. | 
| -colors |  Colored output (default off). | 
| -recursive |  Search ```input_dir``` recursively. Dump SPIRV in ```output_dir.``` | 
| -showcmd |  Print the shaderc compile command (default false). | 
| -force |  Force recompilation of all files upon start. (default false). | 
| -cpd=on/off |  Match folder layout of ```input_dir```. Subfolders in ```output_dir``` will match the subfolders in ```input_dir```. ```shader_poll``` will create new folders as necessary. If disabled files may be overwritten. (default on). | 
| --ex**=** |  _Custom file extensions (syntax: -exg=.gs -exv=.vs)_ (see below) | 
| -- | Additional commands sent to shaderc... e.g. -- --target-env=vulkan1.2, --target-env=opengl4.5, etc. Must come last. | 

### Custom Shader Type Extensions
| Command | Desciption |
| :---: | :---: |
| -exg | Geometry shader extension (default .gs). |
| -exv | Vertex shader extension (default .vs). |
| -exf | Fragment shader extension (default .fs). |
| -extc | Tessellation control shader extension (default .tcs). |
| -exte | Tessellation eval shader extension (default .tes). |
| -exc | Compute eval shader extension (default .cs). |

## Example 
./bin/shaderc_poll-1.0.1_x86d '~/git/shader_poll/shaderc/bin/glslc' './test/test_input' './test/test_output' -errors -warnings -ms=250 -recursive -cpd=on -colors -mt -force -- --target-env=vulkan1.2

## Installation.
* Install shaderc
* Install cmake if you don't have it.
* Compile shader_poll with ```cmake --build```

## Dependencies
* [Shaderc (Google)](https://github.com/google/shaderc)
* CMake

## Future Additions
* Allow users to specify multiple input folders.

## Screenshot
![Screenshot](https://github.com/metalmario971/shaderc_poll/blob/master/screenshot.png)

