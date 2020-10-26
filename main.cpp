#include <fstream>
#include "CSVWriter.hpp"
#include "CSVReader.hpp"

static void printVector(const std::vector<double> row) {
	if (row.empty())
		std::cout << std::endl;

	std::cout << row[0];
	for (int i = 1; i < row.size(); ++i)
		std::cout << ',' << row[i];
	std::cout << std::endl;
}

static void copy(const std::string &inFilePath, const std::string &outFilePath) {
	std::ifstream inStream(inFilePath);
	CSVReader reader(inStream);

	std::ofstream outStream(outFilePath);
	CSVWriter writer(outStream);

	std::vector<double> row;
	while (reader.read(row)) {
		writer.write(row);
	}
	inStream.close();
	outStream.close();
}

int main(int argc, char *argv[])
{
	if (argc < 3) {
		std::cerr << "usage: " << argv[0] << " INPUT OUTPUT" << std::endl;
		return -1;
	}

	copy(argv[1], argv[2]);
	return 0;
}
