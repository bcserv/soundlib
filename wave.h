// by Evan X. Merz
// www.thisisnotalabel.com

// Example Wav file input and output
// this was written for educational purposes, but feel free to use it for anything you like 
// as long as you credit me appropriately ("wav IO based on code by Evan Merz")

// if you catch any bugs in this, or improve upon it significantly, send me the changes
// at evan at thisisnotalabel dot com, so we can share your changes with the world

#include <fstream>
#include <iostream>
#include <string>
using namespace std;



class WavFileForIO
{
/*
     WAV File Specification
     FROM http://ccrma.stanford.edu/courses/422/projects/WaveFormat/
    The canonical WAVE format starts with the RIFF header:
    0         4   ChunkID          Contains the letters "RIFF" in ASCII form
                                   (0x52494646 big-endian form).
    4         4   ChunkSize        36 + SubChunk2Size, or more precisely:
                                   4 + (8 + SubChunk1Size) + (8 + SubChunk2Size)
                                   This is the size of the rest of the chunk 
                                   following this number.  This is the size of the 
                                   entire file in bytes minus 8 bytes for the
                                   two fields not included in this count:
                                   ChunkID and ChunkSize.
    8         4   Format           Contains the letters "WAVE"
                                   (0x57415645 big-endian form).

    The "WAVE" format consists of two subchunks: "fmt " and "data":
    The "fmt " subchunk describes the sound data's format:
    12        4   Subchunk1ID      Contains the letters "fmt "
                                   (0x666d7420 big-endian form).
    16        4   Subchunk1Size    16 for PCM.  This is the size of the
                                   rest of the Subchunk which follows this number.
    20        2   AudioFormat      PCM = 1 (i.e. Linear quantization)
                                   Values other than 1 indicate some 
                                   form of compression.
    22        2   NumChannels      Mono = 1, Stereo = 2, etc.
    24        4   SampleRate       8000, 44100, etc.
    28        4   ByteRate         == SampleRate * NumChannels * BitsPerSample/8
    32        2   BlockAlign       == NumChannels * BitsPerSample/8
                                   The number of bytes for one sample including
                                   all channels. I wonder what happens when
                                   this number isn't an integer?
    34        2   BitsPerSample    8 bits = 8, 16 bits = 16, etc.

    The "data" subchunk contains the size of the data and the actual sound:
    36        4   Subchunk2ID      Contains the letters "data"
                                   (0x64617461 big-endian form).
    40        4   Subchunk2Size    == NumSamples * NumChannels * BitsPerSample/8
                                   This is the number of bytes in the data.
                                   You can also think of this as the size
                                   of the read of the subchunk following this 
                                   number.
    44        *   Data             The actual sound data.
*/


    	private:
		char* 	myPath;
		int 	myChunkSize;
		int	mySubChunk1Size;
    		short 	myFormat;
		short 	myChannels;
		int   	mySampleRate;
		int   	myByteRate;
		short 	myBlockAlign;
		short 	myBitsPerSample;
		int	myDataSize;

		int dataChunkSize;
		
	public:
		// I made this public so that you can toss whatever you want in here
		// maybe a recorded buffer, maybe just whatever you want
		char* 	myData;

		// get/set for the Path property
		char* getPath()
		{
			return myPath;
		}
		void setPath(char* newPath)
		{
			myPath = new char[200];
			strcpy(myPath, newPath);
		}

		~WavFileForIO()
		{
			delete myPath;
			myChunkSize = NULL;
			mySubChunk1Size = NULL;
	    		myFormat = NULL;
			myChannels = NULL;
			mySampleRate = NULL;
			myByteRate = NULL;
			myBlockAlign = NULL;
			myBitsPerSample = NULL;
			myDataSize = NULL;
		}

	// empty constructor
	WavFileForIO()
        {
		myPath = new char[200];
        }

	// constructor takes a wav path
	WavFileForIO(char* tmpPath)
        {
		myPath = new char[200];
		strcpy(myPath, tmpPath);
		read();
        }

	// read a wav file into this class
	bool read()
	{
		ifstream inFile( myPath, ios::in | ios::binary);

		if (!inFile.is_open()) {
			return false;
		}

		//printf("Reading wav file...\n"); // for debugging only

		inFile.seekg(4, ios::beg);
		inFile.read( (char*) &myChunkSize, 4 ); // read the ChunkSize

		inFile.seekg(16, ios::beg);
		inFile.read( (char*) &mySubChunk1Size, 4 ); // read the SubChunk1Size

		//inFile.seekg(20, ios::beg);
		inFile.read( (char*) &myFormat, sizeof(short) ); // read the file format.  This should be 1 for PCM

		//inFile.seekg(22, ios::beg);
		inFile.read( (char*) &myChannels, sizeof(short) ); // read the # of channels (1 or 2)

		//inFile.seekg(24, ios::beg);
		inFile.read( (char*) &mySampleRate, sizeof(int) ); // read the samplerate

		//inFile.seekg(28, ios::beg);
		inFile.read( (char*) &myByteRate, sizeof(int) ); // read the byterate

		//inFile.seekg(32, ios::beg);
		inFile.read( (char*) &myBlockAlign, sizeof(short) ); // read the blockalign

		//inFile.seekg(34, ios::beg);
		inFile.read( (char*) &myBitsPerSample, sizeof(short) ); // read the bitspersample

		inFile.seekg(40, ios::beg);
		inFile.read( (char*) &myDataSize, sizeof(int) ); // read the size of the data


		// read the data chunk
		myData = new char[4096];
		inFile.seekg(44, ios::beg);

		dataChunkSize = 0;

		while (!inFile.eof()) {
			inFile.read(myData, 4096);
			dataChunkSize += inFile.gcount(); 
		}

		inFile.close(); // close the input file

		return true; // this should probably be something more descriptive
	}

	float getSongDuration() {
		return (float)dataChunkSize/(float)myByteRate;
	}

	int getBitRate() {
		return (int)((myByteRate*8)/1024)+4;
	}

	int getSamplingRate() {
		return mySampleRate;
	}

	// return a printable summary of the wav file
	char *getSummary()
	{
		char *summary = new char[250];
		sprintf(summary, " Format: %d\n Channels: %d\n SampleRate: %d\n ByteRate: %d\n BlockAlign: %d\n BitsPerSample: %d\n DataSize: %d\n DataChunkSize: %d\n Song-length: %f\n", myFormat, myChannels, mySampleRate, myByteRate, myBlockAlign, myBitsPerSample, myDataSize, dataChunkSize, getSongDuration());
		return summary;
	}
};
