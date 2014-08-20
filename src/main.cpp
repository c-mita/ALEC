#include <string>
#include <vector>
#include <iostream>
#include <fstream>

#define asc_CR 0x0D
#define asc_LF 0x0A

enum CONVERSION {
    DOS_TO_UNIX,
    UNIX_TO_DOS
};

void LfToCrLf( std::vector< char >* data ) {
    std::vector< int > positions;
    int index = 0;
    for ( auto it = data->begin(); it != data->end(); ++it ) {
        if ( *it == asc_LF && it != data->begin() && *(it - 1) != asc_CR ) {
            positions.push_back( index );
        }
        index++;
    }
    data->reserve( data->size() + positions.size() );
    int offset = 0;
    for ( auto it = positions.begin(); it != positions.end(); ++it ) {
        auto itData = data->begin();
        itData += (*it) + offset;
        data->insert( itData, asc_CR );
        offset++;
    }
    return;
}

void CrLfToLf( std::vector< char >* data ) {
    std::vector< char > newData;
    newData.reserve( data->size() );
    for ( auto it = data->begin(); it != data->end(); ++it ) {
        if ( *it != asc_CR || ( (it + 1) != data->end() && *(it + 1) != asc_LF ) ) {
            newData.push_back( *it );
        }
    }
    *data = std::move( newData );
    return;
}

void WriteFile( std::string fileName, std::vector< char >* data ) {
    std::ofstream outputFile( fileName, std::ios::out | std::ios::binary );
    outputFile.write( &(*data)[0], data->size() );
    outputFile.close();
}

void ProcessArgs( std::vector< std::string >* args, CONVERSION* conversion, std::string* input, std::string* output ) {
    std::vector< std::string > files;
    for ( auto it = args->begin() + 1; it != args->end(); ++it ) {
        if ( *it == "-D" || *it == "-d" ) {
            *conversion = DOS_TO_UNIX;
            continue;
        }
        if ( *it == "-U" || *it == "-u" ) {
            *conversion = UNIX_TO_DOS;
            continue;
        }
        files.push_back( *it );
    }
    if ( files.size() < 1 ) throw;
    *input = files[0];
    *output = files.size() < 2 ? files[0] : files[1];
    return;
}

int main( int argc, char** argv ) {
    std::vector< std::string > args( argv, argv + argc );

    CONVERSION conversion = DOS_TO_UNIX;
    std::string input;
    std::string output;
    try {
        ProcessArgs( &args, &conversion, &input, &output );
    } catch ( ... ) {
        std::cout << "Invalid arguments" << std::endl;
        return 1;
    }

    std::vector< char > data;
    std::ifstream inputFile;
    inputFile.exceptions( std::ifstream::failbit | std::ifstream::badbit );
    try {
        inputFile.open( input, std::ios::binary );
        if ( inputFile ) {
            inputFile.seekg( 0, std::ios::end );
            std::streampos length = inputFile.tellg();
            inputFile.seekg( 0, std::ios::beg );
            data.resize( length );
            std::cout << length << std::endl;
            inputFile.read( &data[0], length );
        }
    } catch ( std::ifstream::failure ex ) {
        std::cout << "Exception reading file" << std::endl;
        return 1;
    }

    if ( conversion == DOS_TO_UNIX ) {
        LfToCrLf( &data );
    } else {
        CrLfToLf( &data );
    }

    try {
        WriteFile( output, &data );
    } catch ( ... ) {
        std::cout << "Exception writing file" << std::endl;
        return 1;
    }

    return 0;
}
