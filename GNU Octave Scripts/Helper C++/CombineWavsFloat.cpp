//g++64 -std=c++17 -o CombineWavsFloat -Wall -g CombineWavsFloat.cpp -lmingw32
//CombineWavs left.wav right.wav combined.wav

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
		std::cerr << "Parameter 1 must be left.wav." << std::endl;
		return 1;
	}
	char* leftFileTypeChar = &argv[1][strlen(argv[1])-4];
	if(std::strncmp(leftFileTypeChar,".wav",4)!=0){
		std::cerr << "Parameter 1 must be left.wav." << std::endl;
		return 1;
	}
	if(strlen(argv[2])<4){
		std::cerr << "Parameter 2 must be .wav." << std::endl;
		return 1;
	}
	char* rightFileTypeChar = &argv[2][strlen(argv[2])-4];
	if(std::strncmp(rightFileTypeChar,".wav",4)!=0){
		std::cerr << "Parameter 2 must be .wav." << std::endl;
		return 1;
	}
	if(strlen(argv[3])<4){
		std::cerr << "Parameter 3 must be .wav." << std::endl;
		return 1;
	}
	char* combinedFileTypeChar = &argv[3][strlen(argv[3])-4];
	if(std::strncmp(combinedFileTypeChar,".wav",4)!=0){
		std::cerr << "Parameter 3 must be .wav." << std::endl;
		return 1;
	}
	
	//Read in the metadata of the two wav files.
	drwav leftDrWav;
	if(!drwav_init_file(&leftDrWav, argv[1], NULL)) {
		std::cerr << "Problem reading left file." << std::endl;
		return 1;
	}
	drwav rightDrWav;
	if(!drwav_init_file(&rightDrWav, argv[2], NULL)) {
		std::cerr << "Problem reading left file." << std::endl;
		return 1;
	}
	
	//Get the formatting of the file.
	drwav_uint16 leftFormat = drwav_fmt_get_format(&leftDrWav.fmt);
	drwav_uint16 rightFormat = drwav_fmt_get_format(&rightDrWav.fmt);
	
	//Get the frame and sample counts of the file.
	unsigned int leftChannels = leftDrWav.channels;
	long long unsigned int leftFrameCount = leftDrWav.totalPCMFrameCount;
	long long unsigned int leftSampleCount = leftFrameCount * leftChannels;
	unsigned int rightChannels = rightDrWav.channels;
	long long unsigned int rightFrameCount = rightDrWav.totalPCMFrameCount;
	long long unsigned int rightSampleCount = rightFrameCount * rightChannels;
	
	//The files must be 16 bit pcm.
	if(leftDrWav.bitsPerSample != 32 || leftFormat != DR_WAVE_FORMAT_IEEE_FLOAT){
		std::cerr << "The left file must be 32 bit floating point." << std::endl;
		return 1;
	}
	if(rightDrWav.bitsPerSample != 32 || rightFormat != DR_WAVE_FORMAT_IEEE_FLOAT){
		std::cerr << "The right file must be 32 bit floating point." << std::endl;
		return 1;
	}
	
	//Make sure the file isn't corrupted or empty.
	if(leftSampleCount==0){
		std::cerr << "No samples read from left." << std::endl;
		return 1;
	}
	if(rightSampleCount==0){
		std::cerr << "No samples read from right." << std::endl;
		return 1;
	}
	
	//The files must have the same number of samples.
	if(leftSampleCount != rightSampleCount){
		std::cerr << "The files do not have the same number of samples." << std::endl;
		return 1;
	}
	
	//Read the wav files into RAM.
	float* leftFramesF32 = (float*)malloc(leftSampleCount * sizeof(float));
	drwav_read_pcm_frames_f32(&leftDrWav, leftFrameCount, leftFramesF32);
	float* rightFramesF32 = (float*)malloc(rightSampleCount * sizeof(float));
	drwav_read_pcm_frames_f32(&rightDrWav, rightFrameCount, rightFramesF32);
	
	//Combine the files.
	float* combinedFramesF32 = (float*)malloc(leftSampleCount * sizeof(float));
	for(unsigned int i = 0; i < leftFrameCount; i++){
		combinedFramesF32[i*2] = leftFramesF32[i*2];
		combinedFramesF32[i*2+1] = rightFramesF32[i*2+1];
	}
	
	//Create the output file.
	drwav_data_format format;
	format.container = drwav_container_riff;
	format.format = DR_WAVE_FORMAT_IEEE_FLOAT;
	format.channels = 2;
	format.sampleRate = 44100;
	format.bitsPerSample = 32;
	drwav wav;
	drwav_init_file_write(&wav, argv[3], &format, NULL);
	
	//Write the file.
	drwav_write_pcm_frames(&wav, leftFrameCount, combinedFramesF32);
	drwav_uninit(&wav);
	
	//Clean up.
	drwav_free(leftFramesF32, NULL);
	drwav_free(rightFramesF32, NULL);
	drwav_free(combinedFramesF32, NULL);
	return 0;
}