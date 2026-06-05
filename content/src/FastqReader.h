#ifndef FASTQREADER
#define FASTQREADER

#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <vector>
#include <zlib.h> // Requis pour la lecture des fichiers .gz


using namespace std;
class FastqReader {
private:
    gzFile gz_file = nullptr;
    ifstream std_file;
    bool is_gzipped = false;

    // Buffer interne pour la lecture par blocs (.gz)
    static const size_t BUFFER_SIZE = 131072; // 128 Ko
    vector<char> buffer;
    size_t buffer_pos = 0;
    size_t buffer_size = 0;

public:
    FastqReader(const string& filepath) : buffer(BUFFER_SIZE) {
        if (filepath.size() >= 3 && filepath.substr(filepath.size() - 3) == ".gz") {
            is_gzipped = true;
            gz_file = gzopen(filepath.c_str(), "rb");
            // Optionnel mais recommandé : augmente le buffer interne de zlib
            gzbuffer(gz_file, BUFFER_SIZE);
        } else {
            is_gzipped = false;
            std_file.open(filepath);
        }
    }

    ~FastqReader() {
        if (is_gzipped && gz_file) gzclose(gz_file);
    }

    bool is_open() {
        return is_gzipped ? (gz_file != nullptr) : std_file.is_open();
    }

    // Version optimisée qui extrait les données depuis un buffer en mémoire
    bool get_line(string& line) {
        line.clear();
        if (!is_gzipped) {
            return (bool)getline(std_file, line);
        }

        while (true) {
            // Si le buffer interne est vide, on le remplit à nouveau depuis le fichier
            if (buffer_pos >= buffer_size) {
                int bytes_read = gzread(gz_file, buffer.data(), BUFFER_SIZE);
                if (bytes_read <= 0) {
                    return !line.empty(); // Fin du fichier
                }
                buffer_size = bytes_read;
                buffer_pos = 0;
            }

            // On cherche le prochain saut de ligne dans notre buffer en mémoire
            char c = buffer[buffer_pos++];
            if (c == '\r') continue; // Gestion des fins de ligne Windows optionnelle
            if (c == '\n') return true;
            
            line.push_back(c);
        }
    }
};
#endif