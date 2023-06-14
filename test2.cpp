#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#define BUFF_SIZE 100

int processFile(std::ifstream& file, int &byte_reading) {
	
	file.seekg (0, file.end);
	int length = file.tellg();
	file.seekg (0, file.beg);
	std::vector<char> buffer(100);

	if (byte_reading < length)
	{
		int chunkSize = std::min(BUFF_SIZE, length - byte_reading);
		// int chunkSize = std::min(length, length - byte_reading);
		buffer.resize(chunkSize);
		file.seekg(position);
		file.read(buffer.data(), chunkSize);
		position = file.tellg();
		byte_reading += file.gcount();
		if (file.gcount() == 0)
		{
			file.close();
			return (-1);
		}
	}
	return (length);
}

int main() {
    // Open the file
    std::ifstream file("srcs/index.html");
    std::ifstream file2("www/400.html");
	int *byte_reading1 = 0;
	int *byte_reading2 = 0;
    // Check if the file was successfully opened
    if (!file) {
        std::cerr << "Failed to open the file." << std::endl;
        return 1;
    }
	if (!file2) {
        std::cerr << "Failed to open the file2." << std::endl;
        return 1;
    }
	while (1)
	{
		if(byte_reading1 < )

	}
    // Pass the file to the function

    processFile(file1);
    processFile(file2);

    return 0;
}