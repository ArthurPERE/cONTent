#ifndef FASTQPROC
#define FASTQPROC

#include <string>
#include <cmath>
#include "FastqReader.h"

class FastqProcessor {
public:
    FastqProcessor(const string& inputFilePath, const string& outputFilePath)
        : inputFilePath(inputFilePath), outputFilePath(outputFilePath), input_file(inputFilePath), output_file(outputFilePath) {
            for (int q = 0; q < 129 ; q ++) {
                tab_quality[q] = pow((double)10, -((double)q) / 10.0);
            }
            
            if (!input_file.is_open()) {
                cerr << "Error: unable to open the input file" << inputFilePath << endl;
                return;
            }
            
            //  Open the output file
            if (!output_file.is_open()) {
                cerr << "Error: unable to open the output file" << endl;
                return;
            }
            
        }
        
        ~FastqProcessor() {
            // Fermer les fichiers
            output_file.close();
        }
        
        void process_fastq_file() {
            
            //  Add the columns names in the output file (first line)
            output_file << "read_name\tread_length\tread_avg_quality" << endl;
            
            //  Init the vars were the data will be stored.
            string sequence, header, quality, line;
            unsigned short int line_count = 0;
            unsigned long int seq_len = 0;
            bool all_quality_content=true;

            //  Read the input file line per line
            while (input_file.get_line(line)) {
                if (line[0] == '@' && all_quality_content) {  // header lines
                    if (!sequence.empty()) {
                        write_line(header, sequence, quality);
                        sequence.clear();
                        quality.clear();
                    }
                    size_t space_pos = line.find_first_of(" \t");
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
                write_line(header, sequence, quality);
            }
        }

private:
    string inputFilePath;
    string outputFilePath;
    FastqReader input_file;
    ofstream output_file;

    double tab_quality[129];

    double calculate_avg_qual(const string& quality) {
        double avg_quality = 0.0;
        for (uint i = 0; i < quality.length(); i++) {
            avg_quality += tab_quality[(int)quality[i] - 33];
        }
        avg_quality /= (double)quality.length();
        return avg_quality;
    }

    void write_line(const string& header, const string& sequence, const string& quality) {
        double avg_quality = calculate_avg_qual(quality);
        output_file << header.substr(1) << "\t" << sequence.length() << "\t" << -10 * log10(avg_quality) << endl;
    }
};


#endif


