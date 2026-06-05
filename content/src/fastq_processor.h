#ifndef FASTQPROC
#define FASTQPROC


#include <string>
#include <cmath>
#include "FastqReader.h"

class FastqProcessor {
public:
    FastqProcessor(const string& inputFilePath, const string& outputFilePath)
        : inputFilePath(inputFilePath), outputFilePath(outputFilePath) {}

    void process_fastq_file() {
        //  Open the input file
        FastqReader input_file(inputFilePath);
        if (!input_file.is_open()) {
            cerr << "Error: unable to open the input file" << inputFilePath << endl;
            return;
        }

        //  Open the output file
        ofstream output_file(outputFilePath);
        if (!output_file.is_open()) {
            cerr << "Error: unable to open the output file" << endl;
            return;
        }

        //  Add the columns names in the output file (first line)
        output_file << "read_name\tread_length\tread_avg_quality" << endl;

        //  Init the vars were the data will be stored.
        string sequence, header, quality, line;
        unsigned short int line_count = 0;
        unsigned long int seq_len = 0;
        bool all_quality_content=true;


        double tab_quality[129];
        for (int q = 0; q < 129 ; q ++) {
            tab_quality[q] = pow((double)10, -((double)q) / 10.0);
        }

        //  Read the input file line per line
        while (input_file.get_line(line)) {
            if (line[0] == '@' && all_quality_content) {  // header lines
                if (!sequence.empty()) {
                    double avg_quality = 0.0;
                    for (uint i = 0; i < quality.length(); i++) {
                        avg_quality += tab_quality[(int)quality[i] - 33];
                    }
                    avg_quality /= (double)quality.length();
                    output_file << header.substr(1) << "\t" << sequence.length() << "\t" << -10 * log10(avg_quality) << endl;
                    sequence.clear();
                    quality.clear();
                }
                size_t space_pos = line.find(' ');
                header = (space_pos != string::npos) ? line.substr(1, space_pos - 1) : line.substr(1);
                line_count = 1;
                all_quality_content = false;
            } else if (line_count == 1) {  // Sequence
                if (line == (string)"+") { 
                    line_count = 3;
                    seq_len = sequence.length();
                    continue;
                }
                sequence += line;
            } else if (line_count == 3) {  // Quality
                quality += line;
                if (quality.length() == seq_len) { all_quality_content=true; }
                else if (quality.length() > seq_len) {
                    cerr << "Error : The Sequence and Quality lengths mismatch for read " << header << endl;
                    exit(EXIT_FAILURE);
                }
            }
        }

        //  Compute the mean Phred quality Afficher le Phred quality score moyen de la dernière séquence du fichier fastq
        if (!sequence.empty()) {
            double avg_quality = 0.0;
            for (uint i = 0; i < quality.length(); i++) {
                avg_quality += tab_quality[(int)quality[i] - 33];
            }
            avg_quality /= (double)quality.length();
            output_file << header.substr(1) << "\t" << sequence.length() << "\t" << -10 * log10(avg_quality) << endl;
        }

        // Fermer les fichiers
        output_file.close();
    }

private:
    string inputFilePath;
    string outputFilePath;
};


#endif


