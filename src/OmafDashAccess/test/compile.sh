#!/bin/bash -e

cp ../../google_test/libgtest.a .

g++ -I../../google_test -std=c++11 -I../util/ -g  -c testMediaSource.cpp -D_GLIBCXX_USE_CXX11_ABI=0
g++ -I../../google_test -std=c++11 -I../util/ -g  -c testMPDParser.cpp -D_GLIBCXX_USE_CXX11_ABI=0
g++ -I../../google_test -std=c++11 -I../util/ -g  -c testOmafReader.cpp -D_GLIBCXX_USE_CXX11_ABI=0
g++ -I../../google_test -std=c++11 -I../util/ -g  -c testOmafReaderManager.cpp -D_GLIBCXX_USE_CXX11_ABI=0

LD_FLAGS="-I/usr/local/include/ -lcurl -lstdc++ -lOmafDashAccess -lpthread -lglog -l360SCVP -lm -L/usr/local/lib"
g++ -L/usr/local/lib testMediaSource.o testMPDParser.o testOmafReader.o testOmafReaderManager.o libgtest.a -o testLib ${LD_FLAGS}
g++ -L/usr/local/lib testMediaSource.o libgtest.a -o testMediaSource ${LD_FLAGS}
g++ -L/usr/local/lib testMPDParser.o libgtest.a -o testMPDParser ${LD_FLAGS}
g++ -L/usr/local/lib testOmafReader.o libgtest.a -o testOmafReader ${LD_FLAGS}
g++ -L/usr/local/lib testOmafReaderManager.o libgtest.a -o testOmafReaderManager ${LD_FLAGS}

./run.sh
if [ $? -ne 0 ]; then exit 1; fi
