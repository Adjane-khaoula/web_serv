#include <iostream>     // std::cout
#include <fstream>      // std::ifstream
#include <vector>
int main () {

  std::ifstream is ("srcs/index.html", std::ifstream::binary);
  if (is) {
    // get length of file:
    is.seekg (0, is.end);
    int length = is.tellg();
    is.seekg (0, is.beg);

  // char * buffer = new char [length];
  std::vector<char> buffer(100);
  // read data as a block:
  int byte_reading = 0;
  while (byte_reading < length) {
  int chunkSize = std::min(50, length - byte_reading);
  buffer.resize(chunkSize);
  is.read(buffer.data(), chunkSize);

  std::cout << "********************" << std::endl;
  std::cout << "Reading " << is.gcount() << " characters... \n";
  std::cout << "all characters read successfully." << std::endl;
  std::cout << buffer.size() << std::endl;

  for (int i = 0; i < is.gcount(); ++i) {
      std::cout << "\033[32m" << buffer[i] << "\033[0m";
  }
  std::cout << std::endl;

  byte_reading += is.gcount();
  if (is.gcount() == 0)
      break;
}

  // while (byte_reading < length)
	// {
  //     buffer.clear();
  //     int chunkSize = std::min(100, length - byte_reading);
  //     buffer.resize(chunkSize);
  //     is.read (buffer.data() + byte_reading , chunkSize);
  //     std::cout << "********************"<< std::endl;
  //   	std::cout << "Reading " << is.gcount() << " characters... \n";
  //     std::cout << "all characters read successfully." << std::endl;
  //     std::cout << buffer.size() << std::endl;
  //     for (int i = 0; i< is.gcount(); ++i) {
  //           std::cout << "\033[32m"<< buffer[i]<< "\033[0m";
  //     }
  //     std::cout << std::endl;
  //     byte_reading += is.gcount();
  //     if (is.gcount() == 0) 
  //        break;
	// }
  // buffer.clear();
  // is.read (buffer.data() + byte_reading , length - byte_reading);
  // std::cout << "Reading " << is.gcount() << " characters... \n";
	// 	  // std::cout << "\033[32m" <<buffer << "\033[0m" << std::endl;
  //  for (std::vector<char>::const_iterator it = buffer.begin(); it != buffer.end(); ++it) {
  //          std::cout << "\033[32m"<< *it<< "\033[0m";
  //     }
  //     std::cout << "all characters read successfully." << std::endl;
    // else
    //   std::cout << "error: only " << is.gcount() << " could be read";
    is.close();

    // ...buffer contains the entire file...

  }
  return 0;
}