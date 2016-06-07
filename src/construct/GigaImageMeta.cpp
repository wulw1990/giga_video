#include "GigaImageMeta.hpp"

#include <iostream>
#include <fstream>
using namespace std;

#include "DirDealer.hpp"

void GigaImageMeta::generateMetaFile(std::string path, std::string meta) {
  // TODO: magic number
  int layers;
  {
    layers = 0;
    for (int i = 0; i < 1000; ++i) {
      if (!DirDealer::isDir(path + to_string(i)))
        break;
      layers++;
    }
    cout << "GigaImageMeta::generateMetaFile layers=" << layers << endl;
  }
  int head_layers;
  {
    head_layers = 0;
    for (int i = 0; i < layers; ++i) {
      if (!DirDealer::existFileOrDir(path + to_string(i) + "/0_0.jpg"))
        break;
      head_layers++;
    }
    cout << "GigaImageMeta::generateMetaFile head_layers=" << head_layers
         << endl;
  }
  const string TILE_NAME_SUFFIX = ".jpg";
  m_tile_len = 512;

  DirDealer dir_dealer;

  m_tile_name.clear();

  for (int layer_id = 0; layer_id < head_layers; ++layer_id) {
    int rows = 0;
    int cols = 0;
    string path_layer = path + to_string(layer_id) + "/";
    vector<string> list = dir_dealer.getFileList(path_layer);
    for (size_t i = 0; i < list.size(); ++i) {
      // cout << tile_name_list[i] << endl;
      string name = list[i];
      std::size_t pos1 = name.find("_");
      std::size_t pos2 = name.find(".");
      if (pos1 == std::string::npos || pos2 == std::string::npos) {
        cerr << "name error" << endl;
        exit(-1);
      }
      // cout << pos1 << "\t" << pos2 << endl;
      int r = stoi(name.substr(0, pos1));
      int c = stoi(name.substr(pos1 + 1, pos2 - pos1));
      // cout << r << " " << c << endl;
      rows = max(rows, r);
      cols = max(cols, c);
    }
    rows++;
    cols++;
    vector<vector<string>> layer_tile_name;
    for (int r = 0; r < rows; ++r) {
      vector<string> row_tile_name;
      for (int c = 0; c < cols; ++c) {
        string tile_name = to_string(layer_id) + "/" + to_string(r) + "_" +
                           to_string(c) + TILE_NAME_SUFFIX;
        row_tile_name.push_back(tile_name);
      }
      layer_tile_name.push_back(row_tile_name);
    }
    m_tile_name.push_back(layer_tile_name);
  } // for layer_id
  for (int layer_id = head_layers; layer_id < layers; ++layer_id) {
    string path_layer = path + to_string(layer_id) + "/";
    vector<string> row_name_list = dir_dealer.getFileList(path_layer);
    if (row_name_list.empty()) {
      cerr << "row_name_list.empty()" << endl;
      exit(-1);
    }
    vector<string> col_name_list =
        dir_dealer.getFileList(path_layer + row_name_list[0]);
    int rows = static_cast<int>(row_name_list.size());
    int cols = static_cast<int>(col_name_list.size());
    vector<vector<string>> layer_tile_name;
    for (int r = 0; r < rows; ++r) {
      vector<string> row_tile_name;
      for (int c = 0; c < cols; ++c) {
        string tile_name = to_string(layer_id) + "/" + to_string(r) + "/" +
                           to_string(c) + TILE_NAME_SUFFIX;
        row_tile_name.push_back(tile_name);
      }
      layer_tile_name.push_back(row_tile_name);
    }
    m_tile_name.push_back(layer_tile_name);
  } // for layer_id

  ofstream fout(meta);
  if (!fout.is_open()) {
    cerr << "error: file " << meta << endl;
    exit(-1);
  }
  fout << m_tile_len << endl;
  fout << layers << endl;
  for (int layer_id = 0; layer_id < layers; ++layer_id) {
    int rows = getRows(layer_id);
    int cols = getCols(layer_id);
    fout << layer_id << "\t" << rows << "\t" << cols << endl;
    for (int r = 0; r < rows; ++r) {
      for (int c = 0; c < cols; ++c) {
        fout << m_tile_name[layer_id][r][c] << endl;
      }
    }
  }
  cout << "GigaImageMeta::generateMetaFile ok" << endl;
}
void GigaImageMeta::load(std::string meta) {
  ifstream fin(meta);
  if (!fin.is_open()) {
    cerr << "error: file " << meta << endl;
    exit(-1);
  }
  m_tile_name.clear();

  fin >> m_tile_len;
  int layers;
  fin >> layers;

  for (int layer_id = 0; layer_id < layers; ++layer_id) {
    int layer_id_, rows, cols;
    fin >> layer_id_ >> rows >> cols;
    vector<vector<string>> layer_tile_name;
    for (int r = 0; r < rows; ++r) {
      vector<string> row_tile_name;
      for (int c = 0; c < cols; ++c) {
        string name;
        fin >> name;
        row_tile_name.push_back(name);
      }
      layer_tile_name.push_back(row_tile_name);
    }
    m_tile_name.push_back(layer_tile_name);
  } // for layer_id
}
int GigaImageMeta::getLayers() {
  // getLayers
  return m_tile_name.size();
}
int GigaImageMeta::getRows(int layer_id) {
  // getRows
  int layers = m_tile_name.size();
  if (layer_id < 0 || layer_id >= layers)
    return -1;
  return m_tile_name[layer_id].size();
}
int GigaImageMeta::getCols(int layer_id) {
  // getCols
  int layers = m_tile_name.size();
  if (layer_id < 0 || layer_id >= layers)
    return -1;
  return m_tile_name[layer_id][0].size();
}
int GigaImageMeta::getTileLen() {
  // getTileLen
  return m_tile_len;
}
std::string GigaImageMeta::getTileName(int layer_id, int r, int c) {
  int layers = m_tile_name.size();
  if (layer_id < 0 || layer_id >= layers)
    return "";
  int rows = getRows(layer_id);
  int cols = getCols(layer_id);
  if (r < 0 || r >= rows)
    return "";
  if (c < 0 || c >= cols)
    return "";
  return m_tile_name[layer_id][r][c];
}
