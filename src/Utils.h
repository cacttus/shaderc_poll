/**
*  @file shaderc_poll.h
*  @date Nov 4, 2020
**/
#pragma once
#ifndef __UTILS_006799117548949000362_H__
#define __UTILS_006799117548949000362_H__

#include "./ShaderCPollHeader.h"

namespace SCP {

class Utils {
public:
  static string_t _exeLocation;
  static void logError(const string_t&);
  static void logWarn(const string_t&);
  static void logInfo(const string_t&);
  static string_t getDate();
  static bool equals(const string_t& a, const string_t& b);
  static bool beginsWith(const string_t& search_str, const string_t& prefix);
  static string_t getEnvironmentVariable(const string_t& var);
  static void debugBreak();
  static string_t stripQuotes(const string_t& s);
  static bool fileExists(const string_t& filename);
  static bool directoryExists(const string_t& dirName);
  static string_t executeReadOutput(const string_t& cmd);
  static bool createDirectoryRecursive(const string_t& dirName);
  static void throwException(const string_t& ex);
  static bool isFile(const string_t& fileOrDirPath);
  static bool isDir(const string_t& fileOrDirPath);
  static bool getAllFilesOrDirs(const string_t& dir, std::vector<string_t>& dirs, bool bFiles);
  static bool getAllDirs(const string_t& dir, std::vector<string_t>& dirs);
  static bool getAllFiles(const string_t& dir, std::vector<string_t>& files);
  static string_t combinePath(const std::string& a, const std::string& b);
  static string_t getExtensionPartOfFileName(const string_t& name);
  static string_t getFileNameFromPath(const string_t& name);
  static string_t getDirectoryNameFromPath(const string_t& pathName);
  static string_t formatPath(const string_t& p);
  static string_t replaceAll(const string_t& str, char charToRemove, char charToAdd);
  static string_t replaceAll(const string_t& str, const string_t& strToRemove, const string_t& strToAdd);
  static time_t getLastModifyTime(const string_t& location);
  static string_t getFilePartOfFileName(const string_t& fileName);
  static void findFile(FileInfo& inf, const string_t& search_root);
  static void split(const string_t& in, const std::vector<char>& dels, std::vector<string_t>& ret);
};

}  // namespace SCP

#endif