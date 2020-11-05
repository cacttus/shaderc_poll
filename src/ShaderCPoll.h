/**
*  @file shaderc_poll.h
*  @date Nov 4, 2020
**/
#pragma once
#ifndef __SHADERC_POLL_8923479870061385691283_H__
#define __SHADERC_POLL_8923479870061385691283_H__

#include "./ShaderCPollHeader.h"

namespace SCP {

class ShaderCPoll {
public:
  ShaderCPoll();
  virtual ~ShaderCPoll();

  void init(std::vector<std::string>& args);
  void run();

private:
  string_t _input_dir;
  string_t _output_dir;
  string_t _shaderc_path;
  string_t _additionalShaderCArgs;
  string_t _outputExt = ".spv";
  bool _colors = false;
  bool _errors = true;
  bool _warnings = true;
  int32_t _sleepMS = 250;
  OutputMode _outputMode = OutputMode::ErrorsAndWarnings;
  //RecursiveMode _recursiveMode = RecursiveMode::Off;
  bool _recursive = false;
  std::vector<std::shared_ptr<ShaderTypeExt>> _extensions;
  std::unordered_map<string_t, std::shared_ptr<ShaderFile>> _files;
  bool _copyFolderStructure = true;
  bool _force = false;
  bool _showcmd = false;
  bool _threaded = false;
  bool _nobanner = false;
  std::shared_ptr<ShaderTypeExt> getFileType(const string_t& file);
  string_t nextArg(std::vector<std::string>& args, size_t& iArg);
  string_t parseArg(const string_t& arg);
  void processFile(std::shared_ptr<ShaderFile> file);
  void getFiles(bool recursive, const string_t& root, std::unordered_map<string_t, std::shared_ptr<ShaderFile>>& newFiles);
  bool parseExtensions(const string_t& arg);
  bool parseExt(const string_t& arg, const string_t& flag, string_t& outExt);
};

}  // namespace SCP

#endif