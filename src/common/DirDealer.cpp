#include "DirDealer.hpp"

#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <stdlib.h>

static string exec(string cmd) {
  FILE *pipe = popen(cmd.c_str(), "r");
  if (!pipe)
    return "ERROR";
  char buffer[128];
  std::string result = "";
  while (!feof(pipe)) {
    if (fgets(buffer, 128, pipe) != NULL)
      result += buffer;
  }
  pclose(pipe);
  return result;
}
static vector<string> getLineFromString(string str) {
  stringstream ss;
  ss << str;
  vector<string> list;
  while (getline(ss, str))
    list.push_back(str);
  return list;
}
static bool hasSpaceChar(const string &s) {
  for (size_t i = 0; i < s.length(); ++i) {
    if (isspace(s[i]))
      return true;
  }
  return false;
}
void DirDealer::systemInternal(std::string cmd) {
  if (system(cmd.c_str())) {
    cerr << "systemInternal call failed: " << cmd << endl;
    exit(-1);
  }
}

std::vector<std::string> DirDealer::getNameList(const std::string &path) {
  string cmd;
  cmd = string("ls \"") + path + "\"";
  string result = exec(cmd);
  vector<string> list = getLineFromString(result);
  return list;
}
std::vector<std::string> DirDealer::getNameListValid(const std::string &path) {
  vector<string> list = getNameList(path);
  vector<string> list_valid;
  for (size_t i = 0; i < list.size(); ++i) {
    if (!hasSpaceChar(list[i]))
      list_valid.push_back(list[i]);
  }
  return list_valid;
}
std::vector<std::string>
DirDealer::getNameListValidText(const std::string &path) {
  vector<string> list_src = getNameListValid(path);
  map<string, int> mm;
  mm[".txt"] = 1;
  vector<string> list_dst;
  for (size_t i = 0; i < list_src.size(); ++i) {
    string name = list_src[i];
    if (name.length() < 4)
      continue;
    string ext = name.substr(name.length() - 4, 4);
    if (mm[ext] == 1) {
      list_dst.push_back(name);
    }
  }
  return list_dst;
}
std::vector<std::string>
DirDealer::getNameListValidImage(const std::string &path) {
  vector<string> list_src = getNameListValid(path);
  map<string, int> mm;
  mm[".jpg"] = 1;
  mm[".png"] = 1;
  mm[".bmp"] = 1;
  vector<string> list_dst;
  for (size_t i = 0; i < list_src.size(); ++i) {
    string name = list_src[i];
    if (name.length() < 4)
      continue;
    string ext = name.substr(name.length() - 4, 4);
    if (mm[ext] == 1) {
      list_dst.push_back(name);
    }
  }
  return list_dst;
}
std::vector<std::string>
DirDealer::getNameListValidVideo(const std::string &path) {
  vector<string> list_src = getNameListValid(path);
  map<string, int> mm;
  mm[".avi"] = 1;
  mm[".mp4"] = 1;
  vector<string> list_dst;
  for (size_t i = 0; i < list_src.size(); ++i) {
    string name = list_src[i];
    if (name.length() < 4)
      continue;
    string ext = name.substr(name.length() - 4, 4);
    if (mm[ext] == 1) {
      list_dst.push_back(name);
    }
  }
  return list_dst;
}
vector<string> DirDealer::getFileList(string name) {
  string cmd;
  cmd = string("ls \"") + name + "\"";
  string result = exec(cmd);
  vector<string> list = getLineFromString(result);
  return list;
}
bool DirDealer::existFileOrDir(string name) {
  fstream f;
  f.open(name.c_str(), ios::in);
  if (f)
    return true;
  else
    return false;
}
bool DirDealer::isDir(string dir) {
  if (!existFileOrDir(dir))
    return false;
  string cmd;
  cmd = string("ls -a \"") + dir + "\"";
  string result = exec(cmd);
  vector<string> list = getLineFromString(result);
  if (list[0] == "." && list[1] == "..")
    return true;
  else
    return false;
}
void DirDealer::mkdir_p(string dir) {
  string cmd = "mkdir -p ";
  cmd += "\"" + dir + "\"";
  systemInternal(cmd.c_str());
}
void DirDealer::rm_rf(string dir) {
  string cmd = "rm -rf ";
  cmd += dir;
  systemInternal(cmd.c_str());
}
string DirDealer::int2String(int i) {
  stringstream ss;
  ss << i;
  return ss.str();
}

int DirDealer::String2int(string str) {
  stringstream ss;
  ss << str;
  int i;
  ss >> i;
  return i;
}
vector<string> DirDealer::getFileListRec(string path, string dir) {
  vector<string> list = getFileList(path + dir);
  vector<string> result;

  for (int i = 0; i < (int)list.size(); ++i) {
    // cout << path + dir + list[i] << endl;
    if (list[i][list[i].length() - 1] == '~') {
      continue;
    }
    // cout << list[i] << endl;
    if (isDir(path + dir + list[i])) {
      vector<string> tmp = getFileListRec(path, dir + list[i] + "/");
      result.insert(result.end(), tmp.begin(), tmp.end());
    } else
      result.push_back(dir + list[i]);
  }
  return result;
}
vector<string> DirDealer::getFileListRecursive(string path) {
  return getFileListRec(path, "");
}
void DirDealer::saveList(vector<string> &list, string name) {
  ofstream fout(name.c_str());
  fout << list.size() << endl;
  for (int i = 0; i < (int)list.size(); ++i)
    fout << list[i] << endl;
}
void DirDealer::readList(vector<string> &list, string name) {
  ifstream fin(name.c_str());
  int n = -1;
  fin >> n;
  list.resize(n);
  for (int i = 0; i < n; ++i)
    fin >> list[i];
  fin.close();
}
void DirDealer::printList(vector<string> &list) {
  for (int i = 0; i < (int)list.size(); ++i)
    cout << list[i] << endl;
}
void DirDealer::mkdirRecursive(string path, string list_file) {
  int n;
  ifstream fin(list_file.c_str());
  fin >> n;
  vector<string> list(n);
  for (int i = 0; i < n; ++i) {
    fin >> list[i];
  }
  mkdirRecursive(path, list);
  fin.close();
}
void DirDealer::mkdirRecursive(string path, vector<string> &list) {
  int n = (int)list.size();
  for (int i = 0; i < n; ++i) {
    string dir, name;
    seperatePathName(list[i], dir, name);
    // cout << dir << endl;
    if (!isDir(path + dir))
      mkdir_p(path + dir);
  }
}
string DirDealer::getSuffixRemoved(string name) {
  const int n = (int)name.length();
  int i = n - 1;
  for (; i >= 0; --i) {
    if (name[i] == '.')
      break;
  }
  return name.substr(0, i);
}
void DirDealer::seperatePathName(string full_name, string &path, string &name) {
  int n = (int)full_name.length();
  int i = n - 1;
  for (; i >= 0; --i) {
    if (full_name[i] == '/')
      break;
  }
  path = full_name.substr(0, i + 1);
  name = full_name.substr(i + 1, n - (i + 1));
}
