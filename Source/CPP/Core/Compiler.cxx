#include "Compiler.hxx"

#include <stdexcept>

StoreLines *Compiler::Compile(FileSystemBase *fileSystem, const char *filename)
{
    vector<const char *> *program = fileSystem->Contents(filename);
    return(Compile(program));
}

StoreLines *Compiler::Compile(const vector<const char *> *program)
{
    if (program->size() == 0)
    {
        throw runtime_error("Program is null");
    }

    auto storeLines = new StoreLines();

    return(storeLines);
}

vector<Compiler::TokenisedLine *> *Compiler::Tokenise(const vector<const char *> &)
{
    vector<Compiler::TokenisedLine *> *result = new vector<Compiler::TokenisedLine *>();

    return(result);
}