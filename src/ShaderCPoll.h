/**
*  @file shaderc_poll.h
*  @date Nov 4, 2020
**/
#pragma once
#ifndef __SHADERC_POLL_8923479870061385691283_H__
#define __SHADERC_POLL_8923479870061385691283_H__

#include "./ShaderCPollHeader.h"

namespace SCP {
enum class SCPCompileStatus {
  Unset,
  Skipped,
  Success,
  Failure
};
  /**
* @class ShaderCPollTask
* @brief A shader module compilation task.
*/
class ShaderCPollTask : public std::enable_shared_from_this<ShaderCPollTask> {
public:
  ShaderCPollTask(std::shared_ptr<ShaderCPoll> cpoll, std::shared_ptr<ShaderFile> file);
  virtual ~ShaderCPollTask();

  void launchThread();
  void launchSync();
  void printOutput();
  SCPCompileStatus compiled() { return _status; }
  int32_t errors() { return _errors; }
  int32_t warnings() { return _warnings; }

private:
  SCPCompileStatus _status = SCPCompileStatus::Unset;
  std::vector<OutputLine> _output;
  void addOutput(const OutputLine& line) {
    _output.push_back(line);
  }
  void execute();
  bool mustCompile(const string_t& in_dir, const string_t& out_dir, const string_t& out_file);
  void getOutput(string_t& out_file, string_t& out_dir);
  string_t createCommand(const string_t& out_file);
  void parseOutput(const string_t& output);

  std::shared_ptr<ShaderFile> _file = nullptr;
  std::shared_ptr<ShaderCPoll> _shaderCPoll = nullptr;
  std::thread _thread;
  int32_t _errors = 0;
  int32_t _warnings = 0;
};
/**
* @class ShaderCPoll
* @brief Checks file changes and schedules shader compilation tasks.
*/
class ShaderCPoll : public std::enable_shared_from_this<ShaderCPoll> {
public:
  ShaderCPoll();
  virtual ~ShaderCPoll();

  void init(std::vector<std::string>& args);
  void run();

  bool copyFolderStructure() { return _copyFolderStructure; }
  const string_t& output_dir() { return _output_dir; }
  const string_t& outputExt() { return _outputExt; }
  bool force() { return _force; }
  const string_t& shaderc_path() { return _shaderc_path; }
  const string_t& additionalShaderCArgs() { return _additionalShaderCArgs; }
  bool colors() { return _colors; }
  bool showcmd() { return _showcmd; }
  OutputMode outputMode() { return _outputMode; }
  const string_t& inputDir() { return _input_dir; }
  bool printStats() { return _printStats; }

private:
  std::shared_ptr<ShaderTypeExt> getFileType(const string_t& file);
  string_t nextArg(std::vector<std::string>& args, size_t& iArg);
  string_t parseArg(const string_t& arg);
  void getFiles(bool recursive, const string_t& root, std::unordered_map<string_t, std::shared_ptr<ShaderFile>>& newFiles);
  bool parseExtensions(const string_t& arg);
  bool parseExt(const string_t& arg, const string_t& flag, string_t& outExt);
  void launchTasks(std::unordered_map<string_t, std::shared_ptr<ShaderFile>>& new_files, uint32_t& comp_success_count, uint32_t& comp_fail_count, uint32_t& comp_error_count, uint32_t& comp_warning_count);

  const char* c_mkod_cmd = "-makeoutputdir";
  string_t _input_dir;
  string_t _output_dir;
  string_t _shaderc_path;
  string_t _additionalShaderCArgs;
  string_t _outputExt = ".spv";
  int32_t _sleepMS = 250;
  OutputMode _outputMode = OutputMode::ErrorsAndWarnings;
  //RecursiveMode _recursiveMode = RecursiveMode::Off;
  std::vector<std::shared_ptr<ShaderTypeExt>> _extensions;
  bool _colors = true;
  bool _copyFolderStructure = true;
  bool _recursive = false;
  bool _force = false;
  bool _showcmd = false;
  bool _threaded = false;
  int32_t _defaultTaskBatchSize = 50;
  int32_t _taskBatchSize = _defaultTaskBatchSize;
  bool _nobanner = true;
  bool _makeOutputDir = false;
  bool _printStats = false;
};

}  // namespace SCP

#endif