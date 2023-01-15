mkdir -p build && cd build
cmake -DCMAKE_BUILD_TYPE=Debug -DFSANITIZE=address,undefined .. && make && ./bin/Game