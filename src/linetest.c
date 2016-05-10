#include "akhttp.h"
#include <stdio.h>
#include <stdlib.h>
#include "bufferfile/bufferfile.h"

static void print_fields_response(const struct AK_HTTPResponse *rsp){
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

static void print_fields_request(const struct AK_HTTPRequest *rqt){
    unsigned i = 0;
    
    printf("Printing request with %i fields\n", rqt->num_fields);
    
    while(i<rqt->num_fields){
        fwrite(rqt->names[i].data, rqt->names[i].len, 1, stdout);
        fputs(" : ", stdout);
        fwrite(rqt->fields[i].data, rqt->fields[i].len, 1, stdout);
        fputc('\n', stdout);
        i++;
    }
}

int main(int argc, char *argv[]){
    char *filedata;
    int len;
    struct AK_HTTPResponse rsp;
    struct AK_HTTPRequest rqt;
    unsigned response = 1;
    puts("Beginning linetest.");
    
    if(argc < 2){
        puts("usage: linetest [request|response] <infile>");
        return EXIT_FAILURE;
    }

    if(argc > 3){
        if(strcmp(argv[1], "request")==0)
            response = 0;
        else if(strcmp(argv[1], "response") != 0){
            puts("argument 1 must be either `request` or `response`");
            return 1;
        }
        filedata = BufferFile(argv[2], &len);
    }
    else
        filedata = BufferFile(argv[1], &len);
    
    if(response){
        const int err = AK_ParseHTTPResponseN(&rsp, filedata, len);
        
        if(err == 0){
            print_fields_response(&rsp);
        }
        else{
            printf("Error %i\n", err);
        }
    }
    else{
        const int err = AK_ParseHTTPRequestN(&rqt, filedata, len);
        
        if(err == 0){
            print_fields_request(&rqt);
        }
        else{
            printf("Error %i\n", err);
        }
    }
    
    FreeBufferFile(filedata, len);
    
    return 0;
}
