#include "dcdhelper.h"

#include <fstream>
#include <iostream>
#include <string>
#include <unistd.h>
#include <algorithm>
#include <dirent.h>
#include <errno.h>
#include <sstream>
#include <iomanip>
#include <sys/stat.h>

double unifRand() {
	return rand() / double(RAND_MAX);
}

std::vector<double> readDCDExafsScoreFile(std::string filename) {

	std::vector<double> indexes;
	std::ifstream index_file(filename.c_str());
	if (index_file.is_open() && index_file.good()) {

		std::string score_str;
		while(index_file.good()) {

			index_file >> score_str;
			double score = atof(score_str.c_str());

			indexes.push_back(score);
		}
	}

	return indexes;
}

std::vector<int> dcdGetIndexLessThan(std::string filename, double limit) {

	std::vector<int> indexes;
	std::ifstream index_file(filename.c_str());
	if (index_file.is_open() && index_file.good()) {

		std::string score_str;
		int i = 0;
		while(index_file.good()) {

			index_file >> score_str;
			double score = atof(score_str.c_str());

			if (score < limit) {
				indexes.push_back(i);
			}
			
			++i;
		}
	}

	return indexes;
}

int main(int argc, char **argv) {
	
	int c;
	std::string count_s, dcd_file_s, output_directory_s, exafs_score_file_s;

	while ( (c = getopt(argc, argv, "c:f:o:s:")) != -1 ) {

		switch(c) {
			case 'c':
				count_s.assign(optarg);
				break;
			case 'f':
				dcd_file_s.assign(optarg);
				break;
			case 'o':
				output_directory_s.assign(optarg);
				break;
			case 's':
				exafs_score_file_s.assign(optarg);
				break;
			default:
				exit(EXIT_FAILURE);
		}
	}

	if (count_s.size() == 0) {
		std::cout << "Frame count required. Use -c <count>." << std::endl;
		exit(EXIT_FAILURE);
	}

	if (dcd_file_s.size() == 0) {
		std::cout << "DCD Output file required. Use -f <file>." << std::endl;
		exit(EXIT_FAILURE);
	}

	if (output_directory_s.size() == 0) {
		std::cout << "Output directory required. Use -o <file>." << std::endl;
		exit(EXIT_FAILURE);
	}

	if (exafs_score_file_s.size() == 0) {
		std::cout << "EXAFS Score file required. Use -s <file>." << std::endl;
		exit(EXIT_FAILURE);
	}

	// Read in EXAFS score file.
	std::vector<double> dcdExafsScores = readDCDExafsScoreFile(exafs_score_file_s);
	std::sort(dcdExafsScores.begin(), dcdExafsScores.end());

	int frameCount = atoi(count_s.c_str());

	if (frameCount > (int)dcdExafsScores.size()) {
		std::cout << "Frame count is greater than the number of available frames." << std::endl;
		exit(EXIT_FAILURE);
	}

	double maxExafsScore = dcdExafsScores.at(frameCount);
	std::vector<int> frameIndexes = dcdGetIndexLessThan(exafs_score_file_s, maxExafsScore);

	mkdir(output_directory_s.c_str(), 0755);

	std::ostringstream oss;
	DCDHelper dcd_helper = DCDHelper(dcd_file_s);
	for (int i = 0; i < (int)frameIndexes.size(); ++i) {
		
		std::vector<PDBAtom> frame = dcd_helper.getXYZAtFrame(frameIndexes.at(i));

		oss << output_directory_s << "/" << (i+1) << ".xyz";
		std::ofstream xyz_file(oss.str().c_str());

		// Number of atoms
		xyz_file << frame.size() << std::endl;

		// Comment line
		xyz_file << std::endl;

		for (std::vector<PDBAtom>::iterator atom = frame.begin(); atom != frame.end(); ++atom) {
			xyz_file << std::setprecision(15) << "H " << atom->x << " " << atom->y << " " << atom->z << std::endl;
		}

		xyz_file.close();

		oss.clear();
		oss.str("");
	}

	return 0;
}
