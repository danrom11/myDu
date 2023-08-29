
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include <cstdint>
#include <cstdio>
#include <getopt.h>

#define SIZE_BLOCK 512

namespace fs = std::filesystem;

uint8_t flag_h = 0;
uint8_t flag_b = 0;
uint8_t flag_a = 0;
uint8_t flag_c = 0;
uint8_t flag_s = 0;
uint8_t flag_f = 0;

size_t bytesToSizeBlock(size_t bytes) {
  if (bytes == 0)
    return 1;
  return static_cast<size_t>(
      std::ceil(static_cast<double>(bytes) / SIZE_BLOCK));
}

size_t getFileSize(std::string filename) // path to file
{
  FILE *p_file = NULL;
  p_file = fopen(filename.c_str(), "rb");
  if (p_file == NULL) {
    std::cerr << "Unable to open the file" << std::endl;
    return 0;
  }
  fseek(p_file, 0, SEEK_END);
  size_t size = ftell(p_file);
  fclose(p_file);
  return size;
}

size_t openDirFile(std::string path) {
  if (!fs::exists(path)) {
    std::cerr << "No such directory: " << path << std::endl;
    return 0;
  }

  size_t thisSizeDir = 0;
  size_t fileSize = 0;
  for (const auto &entry : fs::directory_iterator(path)) {
    if (!fs::is_directory(entry.path().string())) {
      fileSize = getFileSize(path + entry.path().filename().string());
      thisSizeDir += fileSize;
      if (flag_a && !flag_s)
        std::cout << (flag_b ? fileSize : bytesToSizeBlock(fileSize)) << "\t"
                  << path + entry.path().filename().string() << std::endl;
    } else {
      thisSizeDir += openDirFile(path + entry.path().filename().string() + "/");
    }
  }

  if (!flag_s) {
    if (thisSizeDir > 0)
      std::cout << (flag_b ? thisSizeDir : bytesToSizeBlock(thisSizeDir))
                << "\t" << path << std::endl;
    else
      std::cout << (flag_b ? 0 : 1) << "\t" << path << std::endl;
  }
  return thisSizeDir;
}

void help() {
  std::cout << "myDu - estimate file space usage" << std::endl;

  std::cout << "DESCRIPTION" << std::endl
            << "Summarize disk usage of the set of FILEs, recursively for "
               "directories."
            << std::endl
            << "Mandatory arguments to long options are mandatory for short "
               "options too."
            << std::endl
            << std::endl;
  ;

  std::cout << "the -b/--bytes parameter outputs the exact size in bytes"
            << std::endl;
  std::cout << "the -a/--all parameter outputs data and for each file"
            << std::endl;
  std::cout << "the -c/--total parameter outputs the total size at the end"
            << std::endl;
  std::cout << "the -s/--summarize parameter outputs only the total size"
            << std::endl;
  std::cout << "the -f/files-from parameter allows you to specify the file "
               "from which the paths will be taken"
            << std::endl;
}

std::vector<std::string> split(const std::string &str, char sp) {
  std::stringstream ss(str);
  std::string item;
  std::vector<std::string> elements;
  while (std::getline(ss, item, sp)) {
    elements.push_back(std::move(item));
  }
  return elements;
}

int main(int argc, char **argv) {

  int r = 0;
  int option_index = 0;

  char *pathDir = NULL;

  const char *short_options = "habcsf:";

  const struct option long_options[] = {
      {"help", no_argument, NULL, 'h'},
      {"all", no_argument, NULL, 'a'},
      {"bytes", no_argument, NULL, 'b'},
      {"total", no_argument, NULL, 'c'},
      {"summarize", no_argument, NULL, 's'},
      {"files-from", required_argument, NULL, 'f'},
      {NULL, 0, NULL, 0}};

  while ((r = getopt_long(argc, argv, short_options, long_options,
                          &option_index)) != -1) {
    switch (r) {
    case 'h':
      flag_h = 1;
      break;
    case 'a':
      flag_a = 1;
      break;
    case 'b':
      flag_b = 1;
      break;
    case 'c':
      flag_c = 1;
      break;
    case 's':
      flag_s = 1;
      break;

    case 'f':
      if (optarg != NULL) {
        flag_f = 1;
        pathDir = optarg;
      }
      break;
    case '?':
    default:
      std::cerr << "unkown option" << std::endl;
      break;
    }
  }

  std::vector<std::string> paths;
  for (int i = 1 + flag_a + flag_b + flag_c + flag_s; i < argc; i++) {
    paths.push_back(argv[i]);
  }

  if (paths.size() == 0 && !flag_f) {
    std::cerr << "Not found path!" << std::endl;
    return 0;
  }

  size_t byteSize = 0;
  if (flag_f) {
    if (pathDir[0] == '=')
      pathDir++;

    std::fstream file(pathDir);
    std::string line;
    while (std::getline(file, line)) {
      if (fs::exists(line)) {
        byteSize = openDirFile(line);
        if (flag_s) {
          std::cout << (flag_b ? byteSize : bytesToSizeBlock(byteSize)) << "\t"
                    << line << std::endl;
        }
        if (flag_c) {
          std::cout << (flag_b ? byteSize : bytesToSizeBlock(byteSize)) << "\t"
                    << "total" << std::endl;
        }
      } else {
        std::cerr << "No such directory: " << line << std::endl;
      }
    }
  } else {
    for (size_t i = 0; i < paths.size(); i++) {
      byteSize = openDirFile(paths.at(i));
      if (flag_s) {
        std::cout << (flag_b ? byteSize : bytesToSizeBlock(byteSize)) << "\t"
                  << paths.at(i) << std::endl;
      }
      if (flag_c) {
        std::cout << (flag_b ? byteSize : bytesToSizeBlock(byteSize)) << "\t"
                  << "total" << std::endl;
      }
    }
  }

  return 0;
}
