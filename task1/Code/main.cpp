#include "CImg.h"
#include <iostream>
#include <string>
#include <algorithm>
#include <math.h>
#include <chrono> 
#include <fstream>

using namespace std::chrono;
using namespace cimg_library;

double get_threshold(int x, int y, int w, double k, double R, CImg<unsigned char>& image){
	int left_x = std::max(0, x - w/2);
	int right_x = std::min(image.width(), x + w/2);

	int left_y = std::max(0, y - w/2);
	int right_y = std::min(image.height(), y + w/2);

	auto tmp = image.get_crop(left_x, left_y, right_x, right_y);

	auto m = tmp.mean();
	auto sd = sqrt(tmp.variance());

	double t = m * (1 + k * ((sd / R) - 1));
	return t;
}

unsigned char gb2gray(int x, int y, CImg<unsigned char>& image){
	unsigned char r = *image.data(x, y, 0);
	unsigned char g = *image.data(x, y, 1);
	unsigned char b = *image.data(x, y, 2);
    unsigned char gray = 0.2989 * r + 0.5870 * g + 0.1140 * b;

    return gray;
}	

double calc_speed(timespec start, timespec finish, CImg<unsigned char>& image){
	auto sec = finish.tv_sec - start.tv_sec;
	auto nsec = finish.tv_nsec - start.tv_nsec;

	std::cout << sec << "" << nsec << "\n";


	return static_cast<double>((sec * 1e9 + nsec))/(image.width()*image.height()*1000);
}

int main(int argc, char *argv[]){

	int w = 15;
	double k=0.2;
	double R = 255/2;

    if( argc != 2 ) {
        std::cout << "Error: invalid number of arguments\n";
        return 0;
    }

	std::ifstream file(argv[1]);
    std::string imagename;
    while( file >> imagename ) {
    	CImg<unsigned char> image = CImg<unsigned char>(imagename.c_str()); 
		CImg<unsigned char> result(image.width(), image.height()); 

		auto start = high_resolution_clock::now();

		for(int x = 0; x < image.width(); x++){
			for(int y = 0; y < image.height(); y++){
				double t = get_threshold(x, y, w, k, R, image);

				auto gray = gb2gray(x, y, image);
				if(gray > t){
					*result.data(x, y) = 255;
				}else{
					*result.data(x, y) = 0;
				}
			}
		}

		auto stop = high_resolution_clock::now(); 

		auto duration = duration_cast<nanoseconds>(stop - start);

		double speed = duration.count()/(image.width()*image.height());
		std::cout << speed << " nanosec/MP\n";

		std::string binname = imagename + "__bin.png";
		result.save_png(binname.c_str(), 1);
		std::cout << "Saved as " << binname << "\n";
    }
    return 0;
}