// FolderReader.cpp

#include "FolderReader.hpp"

using namespace std;

namespace ECE141 {
    bool hasExtension(const string &aPath, const string &anExtension) {
      if (!anExtension.empty()) {
        int idx = (int)(aPath.rfind('.'));
        if (idx != -1) {
          return aPath.substr(idx + 1) == (anExtension.find('.') != -1 ? anExtension.substr(1) : anExtension);
        }
      }
      return false;
    }

    void FolderReader::each(FolderListener &aListener, const std::string &anExtension) const {
      //STUDENT: iterate db's, pass the name of each to listener
      // as for extension, * means every file in the dir
      for (auto& p : fs::directory_iterator(path)) {
        if (hasExtension(p.path(), anExtension)) {
          aListener(p.path());
        }
      }
    }
}

