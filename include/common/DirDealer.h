#ifndef _DIR_DEALER_H_
#define _DIR_DEALER_H_

#include <string>
#include <vector>
using namespace std;

class DirDealer
{
	public:
		static string int2String(int i);
		static int String2int(string str);
		string getSuffixRemoved(string name);
		void seperatePathName(string full_name, string& path, string& name);

		vector<string> getFileList(string path);
		vector<string> getFileListRecursive(string path);

		void saveList(vector<string>& list, string name);
		void readList(vector<string>& list, string name);
		void printList(vector<string>& list);

		void mkdirRecursive(string path, string list_file);
		void mkdirRecursive(string path, vector<string>& list);

		static void mkdir_p(string dir);
		static void rm_rf(string dir);

		static bool existFileOrDir(string name);
		static bool isDir(string dir);
	private:
		vector<string> getFileListRec(string path, string dir);
		vector<string> getListFromString(string str);
};

#endif
