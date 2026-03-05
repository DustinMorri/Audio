//g++64 -std=c++17 -o ReadWavWriteFloats -Wall -g ReadWavWriteFloats.cpp -lmingw32
//ReadWavWriteFloats in.wav out.txt channel
//ReadWavWriteFloats in.wav out.txt 0
//ReadWavWriteFloats "Car Sound Test Driver.wav" out.txt 0

#include <cmath>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>

#define DR_WAV_IMPLEMENTATION
#include "dr_wav.h"

int main(int argc,char* argv[]){
	//Make sure the file names are correct.
	if(strlen(argv[1])<4){
		std::cerr << "Input file must be .wav." << std::endl;
		return 1;
	}
	if(strlen(argv[2])<4){
		std::cerr << "Output file must be .txt." << std::endl;
		return 1;
	}
	char* inputFileTypeChar = &argv[1][strlen(argv[1])-4];
	if(std::strncmp(inputFileTypeChar,".wav",4)!=0){
		std::cerr << "Input file must be .wav." << std::endl;
		return 1;
	}
	char* outputFileTypeChar = &argv[2][strlen(argv[2])-4];
	if(std::strncmp(outputFileTypeChar,".txt",4)!=0){
		std::cerr << "Output file must be .txt." << std::endl;
		return 1;
	}
	
	//Get the channel the user wants to read.
	int channel = std::stoi(argv[3]);
	
	//Read in the metadata of the wav file.
	drwav inputDrWav;
	if(!drwav_init_file(&inputDrWav, argv[1], NULL)) {
		std::cerr << "Problem reading input file." << std::endl;
		return 1;
	}
	
	//Get the formatting of the file.
	drwav_uint16 inputFormat = drwav_fmt_get_format(&inputDrWav.fmt);
	
	//Get the frame and sample counts of the file.
	unsigned int inputChannels = inputDrWav.channels;
	long long unsigned int inputFrameCount = inputDrWav.totalPCMFrameCount;
	long long unsigned int inputSampleCount = inputFrameCount * inputChannels;
	
	//The files must be 16 bit pcm.
	if(inputDrWav.bitsPerSample != 16 || inputFormat != DR_WAVE_FORMAT_PCM){
		std::cerr << "The input file must be 16 bit pcm." << std::endl;
		return 1;
	}
	
	//Make sure the file isn't corrupted or empty.
	if(inputSampleCount==0){
		std::cerr << "No samples read from input." << std::endl;
		return 1;
	}
	
	//Read the wav file into RAM and convert the signed integers to floating point arithmetic.
	drwav_int16* inputFramesI16 = (drwav_int16*)malloc(inputSampleCount * sizeof(drwav_int16));
	drwav_read_pcm_frames_s16(&inputDrWav, inputFrameCount, inputFramesI16);
	float* inputFramesF32 = (float*)malloc(inputSampleCount * sizeof(float));
	drwav_s16_to_f32(inputFramesF32, inputFramesI16, inputSampleCount);
	drwav_free(inputFramesI16, NULL);
	
	//Open the output file.
	std::ofstream outFile;
	outFile.open(argv[2]);
	for(long long unsigned int i = 0; i < inputFrameCount; i++){
		outFile << inputFramesF32[i*inputChannels+channel] << "\t";
	}
	outFile.close();
	return 0;
}