#include "./ShaderCPoll.h"
#include "./Utils.h"
#include "./ColoredConsole.h"

namespace SCP {

ShaderCPoll::ShaderCPoll() {
  /*
    what they smokin?
  -fshader-stage=<stage>
                    Treat subsequent input files as having stage <stage>.
                    Valid stages are vertex, vert, fragment, frag, tesscontrol,
                    tesc, tesseval, tese, geometry, geom, compute, and comp.
  */
  _extensions.push_back(std::make_shared<ShaderTypeExt>(ShaderType::Geometry, ".gs", "geometry", "-exg"));
  _extensions.push_back(std::make_shared<ShaderTypeExt>(ShaderType::Vertex, ".vs", "vertex", "-exv"));
  _extensions.push_back(std::make_shared<ShaderTypeExt>(ShaderType::Fragment, ".fs", "fragment", "-exf"));
  _extensions.push_back(std::make_shared<ShaderTypeExt>(ShaderType::TessControl, ".tcs", "tesscontrol", "-extc"));
  _extensions.push_back(std::make_shared<ShaderTypeExt>(ShaderType::TessEval, ".tes", "tesseval", "-exte"));
  _extensions.push_back(std::make_shared<ShaderTypeExt>(ShaderType::Compute, ".cs", "compute", "-exc"));
}
ShaderCPoll::~ShaderCPoll() {
  //--target-env=<environment>
  //          vulkan1.0       # The default
  //        vulkan1.1
  //        vulkan1.2
  //        vulkan          # Same as vulkan1.0
  //        opengl4.5
  //        opengl          # Same as opengl4.5
}
string_t ShaderCPoll::parseArg(const string_t& arg) {
  string_t ret = "";
  auto index = arg.find('=');
  if (index == string_t::npos) {
    std::string st = "Invalid argument: " + arg;
    throw std::invalid_argument(st.c_str());
  }
  ret = arg.substr(index + 1, arg.size() - (index + 1));

  return ret;
}

string_t ShaderCPoll::nextArg(std::vector<std::string>& args, size_t& iArg) {
  if (iArg == args.size() - 1) {
    string_t st = "Missing argument after " + args[iArg];
    throw std::invalid_argument(st.c_str());
  }
  iArg++;
  string_t ret = Utils::stripQuotes(args[iArg]);
  return ret;
}
void ShaderCPoll::init(std::vector<std::string>& args) {
  if (args.size() < 3) {
    throw std::invalid_argument("Must have at least 3 arguments: shaderc_path, input_dir, output_dir. Do not use '=', just the path.");
  }
  _shaderc_path = Utils::stripQuotes(args[0]);
  _input_dir = Utils::stripQuotes(args[1]);
  _output_dir = Utils::stripQuotes(args[2]);

  bool output_mode_set = false;
  bool recursive_mode_set = false;

  for (size_t iArg = 3; iArg < args.size(); ++iArg) {
    string_t& arg = args[iArg];
    if (Utils::beginsWith(arg, "-ms")) {
      string_t val = parseArg(arg);
      _sleepMS = std::stoi(val);

      if (_sleepMS < 100) {
        Utils::logWarn("Sleep value '" + std::to_string(_sleepMS) + "' must be >= 100. Setting to 100.");
        _sleepMS = 100;
      }
    }
    else if (Utils::equals(arg, "-colors")) {
      _colors = true;
    }
    else if (Utils::equals(arg, "-showcmd")) {
      _showcmd = true;
    }
    else if (Utils::equals(arg, "-woff")) {
      if (output_mode_set) {
        Utils::logWarn("Multiple output modes have been specified. Only the last will be used.");
      }
      _outputMode = OutputMode::Off;
      output_mode_set = true;
    }
    else if (Utils::equals(arg, "-errors")) {
      if (output_mode_set) {
        Utils::logWarn("Multiple output modes have been specified. Only the last will be used.");
      }
      _outputMode = OutputMode::Errors;
      output_mode_set = true;
    }
    else if (Utils::equals(arg, "-warnings")) {
      if (output_mode_set) {
        Utils::logWarn("Multiple output modes have been specified. Only the last will be used.");
      }
      _outputMode = OutputMode::ErrorsAndWarnings;
      output_mode_set = true;
    }
    else if (Utils::equals(arg, "-cpd=on")) {
      _copyFolderStructure = true;
    }
    else if (Utils::equals(arg, "-cpd=off")) {
      _copyFolderStructure = false;
    }
    else if (Utils::equals(arg, "-force")) {
      _force = true;
    }
    else if (Utils::equals(arg, "-nobanner")) {
      _nobanner = true;
    }
    else if (Utils::equals(arg, "-recursive")) {
      if (recursive_mode_set) {
        Utils::logWarn("Multiple recursive modes have been specified. Only the last will be used.");
      }
      _recursive = true;
      recursive_mode_set = true;
    }
    else if (Utils::equals(arg, "-mt")) {
      _threaded = true;
    }
    else if (parseExtensions(arg)) {
      //Parsed.
    }
    else if (Utils::equals(arg, "--")) {
      _additionalShaderCArgs = nextArg(args, iArg);
    }
    else {
      Utils::logError("Unrecognized flag: " + arg);
    }
  }
}
bool ShaderCPoll::parseExtensions(const string_t& arg) {
  string_t ex = "";
  for (auto ext : _extensions) {
    if (parseExt(arg, ext->_flag, ex)) {
      ext->_ext = ex;
      return true;
    }
  }

  if (parseExt(arg, "-outext", ex)) {
    _outputExt = ex;
    return true;
  }

  return false;
}
bool ShaderCPoll::parseExt(const string_t& arg, const string_t& flag, string_t& outExt) {
  if (Utils::beginsWith(arg, flag)) {
    outExt = parseArg(arg);
    if (!Utils::beginsWith(outExt, ".")) {
      Utils::logWarn("Extension '" + outExt + "' did not begin with '.', adding '.'");
      outExt = Stz "." + outExt;
    }
    return true;
  }
  return false;
}
void ShaderCPoll::run() {
  if (_nobanner == false) {
    ColoredConsole::print("shaderc_poll by MetalMario971. Press Ctrl+C to quit.\n", _colors ? ColoredConsole::Color::FG_CYAN : ColoredConsole::Color::FG_GRAY);
  }

  bool exit = false;
  while (!exit) {
    if (!Utils::directoryExists(_input_dir)) {
      Utils::throwException("Input directory '" + _input_dir + "' does not exist.");
    }
    if (!Utils::directoryExists(_output_dir)) {
      Utils::throwException("Output directory '" + _output_dir + "' does not exist.");
    }

    //Gather files.
    std::unordered_map<string_t, std::shared_ptr<ShaderFile>> new_files;
    getFiles(_recursive, _input_dir, new_files);
    _files = new_files;

    //Process files.
    for (auto pair : _files) {
      processFile(pair.second);
    }

    //Clear Force flag.
    _force = false;

    std::this_thread::sleep_for(std::chrono::milliseconds(_sleepMS));
  }
}
void ShaderCPoll::getFiles(bool recursive, const string_t& root, std::unordered_map<string_t, std::shared_ptr<ShaderFile>>& new_files) {
  //Gathers all ShaderFile files into a new std::map
  std::vector<string_t> out_filepaths;
  Utils::getAllFiles(root, out_filepaths);
  for (auto filepath : out_filepaths) {
    auto it = _files.find(filepath);
    if (it == _files.end()) {
      auto type = getFileType(filepath);
      if (type != nullptr) {
        auto newf = std::make_shared<ShaderFile>();
        newf->_ext = type;
        newf->_inputPath = filepath;
        new_files.insert(std::make_pair(filepath, newf));
      }
      else {
        //File is not a shader.
      }
    }
    else {
      new_files.insert(std::make_pair(filepath, it->second));
    }
  }

  if (recursive) {
    std::vector<string_t> dirs;
    Utils::getAllDirs(root, dirs);
    for (auto dir : dirs) {
      getFiles(recursive, dir, new_files);
    }
  }
}
std::shared_ptr<ShaderTypeExt> ShaderCPoll::getFileType(const string_t& file) {
  auto ext = Utils::getExtensionPartOfFileName(file);
  for (auto it : _extensions) {
    if (Utils::equals(ext, it->_ext)) {
      return it;
    }
  }
  return nullptr;
}
void ShaderCPoll::processFile(std::shared_ptr<ShaderFile> file) {
  //./shaderc/bin/glslc -fshader-stage=vertex ./test.vs -o ./test_vs.spv
  AThrow(file->_ext != nullptr);

  if (!Utils::fileExists(file->_inputPath)) {
    Utils::logError("Input file '" + file->_inputPath + "' was not found.");
    return;
  }

  //Output Directory
  string_t out_dir = "";
  string_t out_file = "";
  if (_copyFolderStructure) {
    //Swap the input root folder to the output
    auto inpath_path = Utils::getDirectoryNameFromPath(file->_inputPath);
    AThrow((int)inpath_path.length() >= (int)_input_dir.length());
    auto inpath_branch = inpath_path.substr(_input_dir.length(), inpath_path.length() - _input_dir.length());
    out_dir = Utils::combinePath(_output_dir, inpath_branch);
  }
  else {
    out_dir = _output_dir;
  }

  //Output Name
  string_t in_filename = Utils::getFileNameFromPath(file->_inputPath);
  out_file = Utils::combinePath(out_dir, in_filename);
  out_file += _outputExt;

  //Don't compile if we don't need to.
  time_t mod_in = Utils::getLastModifyTime(file->_inputPath);
  if (_force == false) {
    if (Utils::fileExists(out_file)) {
      if (file->_compileTime == 0) {
        time_t mod_out = Utils::getLastModifyTime(out_file);
        //We compiled it before but not in this run. Compare the date on the input and output files to see if it's been updated.
        if (mod_out >= mod_in) {
          return;
        }
      }
      else {
        //Either A) we compiled before, but this time there were errors, or not and or B) the file was updated or C) the file wasn't updated.
        if (file->_compileTime >= mod_in) {
          return;
        }
      }
    }
    else {
      //We haven't compiled it, or, we compiled it and there were errors.
      if (file->_compileTime == 0) {
        //File wasn't compiled, no errors.
      }
      else {
        //We compiled it, but there may have been errors. Don't keep spitting.
        time_t mod_in = Utils::getLastModifyTime(file->_inputPath);
        if (file->_compileTime >= mod_in) {
          //We compiled it, there were errors and the user hasn't updated the file yet. Don't keep spitting.
          return;
        }
      }
    }
  }

  //Create output path
  try {
    Utils::createDirectoryRecursive(out_dir);
  }
  catch (std::exception& e) {
    Utils::logError("Exception thrown creating path recursively: " + std::string(e.what()));
    return;
  }

  //Launch shaderc
  string_t command = _shaderc_path + " " + _additionalShaderCArgs +
                     " -fshader-stage=" + file->_ext->_shadercVar +
                     " " + file->_inputPath + " -o " + out_file;

  ColoredConsole::print(file->_inputPath + "\n", _colors ? ColoredConsole::Color::FG_MAGENTA : ColoredConsole::Color::FG_GRAY);

  if (_showcmd) {
    ColoredConsole::print(command + "\n", _colors ? ColoredConsole::Color::FG_CYAN : ColoredConsole::Color::FG_GRAY);
  }
  file->_compileOutput = Utils::executeReadOutput(command);
  file->_outputPath = out_file;
  file->_compileTime = mod_in;  //The basis for time returned from time(null) appears to be different from stat() so we just use this here.

  //Print output.
  std::vector<string_t> lines;
  Utils::split(file->_compileOutput, { '\n' }, lines);
  for (auto& line : lines) {
    ColoredConsole::Color ccc;
    if (line.find("error:") != std::string::npos) {
      if (_outputMode == OutputMode::Errors || _outputMode == OutputMode::ErrorsAndWarnings) {
        ccc = ColoredConsole::Color::FG_RED;
      }
    }
    else if (line.find("warning:") != std::string::npos) {
      if (_outputMode == OutputMode::ErrorsAndWarnings) {
        ccc = ColoredConsole::Color::FG_YELLOW;
      }
    }
    else {
      ccc = ColoredConsole::Color::FG_GRAY;
    }
    ColoredConsole::print(line + "\n", _colors ? ccc : ColoredConsole::Color::FG_GRAY);
  }

  //If successful, print the output file (green)
  if (Utils::fileExists(out_file)) {
    auto mod_out = Utils::getLastModifyTime(out_file);
    if (mod_out >= mod_in) {
      ColoredConsole::print(Stz " -> " + out_file + "\n", _colors ? ColoredConsole::Color::FG_GREEN : ColoredConsole::Color::FG_GRAY);
    }
  }
}

}  // namespace SCP