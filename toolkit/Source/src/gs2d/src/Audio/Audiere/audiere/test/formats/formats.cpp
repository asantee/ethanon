#include <iostream>
#include <stdlib.h>
#include "audiere.h"
using namespace std;
using namespace audiere;


int main(int argc, char** argv) {

  cout << endl;
  cout << "File formats supported by this compile of Audiere:" << endl;
  cout << endl;

  vector<FileFormatDesc> ffd;
  GetSupportedFileFormats(ffd);

  if (ffd.empty()) {
    cout << "No formats supported." << endl;
    return EXIT_SUCCESS;
  }

  for (int i = 0; i < ffd.size(); ++i) {
    cout << ffd[i].description << endl;
    cout << "\t";
    for (int j = 0; j < ffd[i].extensions.size(); ++j) {
      cout << ffd[i].extensions[j] << " ";
    }
    cout << endl;
  }
}
