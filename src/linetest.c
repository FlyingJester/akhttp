#include "akhttp.h"
#include <stdio.h>
#include <stdlib.h>
#include "bufferfile/bufferfile.h"

static void print_fields(const struct AK_HTTPResponse *rsp){
    unsigned i = 0;
    
    printf("Printing response with %i fields\n", rsp->num_fields);
    
    while(i<rsp->num_fields){
        fwrite(rsp->names[i].data, rsp->names[i].len, 1, stdout);
        fputs(" : ", stdout);
        fwrite(rsp->fields[i].data, rsp->fields[i].len, 1, stdout);
        fputc('\n', stdout);
        i++;
    }
}

int main(int argc, char *argv[]){
    char *filedata;
    int len;
    struct AK_HTTPResponse rsp;
    puts("Beginning linetest.");
    
    if(argc < 2){
        puts("usage: linetest <infile>");
        return EXIT_FAILURE;
    }

    filedata = BufferFile(argv[1], &len);
    
    AK_ParseHTTPResponseN(&rsp, filedata, len);
    
    print_fields(&rsp);
    
    FreeBufferFile(filedata, len);
    

}
