#include "fastq_processor.h"


int main(int argc, char* argv[]) {
    //  Check if the two filenames have been provided
    if (argc < 3) {
        cerr << "Missing input and/or output file path(s)" << endl;
        return 1;
    }

    string inputFilePath = argv[1]; // Input Fastq file path
    string outputFilePath = argv[2]; // Output text file path

    FastqProcessor processor(inputFilePath, outputFilePath);
    processor.process_fastq_file();

    cout << "Fastq processing complete. Results saved in " << outputFilePath << "." << endl;

    return 0;
}