mkdir -p build && cd build
echo "\033[0;32m===================== Building debug version of the app ========================"
cmake -DCMAKE_BUILD_TYPE=Debug -DFSANITIZE=address,undefined .. && make && ./bin/Game