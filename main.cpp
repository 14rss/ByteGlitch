////////////////////////////////////////////////////////////////////
//   ______   _______ _____     ____ _     ___ _____ ____ _   _   //
//  | __ ) \ / /_   _| ____|   / ___| |   |_ _|_   _/ ___| | | |  //
//  |  _ \\ V /  | | |  _|    | |  _| |    | |  | || |   | |_| |  //
//  | |_) || |   | | | |___   | |_| | |___ | |  | || |___|  _  |  //
//  |____/ |_|   |_| |_____|___\____|_____|___| |_| \____|_| |_|  //
//                        |_____|                                 //
////////////////////////////////////////////////////////////////////

//Author: Christan Clark (AKA DeepDarkDweller)
//Date: 04/08/2023
//Purpose: Glitches up files by randomly changing some bits and bytes while a file is copied
//NOTES: 
//Inspired by some weird glitchy images I got when my ipod crashed during a file transfer
//Don't judge my code too harshly. This is very much WIP, wrote it for fun on a saturday evening.

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include<sys/types.h>
#include<sys/stat.h>
#include <fcntl.h>  
#include <thread>
#include <mutex>
#include <semaphore.h>
#include <vector>
#include<random>
#include <time.h>

using namespace std;

void hexPrintBuff(char* buffer, unsigned int buffSize, unsigned int columns = 16);
void debugHeaderPrint(string outString);
int writeBackButGlitched(string IFName, string OFName);
void glitchAlgo(char* cleanBuffer, char* glitchBuffer, int buffSize);

unsigned long totalBytesRead;
unsigned long totalChunksRead;
int numGlitches;

int main(void)
{
    cout << "Initializing..." << endl;

    totalBytesRead = 0;
    totalChunksRead = 0;
    numGlitches = 0;

    srand(time(0));
    string filename = "";
    string filename_glitched = "";
    string outputFormat = "";
    
    cout << "Input the name of a file to glitch (ex: \"doge.png\"): ";
    cin >> filename;
    cout << "Input the desired format of output file (ex: \".png\"): ";
    cin >> outputFormat;

    while(1){
        cout << "Running Byte_Glitch..." << endl;
        string of = "glitched" + outputFormat;
        writeBackButGlitched(filename, of);

        break;//TODO: make main loop generate lots of images so I can make lots of files automatically and just pick my faves
    }
    return 0;
}

//lets corrupt some files!
int writeBackButGlitched(string IFName, string OFName){
    std::ofstream outfile(OFName,std::ofstream::binary);
    std::ifstream is(IFName, std::ifstream::binary);

    totalBytesRead = 0;
    totalChunksRead = 0;

    // allocate memory for file content
    int chunkSize = 4096; //The size of the chunks can affect the glitch result since the glitch algorithm is just looping over chunks
    char* cleanBuffer = new char [chunkSize];
    char* glitchBuffer = new char[chunkSize];

    if (is) 
    {   
        // get length of file
        is.seekg (0, is.end);
        unsigned int length = is.tellg();
        is.seekg (0, is.beg);

        std::cout << "Reading " << length << " bytes... ";

        if(is.good()) 
        {
            // read data in chunks:
            while(1) 
            {
                is.read(cleanBuffer, chunkSize);
                std::streamsize dataSize = is.gcount();

                glitchAlgo(cleanBuffer, glitchBuffer, chunkSize);

                outfile.write(glitchBuffer, chunkSize); // write to outfile

                /*
                hexPrintBuff(cleanBuffer, buffLen); //write bytes to console
                
                if (is)
                    std::cout << "\nAll chars in chunk read successfully!" << endl;
                else
                    std::cout << "\nError: only " << is.gcount() << " chars of Chunk Size: "<< buffLen << " could be read!" << endl;
                */

                cout << "Bytes Read: " << is.gcount() << "/" << chunkSize << "\n" << endl;
                totalChunksRead += 1;
                totalBytesRead += is.gcount();

                if(!is) break; //break if problem of EOF
            }
        }
        std::cout << "Total Chunks: " << totalChunksRead << endl;
        std::cout << "Total Bytes Read: " << totalBytesRead << endl;
        
        is.close();
    }
    else
    {
        std::cout << "\nERROR: CANNOT READ FILE!\n" << endl;
        return 1;
    }

    // release dynamically-allocated memory
    delete[] glitchBuffer;
    delete[] cleanBuffer;
    outfile.close();
    return 0;
}

//function where the glitches happen, just do whatever you want while copying the bytes from cleanBuffer to glitchBuffer
void glitchAlgo(char* cleanBuffer, char* glitchBuffer, int buffSize)
{
    for(int i = 0; i < buffSize; i++){
        if((rand() % 100 ) >= 99 && numGlitches < 5){
            numGlitches +=1; //keep track of how many glitches
            glitchBuffer[i] = cleanBuffer[(rand() % buffSize)] | 0x05; //make a big mistake on purpose
        }else{ 
            glitchBuffer[i] = cleanBuffer[i];
        }   
    }
    //numGlitches = 0; //reset for each chunk for maximum chaos and lots of unreadable files
}

//print the buffer bytes as hex digits
void hexPrintBuff(char* buffer, unsigned int buffSize, unsigned int columns)
{
    unsigned char *u = (unsigned char *) buffer;
    for (unsigned int i=0; i < buffSize; ++i) {
        if((i%columns == 0) && i)
            printf("\n");
        printf("%02x ", u[i]);
    }
}

void debugHeaderPrint(string outString)
{
    std::cout << "------------------------\n" << outString << "\n------------------------" << endl;
}