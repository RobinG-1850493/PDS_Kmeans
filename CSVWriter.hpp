#ifndef CSVWRITER_H
#define CSVWRITER_H

#include <vector>
#include <ostream>
#include <sstream>
#include <iomanip>

class CSVWriter {
public:
	CSVWriter(std::ostream &stream, char delimiter = ',')
		: stream_(stream)
		, delimiter_(delimiter) {}

	template<typename T>
	void write(const std::vector<T> &row) {
		bool first = true;
		std::stringstream ss;

		for (auto &e : row) {
			if (first)
				first = false;
			else
				ss << delimiter_;
			ss << e;
		}
		ss << std::endl;
		stream_ << ss.rdbuf();
	}

	void write(const std::vector<double> &row) {
		bool first = true;
		std::stringstream ss;
		ss << std::setprecision(10);

		for (auto &e : row) {
			if (first)
				first = false;
			else
				ss << delimiter_;
			ss << e;
		}

		ss << std::endl;
		stream_ << ss.rdbuf();
	}
private:
	std::ostream &stream_;
	char delimiter_;
};

#endif /* CSVWRITER_H */
