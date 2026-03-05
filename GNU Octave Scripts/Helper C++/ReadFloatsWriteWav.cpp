//g++64 -std=c++17 -o ReadFloatsWriteWav -Wall -g ReadFloatsWriteWav.cpp -lmingw32
//ReadFloatsWriteWav in.txt out.wav frameCount
//ReadFloatsWriteWav test.txt out.wav 100

#include <cmath>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>

#define DR_WAV_IMPLEMENTATION
#include "dr_wav.h"

int main(int argc,char* argv[]){
	//Open the input file.
	std::ifstream ifs (argv[1], std::ifstream::in);
	//Create a string stream.
	std::stringstream ss;
	if(ifs){
		ss << ifs.rdbuf();
		ifs.close();
	}
	//Create a space for the read-in floating point numbers.
	int frameCount = std::stoi(argv[3]);
	int sampleCount = frameCount * 2;
	float* samples = (float*)malloc(sampleCount * sizeof(float));
	//Read in the numbers.
	std::string s;
	int i = 0;
	bool validFloat;
	int oorCounter = 0;
	while(ss >> s){
		float f;
		validFloat = true;
		try{
			f = std::stof(s);
		}catch(const std::out_of_range& oor){
			f = -1.0;
			oorCounter++;
			std::cout << "Number is not representable as a float. #" << oorCounter << std::endl;
		}catch(const std::invalid_argument& ia){
			validFloat = false;
			//std::cerr << "The section of text: " << s << "is not recognized as a float value." << std::endl;
			//std::cerr << "Invalid argument: " << ia.what() << std::endl;
		}
		samples[i*2] = f;
		samples[i*2+1] = f;
		if(validFloat) i++;
	}
	//Create the output file.
	drwav_data_format format;
	format.container = drwav_container_riff;
	format.format = DR_WAVE_FORMAT_IEEE_FLOAT;
	format.channels = 2;
	format.sampleRate = 44100;
	format.bitsPerSample = 32;
	drwav wav;
	drwav_init_file_write(&wav, argv[2], &format, NULL);
	//Write the file.
	drwav_write_pcm_frames(&wav, frameCount, samples);
	drwav_uninit(&wav);
	return 0;
}