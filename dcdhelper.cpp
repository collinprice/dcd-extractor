#include "dcdhelper.h"

extern "C" {
#include "dcd.h"
}

#include <exception>
#include <iostream>

#define C_TEXT( text ) ((char*)std::string( text ).c_str())

DCDHelper::DCDHelper(std::string filename) {

	initialize_read_xyz(C_TEXT(filename));

	this->x = new float[my_H.N];
	this->y = new float[my_H.N];
	this->z = new float[my_H.N];
}

DCDHelper::~DCDHelper() {

	delete[] this->x;
	delete[] this->y;
	delete[] this->z;
}

int DCDHelper::numberOfFrames() {
	return my_H.NSet;
}

std::vector<PDBAtom> DCDHelper::getXYZAtFrame(int frame) {

	read_xyz(frame, x, y, z);
	std::vector<PDBAtom> points;
	for (int i = 0; i < my_H.N; ++i) {
		points.push_back(PDBAtom(i, x[i], y[i], z[i]));
	}

	return points;
}

void init(std::string filename) {

	initialize_read_xyz(C_TEXT(filename));
}

std::vector< std::vector<PDBAtom> > DCDHelper::getXYZs(std::string filename, int size) {

	init(filename);
	
	float* x = new float[my_H.N];
	float* y = new float[my_H.N];
	float* z = new float[my_H.N];

	if (size == -1) {
		size = my_H.NSet;
	}

	if (size < 1) {
		throw "Size must be greater than zero.";
	} else if (size > my_H.NSet) {
		throw "DCD File does not have enough frames for requested size.";
	}

	double percent = (double)size / my_H.NSet;

	int step = my_H.NSet / (my_H.NSet * percent);
	std::vector< std::vector<PDBAtom> > molecules;
	int counter = 0;
	for (int j = 0; j < my_H.NSet; j += step) {
		++counter;

		read_xyz(j, x, y, z);
		std::vector<PDBAtom> points;
		for (int i = 0; i < my_H.N; ++i) {
			points.push_back(PDBAtom(i, x[i], y[i], z[i]));
		}
		molecules.push_back(points);
	}
	
	delete[] x;
	delete[] y;
	delete[] z;

	return molecules;
}

std::vector< std::vector<PDBAtom> > DCDHelper::getXYZs(std::string filename) {

	return DCDHelper::getXYZs(filename, -1);
}

std::vector< std::vector<PDBAtom> > DCDHelper::getXYZsByIndex(std::string filename, std::vector<int> indexes) {

	init(filename);
	
	float* x = new float[my_H.N];
	float* y = new float[my_H.N];
	float* z = new float[my_H.N];

	std::vector< std::vector<PDBAtom> > molecules;
	for (std::vector<int>::iterator i = indexes.begin(); i != indexes.end(); ++i) {
		
		read_xyz(*i, x, y, z);
		std::vector<PDBAtom> points;
		for (int j = 0; j < my_H.N; ++j) {
			points.push_back(PDBAtom(j, x[j], y[j], z[j]));
		}
		molecules.push_back(points);
	}
	
	delete[] x;
	delete[] y;
	delete[] z;

	return molecules;
}