CXX = g++ # Compiler
CXXFLAGS = -std=c++11 -O3 -Wall # Compiler flags
CXXLIB = -lz

install: build
	echo "Installing python requirements & installing the content module"
	pip install -r requirements.txt
	pip install -e .

build: content/src/*.cpp content/src/*.h
	echo "Building the fastq parser"
	mkdir -p content/build/
	$(CXX) $(CXXFLAGS) content/src/*.cpp content/src/*.h -o content/build/fastq_processor $(CXXLIB)
