# shaderc_poll
Compile shader files and poll for changes. Continuous integration shader compiler.

## Usage
```
shaderc_poll shaderc_path input_dir output_dir [-ms=500] [-mt] [-nobanner] [-errors | -warnings | -woff] [-colors] [-recursive] [-showcmd] [-force] [-cpd=on|off] [-outext=***] [-ex**=**] [-- [shaderc_arg1] [shaderc_arg2]]
```

### Arguments
shaderc_path - path to shaderc
input_dir - directory of input files
output_dir - directory of output files

### Optional arguments
-ms - Milliseconds between polls. (default 250ms).
-mt - multithreaded compilation (default false).
-nobanner - hide the banner.
-errors - Emit errors only.
-warnings Emit errors & warnings (default).
-woff - Do not emit errors and warnings.
-colors - Colored output (default off).
-recursive - Search ```input_dir``` recursively. Dump SPIRV in ```output_dir.```
-showcmd - Print the shaderc compile command (default false).
-force - Force recompilation of all files upon start. (default false).
-cpd=[on|off]- Match folder layout of ```input_dir```. Subfolders in ```output_dir``` will match the subfolders in ```input_dir```. ```shader_poll``` will create new folders as necessary. If disabled files may be overwritten. (default on).
-outext - Output file extension. (default .spv).
-ex**=** _Custom file extensions (syntax: -exg=.gs -exv=.vs)_
* -exg - geometry shader extension (default .gs).
* -exv - vertex shader extension (default .vs).
* -exf - fragment shader extension (default .fs).
* -extc - tessellation control shader extension (default .tcs).
* -exte - tessellation eval shader extension (default .tes).
* -exc - compute eval shader extension (default .cs).
-- - additional commands sent to shaderc... e.g. -- --target-env=vulkan1.2, --target-env=opengl4.5, etc.

## Example 
./bin/shaderc_poll-1.0.1_x86d '~/git/shader_poll/shaderc/bin/glslc' './test/test_input' './test/test_output' -errors -warnings -ms=250 -recursive -cpd=on -colors -mt -force -- --target-env=vulkan1.2

## Note
Included ```sp.bat``` and ```sp.sh``` files that you can modify to easily run shader_poll with one click.

## Platforms
* Windows 10
* Linux

## Installation.
* Install shaderc somewhere.
  * Compile from git, or just install the binaries.
* Install cmake if you don't have it.
  * Linux: apt install cmake.
  * Windows: download from cmake website.
* Compile shader_poll with ```cmake --build```

## Dependencies
* (Shaderc (by Google))[https://github.com/google/shaderc]
* CMake



