#include "wav.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int check_format(WAVHEADER header);
int get_block_size(WAVHEADER header);

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Incorrect Syntax. Use: ./reverse <inputfile> <nameofnewfile>");
        return 1;
    }
    // Ensure proper usage
    // TODO #1

    // Open input file for reading
    // TODO #2
    FILE *input = fopen(argv[1], "r");
    if (input == NULL)
    {
        printf("not enough memory to open input file.");
        return 1;
    }
    // Read header
    // make buffer
    WAVHEADER inputheader;
    fread(&inputheader, 44, sizeof(BYTE), input);

    // TODO #3

    // Use check_format to ensure WAV format
    // TODO #4
    if (check_format(inputheader) == 1)
    {
        printf("incompatible file type\n");
        return 2;
    }
    // Open output file for writing
    // TODO #5
    FILE *output = fopen(argv[2], "w");
    if (output == NULL)
    {
        printf("not enough memory to write output file.");
        return 1;
    }
    // Write header to file
    // TODO #6
    fwrite(&inputheader, 44, sizeof(BYTE), output);

    // Use get_block_size to calculate size of block
    // TODO #7
    int blocksize = get_block_size(inputheader);
    int bytecount = inputheader.subchunk2Size;
    // subchunk2size is returning 300 gigabytes worth of data. checking -> i used the wrong converter, its all cool. 
    printf("%i\n", inputheader.subchunk2Size);
    // Write reversed audio to file
    // TODO #8
    BYTE data[bytecount];
    BYTE middleman;
    int halflen = round(bytecount / 2); //round
    // this works because of indexing, i.e 7 halves to 3.5, but because its while less than in the for loop ->
    int halfblock = round(blocksize / 2); //round **************
    // -> i.e i < 4 means iterate 3 times (i0,i1,i2) which would swap the first 3 with the last 3 and leave the middle untouched.
    // I could have just done + 1 and not included math.h, because by default ints round down. and we are only dealing with whole numbers.
    fread(&data, bytecount, sizeof(BYTE), input);
    // get all the bytes in beginning block,
    // get all bytes in end block
    printf("%i\n", blocksize);
    // here b is the beginning 'scan' and e is the end 'scan'
    // our parent loop iterates in blocksize amount (may need to be blocksize - 1 for indexing)
    // then within we get get all the block values, assuming that the first block starts at the beginning of DATA
    // and the last block ends at the end of DATA
    // the parent loop terminates when we've reached the halfway point, at which point we would be reswapping values.
    // as of 1:08 this loop is causing a segfault.
    // problem was i defined my data and middleman structs as dword because i went off the diagram on the web page linked in the
    // spec. instead i should have used common sense, knowing that I'm working in bytes.

    for (int b = 0, e = bytecount; b < halflen; b += blocksize, e -= blocksize)
    {
        for (int j = 0; j < blocksize; j++)
        {
            middleman = data[b + j];
            data[b + j] = data[e - blocksize + j];
            data[e - blocksize + j] = middleman;
            // b is parent low
            // e is parent high
            // j is block counter
            // if we imagine instead of a mirror image, imagine simply moving the block to the other side, we can see that + 4 = + 4
            // +3 = +3 etc we do data[b + j] where b is the start of each block and j iterates through every byte of the block going
            // forward and we do e (the end point) minus the blocksize (go to beginning of block) + j to match.

            // this is because we want the blocks to be reversed but we want the data within the blocks to be the same.
        }
    }

    fwrite(&data, bytecount, sizeof(BYTE), output);
    fclose(input);
    fclose(output);
}

int check_format(WAVHEADER header)
{
    // TODO #4
    char *wave = "WAVE";
    int match = 0;
    for (int i = 0; i < 4; i++)
    {
        if (header.format[i] != wave[i])
        {
            return 1;
            break;
        }
    }
    return 0;
}

int get_block_size(WAVHEADER header)
{
    // TODO #7
    // channels*bytespersample*subchunk2size
    int size = (header.numChannels) * (header.bitsPerSample / 8);
    return size;
}

// okay so its 23:06 and my code is outputting a whitenoise fart followed by silence.
// either: my header file is incorrect
// or my data read is incorrect
// or my replacement algorithm is incorrect.
// those most likely is my data read, but because algo is easy to check I'll do that first.
// okay i checked it and it was my algorithm. lets get it.

// its a 1d array. its basically a list of integers.
// might it be that i dont need to swap each byte, but actually need to move each group of bytes.
// i.e just because its easy to to fread(numofbytes, sizeofbyte) doesnt mean that the data is computated on a byte by byte basis. it
// might be the relationship between multiple.

// problem found. for get block size, i thought that we were trying to get the length of the array.
// in my defence it was a pretty reasonable assumption to make given the implementation guide.