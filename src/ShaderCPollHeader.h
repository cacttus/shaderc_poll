/**
*  @date Nov 4, 2020
**/
#pragma once
#ifndef __SHADERC_POLL_HEADER_003867910599968275923632_H__
#define __SHADERC_POLL_HEADER_003867910599968275923632_H__

#include <iostream>
#include <string>
#include <vector>
#include <bitset>
#include <thread>
#include <mutex>
#include <chrono>
#include <unordered_map>
#include <map>

#define OS_NOT_SUPPORTED_ERROR error "Operating system not supported"

/////////////////////////////////////////////////////////////////////////

#define BR2_CPP17

/////////////////////////////////////////////////////////////////////////

#ifdef BR2_CPP17
#include <filesystem>
#endif

//G++ / CLANG - NDEBUG is enabled by DEFAULT messing up builds.
#ifdef _DEBUG
#ifdef NDEBUG
#undef NDEBUG
#endif
#ifdef _NDEBUG
#undef _NDEBUG
#endif
#endif

#if defined(linux)
#define BR2_OS_LINUX
#elif defined(_WIN32)
#define BR2_OS_WINDOWS
#endif

#ifdef BR2_OS_LINUX
#include <signal.h>
#endif

#define AThrow(x) \
  if (!(x)) { Utils::throwException("Assertion failed: " + std::string(#x)); }

#define Stz std::string("") +

namespace SCP {

typedef std::string string_t;

enum class OutputMode {
  Off,
  Errors,
  ErrorsAndWarnings
};
enum class RecursiveMode {
  Off,
  Recursive,
  RecursiveMatch
};
enum class ShaderType {
  Unset,
  Geometry,
  Vertex,
  Fragment,
  TessControl,
  TessEval,
  Task_NV,
  Mesh_NV,
  Compute
};
struct ShaderTypeExt;
struct ShaderFile;
class ShaderCPoll;
class Utils;

class FileInfo {
  friend class Utils;

public:
  string_t _file_without_path = "";
  const string_t& path() { return _path; }
  time_t modified() { return _modified; }
  bool found() { return _found; }
  time_t _modified = 0;
  string_t _path = "";
  bool _found = false;
};
class ShaderFile {
public:
  string_t _inputPath = "";
  string_t _outputPath = "";  //The found output file.
  time_t _compileTime = 0;
  std::shared_ptr<ShaderTypeExt> _ext = nullptr;
  string_t _compileOutput = "";
};
class ShaderTypeExt {
public:
  ShaderType _type = ShaderType::Unset;
  string_t _ext = "";
  string_t _shadercVar = "";  //-fshader-type=vertex
  string_t _flag = "";
  ShaderTypeExt(ShaderType t, const string_t& ext, const string_t& shadercvar, const string_t& flag) {
    _type = t;
    _ext = ext;
    _shadercVar = shadercvar;
    _flag = flag;
  }
};

}  // namespace SCP

#endif