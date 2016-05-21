#ifndef _DIR_DEALER_H_
#define _DIR_DEALER_H_

#include <string>
#include <vector>
using namespace std;

class DirDealer {
public:
  static string int2String(int i);
  static int String2int(string str);
  static string getSuffixRemoved(string name);
  static void seperatePathName(string full_name, string &path, string &name);

  // name list
  static std::vector<std::string> getNameList(const std::string &path);
  static std::vector<std::string> getNameListValid(const std::string &path);
  static std::vector<std::string> getNameListValidText(const std::string &path);
  static std::vector<std::string>
  getNameListValidImage(const std::string &path);
  static std::vector<std::string>
  getNameListValidVideo(const std::string &path);

  static vector<string> getFileList(string path);
  static vector<string> getFileListRecursive(string path);

  static void saveList(vector<string> &list, string name);
  static void readList(vector<string> &list, string name);
  static void printList(vector<string> &list);

  static void mkdirRecursive(string path, string list_file);
  static void mkdirRecursive(string path, vector<string> &list);

  static void mkdir_p(string dir);
  static void rm_rf(string dir);

  static bool existFileOrDir(string name);
  static bool isDir(string dir);

private:
  static vector<string> getFileListRec(string path, string dir);
  static vector<string> getListFromString(string str);
};

#endif
