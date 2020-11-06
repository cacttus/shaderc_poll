#include "./ShaderCPoll.h"
#include "./Utils.h"
#include "./ColoredConsole.h"
#include <sstream>

namespace SCP {

#pragma region ShaderCPoll

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

  Utils::_mainThreadId = std::this_thread::get_id();
}
ShaderCPoll::~ShaderCPoll() {
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
    else if (Utils::equals(arg, "-colors=on")) {
      _colors = true;
    }
    else if (Utils::equals(arg, "-colors=off")) {
      _colors = false;
    }
    else if (Utils::equals(arg, "-showcmd=on")) {
      _showcmd = true;
    }
    else if (Utils::equals(arg, "-showcmd=off")) {
      _showcmd = false;
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
    else if (Utils::equals(arg, "-copydirs=on")) {
      _copyFolderStructure = true;
    }
    else if (Utils::equals(arg, "-copydirs=off")) {
      _copyFolderStructure = false;
    }
    else if (Utils::equals(arg, "-stats=on")) {
      _printStats = true;
    }
    else if (Utils::equals(arg, "-stats=off")) {
      _printStats = false;
    }
    else if (Utils::equals(arg, c_mkod_cmd+std::string("=on"))) {
      _makeOutputDir = true;
    }
    else if (Utils::equals(arg, c_mkod_cmd + std::string("=off"))) {
      _makeOutputDir = false;
    }
    else if (Utils::equals(arg, "-force")) {
      _force = true;
    }
    else if (Utils::equals(arg, "-banner=on")) {
      _nobanner = true;
    }
    else if (Utils::equals(arg, "-banner=off")) {
      _nobanner = true;
    }
    else if (Utils::equals(arg, "-recursive")) {
      if (recursive_mode_set) {
        Utils::logWarn("Multiple recursive modes have been specified. Only the last will be used.");
      }
      _recursive = true;
      recursive_mode_set = true;
    }
    else if (Utils::equals(arg, "-threaded=on")) {
      _threaded = true;
    }
    else if (Utils::equals(arg, "-threaded=off")) {
      _threaded = false;
    }
    else if (Utils::beginsWith(arg, "-batchsize")) {
      string_t val = parseArg(arg);
      _taskBatchSize = std::stoi(val);

      if (_taskBatchSize < 0 || _taskBatchSize > 999999) {
        Utils::logWarn("-batchsize '" + std::to_string(_taskBatchSize) + "'. Is invalid, setting to " + std::to_string(_defaultTaskBatchSize) + ".");
        _taskBatchSize = _defaultTaskBatchSize;
      }
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
  if (!Utils::directoryExists(_input_dir)) {
    Utils::throwException("Input directory '" + _input_dir + "' does not exist.");
  }
  if (!Utils::directoryExists(_output_dir)) {
    if (_makeOutputDir) {
      Utils::logInfo(Stz c_mkod_cmd + " was specified, creating '" + _output_dir + "'");
      Utils::createDirectoryRecursive(_output_dir);
    }
    else {
      Utils::throwException("Output directory '" + _output_dir + "' does not exist use " + c_mkod_cmd + " to let shaderc_poll create it.");
    }
  }
  std::unordered_map<string_t, std::shared_ptr<ShaderFile>> curr_files;
  bool exit = false;
  while (!exit) {
    //Gather files.
    std::unordered_map<string_t, std::shared_ptr<ShaderFile>> new_files = curr_files;
    getFiles(_recursive, _input_dir, new_files);

    curr_files = new_files;  //Hold on to the references so they don't delete

    uint32_t comp_success_count = 0;
    uint32_t comp_fail_count = 0;
    uint32_t comp_error_count = 0;
    uint32_t comp_warning_count = 0;
    uint32_t comp_skip_count = 0;
    Stopwatch sw;

    sw.start();
    {
      launchTasks(new_files, comp_success_count, comp_fail_count, comp_error_count, comp_warning_count);
    }
    sw.end();

    if (_printStats) {
      if (comp_success_count > 0 || comp_fail_count > 0 || comp_error_count > 0 || comp_warning_count > 0) {
        string_t stats = "" + std::to_string(comp_success_count) + " succeeded." + "\n" +
                         std::to_string(comp_fail_count) + " failed. " + "\n" +
                         std::to_string(comp_skip_count) + " skipped. " + "\n" +
                         std::to_string(comp_error_count) + " errors. " + "\n" +
                         std::to_string(comp_warning_count) + " warnings." + "\n" +
                         "Total time (" + sw.time() + "), " + "\n";
        ColoredConsole::print(stats, ColoredConsole::Color::FG_WHITE);
      }
    }

    //Clear Force flag.
    _force = false;

    std::this_thread::sleep_for(std::chrono::milliseconds(_sleepMS));
  }  //Program Loop
}
void ShaderCPoll::launchTasks(std::unordered_map<string_t, std::shared_ptr<ShaderFile>>& new_files, uint32_t& comp_success_count,
                              uint32_t& comp_fail_count, uint32_t& comp_error_count, uint32_t& comp_warning_count) {
  //Launch batches of _taskBatchSize tasks
  while (new_files.size()) {
    //Create batch
    std::vector<std::shared_ptr<ShaderFile>> batch;
    for (int iBatch = 0; iBatch < _taskBatchSize; ++iBatch) {
      if (new_files.size() == 0) {
        break;
      }
      batch.push_back(new_files.begin()->second);
      new_files.erase(new_files.begin());
    }

    //Launch Tasks
    std::vector<std::shared_ptr<ShaderCPollTask>> tasks;
    for (auto sfile : batch) {
      std::shared_ptr<ShaderCPoll> that = std::dynamic_pointer_cast<ShaderCPoll>(shared_from_this());
      auto ts = std::make_shared<ShaderCPollTask>(that, sfile);
      tasks.push_back(ts);
      if (this->_threaded) {
        ts->launchThread();
      }
      else {
        ts->launchSync();
      }
    }

    //Join

    for (auto ts : tasks) {
      ts->printOutput();
      comp_success_count += (ts->compiled() == SCPCompileStatus::Success) ? 1 : 0;
      comp_fail_count += (ts->compiled() == SCPCompileStatus::Failure) ? 1 : 0;
      comp_error_count += ts->errors();
      comp_warning_count += ts->warnings();
    }
  }
}
void ShaderCPoll::getFiles(bool recursive, const string_t& root, std::unordered_map<string_t, std::shared_ptr<ShaderFile>>& files) {
  //Gathers all ShaderFile files into a new std::map
  std::vector<string_t> out_filepaths;
  Utils::getAllFiles(root, out_filepaths);
  for (auto filepath : out_filepaths) {
    auto it = files.find(filepath);
    if (it == files.end()) {
      auto type = getFileType(filepath);
      if (type != nullptr) {
        auto newf = std::make_shared<ShaderFile>();
        newf->_ext = type;
        newf->_inputPath = filepath;
        files.insert(std::make_pair(filepath, newf));
      }
      else {
        //File is not a shader.
      }
    }
    else {
      files.insert(std::make_pair(filepath, it->second));
    }
  }

  if (recursive) {
    std::vector<string_t> dirs;
    Utils::getAllDirs(root, dirs);
    for (auto dir : dirs) {
      getFiles(recursive, dir, files);
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

#pragma endregion

#pragma region ShaderCPollTask

ShaderCPollTask::ShaderCPollTask(std::shared_ptr<ShaderCPoll> cpoll, std::shared_ptr<ShaderFile> file) {
  _shaderCPoll = cpoll;
  _file = file;
}
ShaderCPollTask::~ShaderCPollTask() {
}
void ShaderCPollTask::launchSync() {
  execute();
}
void ShaderCPollTask::launchThread() {
  std::weak_ptr<ShaderCPollTask> task = std::dynamic_pointer_cast<ShaderCPollTask>(shared_from_this());
  _thread = std::thread([task]() {
    if (auto spt = task.lock()) {
      spt->execute();
    }
  });
}
void ShaderCPollTask::getOutput(string_t& out_file, string_t& out_dir) {
  //Output Directory
  if (_shaderCPoll->copyFolderStructure()) {
    //Swap the input root folder to the output
    auto inpath_path = Utils::getDirectoryNameFromPath(_file->_inputPath);
    AThrow((int)inpath_path.length() >= (int)_shaderCPoll->inputDir().length());
    auto inpath_branch = inpath_path.substr(_shaderCPoll->inputDir().length(), inpath_path.length() - _shaderCPoll->inputDir().length());
    out_dir = Utils::combinePath(_shaderCPoll->output_dir(), inpath_branch);
  }
  else {
    out_dir = _shaderCPoll->output_dir();
  }

  string_t in_filename = Utils::getFileNameFromPath(_file->_inputPath);
  out_file = Utils::combinePath(out_dir, in_filename);
  out_file += _shaderCPoll->outputExt();
}
void ShaderCPollTask::execute() {
  Stopwatch sw;
  sw.start();

  //./shaderc/bin/glslc -fshader-stage=vertex ./test.vs -o ./test_vs.spv
  AThrow(_file->_ext != nullptr);

  if (!Utils::fileExists(_file->_inputPath)) {
    Utils::logError("Input file '" + _file->_inputPath + "' was not found.", &_output, _shaderCPoll->colors());
    return;
  }

  string_t in_file = _file->_inputPath;
  string_t out_dir = "";
  string_t out_file = "";
  getOutput(out_file, out_dir);

  if (!mustCompile(in_file, out_dir, out_file)) {
    _status = SCPCompileStatus::Skipped;
    return;
  }

  //Create output path
  try {
    Utils::createDirectoryRecursive(out_dir);
  }
  catch (std::exception& e) {
    Utils::logError("Exception thrown creating path recursively: " + std::string(e.what()), &_output, _shaderCPoll->colors());
    return;
  }

  //Launch shaderc
  string_t command = createCommand(out_file);
  _file->_compileOutput = Utils::executeReadOutput(command);
  _file->_outputPath = out_file;
  _file->_compileTime = Utils::getLastModifyTime(in_file);  //The basis for time returned from time(null) appears to be different from stat() so we just use this here.

  parseOutput(_file->_compileOutput);

  string_t output_file_stats = "";
  //If successful, print the output file (green)
  if (Utils::fileExists(out_file)) {
    auto mod_out = Utils::getLastModifyTime(out_file);
    if (mod_out >= _file->_compileTime) {
      output_file_stats += Stz " -> " + out_file;
      _status = SCPCompileStatus::Success;
    }
    else {
      _status = SCPCompileStatus::Failure;
    }
  }
  else {
    _status = SCPCompileStatus::Failure;
  }

  sw.end();
  //Compute time
  if (_shaderCPoll->printStats()) {
    output_file_stats += Stz " (" + sw.time() + ")";
  }

  if (output_file_stats != "") {
    addOutput(OutputLine{ output_file_stats, _shaderCPoll->colors() ? ColoredConsole::Color::FG_GREEN : ColoredConsole::Color::FG_GRAY });
  }
}
void ShaderCPollTask::parseOutput(const string_t& output) {
  _errors = 0;
  _warnings = 0;
  //Print output.
  std::vector<string_t> lines;
  Utils::split(output, { '\n' }, lines);
  for (auto& line : lines) {
    ColoredConsole::Color ccc;
    if (line.find("error:") != std::string::npos) {
      if (_shaderCPoll->outputMode() == OutputMode::Errors || _shaderCPoll->outputMode() == OutputMode::ErrorsAndWarnings) {
        ccc = ColoredConsole::Color::FG_RED;
      }
      _errors++;
    }
    else if (line.find("warning:") != std::string::npos) {
      if (_shaderCPoll->outputMode() == OutputMode::ErrorsAndWarnings) {
        ccc = ColoredConsole::Color::FG_YELLOW;
      }
      _warnings++;
    }
    else {
      ccc = ColoredConsole::Color::FG_GRAY;
    }
    addOutput(OutputLine{ line, _shaderCPoll->colors() ? ccc : ColoredConsole::Color::FG_GRAY });
  }
}
string_t ShaderCPollTask::createCommand(const string_t& out_file) {
  string_t launch = "";

#if defined(BR2_OS_WINDOWS)
  string_t curd = std::filesystem::current_path().string() + "\\";  //On Windows it doens't return with a trailing slash..
  string_t scp_noquotes = Utils::stripQuotes(_shaderCPoll->shaderc_path());
  string_t sc_path = "";
  if (!std::filesystem::path(scp_noquotes).has_root_path()) {
    sc_path = Utils::combinePath(curd, scp_noquotes);
  }
  else {
    sc_path = _shaderCPoll->shaderc_path();
  }

  launch = Stz "start \"shaderc_start\" /D\"" + curd + "\" /HIGH /B  " + Utils::enquote(sc_path) + " ";
#elif defined(BR2_OS_LINUX)
  launch = _shaderc_path + " ";
#endif

  string_t command = launch + _shaderCPoll->additionalShaderCArgs() +
                     " -fshader-stage=" + _file->_ext->_shadercVar +
                     " " + Utils::enquote(_file->_inputPath) + " -o " + Utils::enquote(out_file);

  addOutput(OutputLine{ _file->_inputPath, _shaderCPoll->colors() ? ColoredConsole::Color::FG_MAGENTA : ColoredConsole::Color::FG_GRAY });

  if (_shaderCPoll->showcmd()) {
    addOutput(OutputLine{ command, _shaderCPoll->colors() ? ColoredConsole::Color::FG_CYAN : ColoredConsole::Color::FG_GRAY });
  }

  return command;
}
bool ShaderCPollTask::mustCompile(const string_t& in_file, const string_t& out_dir, const string_t& out_file) {
  //Don't compile if we don't need to.
  time_t mod_in = Utils::getLastModifyTime(in_file);
  if (_shaderCPoll->force() == false) {
    if (Utils::fileExists(out_file)) {
      if (_file->_compileTime == 0) {
        time_t mod_out = Utils::getLastModifyTime(out_file);
        //We compiled it before but not in this run. Compare the date on the input and output files to see if it's been updated.
        if (mod_out >= mod_in) {
          return false;
        }
      }
      else {
        //Either A) we compiled before, but this time there were errors, or not and or B) the file was updated or C) the file wasn't updated.
        if (_file->_compileTime >= mod_in) {
          return false;
        }
      }
    }
    else {
      //We haven't compiled it, or, we compiled it and there were errors.
      if (_file->_compileTime == 0) {
        //File wasn't compiled, no errors.
      }
      else {
        //We compiled it, but there may have been errors. Don't keep spitting.
        time_t mod_in = Utils::getLastModifyTime(in_file);
        if (_file->_compileTime >= mod_in) {
          //We compiled it, there were errors and the user hasn't updated the file yet. Don't keep spitting.
          return false;
        }
      }
    }
  }
  return true;
}
void ShaderCPollTask::printOutput() {
  if (_thread.joinable()) {
    _thread.join();
  }
  for (auto& outputLine : _output) {
    ColoredConsole::print(Stz outputLine._msg + "\n", outputLine._color);
  }
}

#pragma endregion

}  // namespace SCP