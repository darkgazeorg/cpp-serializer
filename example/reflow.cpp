#include "cpp-serializer/txt.hpp"
#include <fmt/format.h>
#include <fstream>
#include <iostream>
#include <stdexcept>

int main(int argc, char *argv[]) {
    if(argc < 2 || argc > 4) {
        fmt::print(stderr, "Usage: {} width [inputfile [outputfile]]\n", argv[0]);
        return 1;
    }

    //convert wrapwidth
    size_t wrapwidth;
    try {
        size_t end;
        wrapwidth = std::stoul(argv[1], &end);
        if(end != strlen(argv[1])) {
            throw std::runtime_error("");
        }
    }
    catch(...) {
        fmt::print(stderr, "Width should be a positive number");
        return 1;
    }

    //obtain streams
    std::istream *input;
    std::ostream *output;

    if(argc > 2) {
        input = new std::ifstream(argv[2]);
    }
    else {
        input = &std::cin;
    }

    if(argc > 3) {
        output = new std::ofstream(argv[3]);
    }
    else {
        output = &std::cout;
    }

    //build transport
    ser::RuntimeTextTransport transport;
    transport.SetWrapWidth(wrapwidth);
    transport.SetFolding(false);

    //parse and emit with new settings
    auto parsed = transport.Parse(*input);
    transport.Emit(parsed, *output);


    if(argc > 2) static_cast<std::ifstream*>(input)->close();
    if(argc > 3) static_cast<std::ofstream*>(output)->close();

    return 0;
}
