#ifndef THREE_ADDR_CODE_H
#define THREE_ADDR_CODE_H

#include "ast.h"
#include <fstream>
#include <string>
#include <map>

using namespace std;

class ThreeAddrCodeGenerator {
private:
    ProgramNode* ast_root;
    ofstream& outcode;
    map<string, string> symbol_to_temp;
    int temp_count;
    int label_count;

public:
    ThreeAddrCodeGenerator(ProgramNode* root, ofstream& out)
        : ast_root(root), outcode(out), temp_count(0), label_count(0) {}

    void generate() {
        // TODO: Implement this method
        // This method should:
        // 1. Write a header to the output file
        // 2. Call the generate_code method of the AST root
        // 3. Write a footer to the output file
    }

    // You may add helper methods here
};

#endif // THREE_ADDR_CODE_H