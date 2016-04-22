#include "akhttp.h"
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#define MAX_REQUEST_SIZE 8192

static unsigned is_digit(char c){
    return c <= '9' && c>= '0';
}

static const char *request_name(enum AK_HTTPMethod m){
    switch(m){
        case AK_eGet:
            return "GET";
        case AK_eHead:
            return "HEAD";
        case AK_eDelete:
            return "DELETE";
        case AK_eOptions:
            return "OPTIONS";
        case AK_eTrace:
            return "TRACE";
        case AK_ePost:
            return "POST";
        case AK_ePut:
            return "PUT";
        case AK_eConnect:
            return "CONNECT";
        case AK_ePatch:
            return "PATCH";
    }
    
    assert(NULL && "Invalid HTTP method");
    return "NIGHTS ";
};

static unsigned is_lower_case(char c){
    return c >= 'a' && c<= 'z';
}

static enum AK_HTTPMethod get_request_type(const char *str, int *offset){
    char buffer[8];
    unsigned i = 0;
    for(;i<8; i++){
        if(is_lower_case(buffer[i] = str[i]))
            buffer[i] += (int)'A' - (int)'a';
        
        if(str[0] == '\0')
            break;
    }
    
    for(i=0; i<AK_eInvalidMethod; i++){
        int n = 0;
        const char *const other = request_name(i);
        do{
            if(other[n] != str[n]){
                n = -1;
                break;
            }
        }while(other[++n] != '\0');
        
        if(n && (str[n] == ' ' || str[n] == '\t')){
            offset[0] = n+1;
            return i;
        }
    }
    
    return AK_eInvalidMethod;
    
}

unsigned AK_MethodRequestHasBody(enum AK_HTTPMethod m){
    switch(m){
        case AK_eGet:
        case AK_eHead:
        case AK_eDelete:
        case AK_eOptions:
        case AK_eTrace:
            return 0u;
        case AK_ePost:
        case AK_ePut:
        case AK_eConnect:
        case AK_ePatch:
            return 1u;
    }
    
    assert(NULL && "Invalid HTTP method");
    return 0u; 
}

unsigned AK_RequestMethodRequestHasBody(const struct AK_HTTPRequest *r){
    return AK_MethodRequestHasBody(r->method); 
}

unsigned AK_MethodResponseHasBody(enum AK_HTTPMethod m){
    switch(m){
        case AK_eHead:
            return 0u;
        case AK_eGet:
        case AK_eDelete:
        case AK_eOptions:
        case AK_ePost:
        case AK_ePut:
        case AK_eConnect:
        case AK_ePatch:
        case AK_eTrace:
            return 1u;
    }
    
    assert(NULL && "Invalid HTTP method");
    return 0u; 
}

unsigned AK_RequestMethodResponseHasBody(const struct AK_HTTPRequest *r){
    return AK_MethodResponseHasBody(r->method);
}

unsigned AK_ParseHTTPRequest(struct AK_HTTPRequest *out, const char *msg){
    return AK_ParseHTTPRequestN(out, msg, strnlen(msg, MAX_REQUEST_SIZE + 1));
}

unsigned AK_ParseHTTPRequestN(
    struct AK_HTTPRequest *out, const char *msg, unsigned len){
    if(len > MAX_REQUEST_SIZE)
        return 1;
    
    
    
}
unsigned AK_ParseHTTPResponse(struct AK_HTTPResponse *rsp, const char *msg){
    return AK_ParseHTTPResponseN(rsp, msg, strlen(msg));
}

unsigned AK_MethodRequestHasBody(enum AK_HTTPMethod m);
unsigned AK_MethodResponseHasBody(enum AK_HTTPMethod m);

unsigned AK_ParseHTTPResponseN(struct AK_HTTPResponse *rsp, const char *msg, unsigned len){
    const char *end;
#define LEN_SYNC() \
    len -= end-msg;\
    msg = end
    
#define MSG_NEXT() do{\
        msg++; \
        len--; } while(0)

    int err = AK_ParseHTTPVersionN(msg, &end, len, 
        &rsp->version_major, &rsp->version_minor, 0);

    if(err!=0)
        return err;
    
    LEN_SYNC();
    rsp->code = 0u;

    while(len && !is_digit(*msg))
        MSG_NEXT();

    while(len && is_digit(*msg)){
        rsp->code *= 10u;
        rsp->code += (*msg) - '0';

        MSG_NEXT();
    }

    while(len && (*msg) != '\n')
        MSG_NEXT();
    
    rsp->num_fields = 0;
    
    if(len == 0){
        rsp->fields_capacity = 0;
        rsp->fields = rsp->names = NULL;
        rsp->msg.len = 0;
        rsp->msg.data = NULL;
        
        return 100;
    }
    else{
        assert(*msg == '\n');
        msg++;
        len--;
    }
    
    rsp->fields_capacity = 16;
    rsp->fields = malloc(16 * sizeof(struct AK_Field));
    rsp->names = malloc(16 * sizeof(struct AK_Field));
    
    while(len && *msg != '\r' && *msg != '\n'){
        if(rsp->num_fields+1 >= rsp->fields_capacity){
            rsp->fields_capacity += 16;
            rsp->fields = realloc(rsp->fields, rsp->fields_capacity * sizeof(struct AK_Field));
            rsp->names = realloc(rsp->names, rsp->fields_capacity * sizeof(struct AK_Field));
        }
        err = AK_ParseHTTPPropertyN(msg, &end, len, rsp->names + rsp->num_fields, rsp->fields + rsp->num_fields);
        LEN_SYNC();
        
        if(err)
            return 200 + err;
        
        rsp->num_fields++;
        
    }
    
    if(len < 2)
        return 105;
    
    if(msg[0] != '\r' || msg[1] != '\n')
        return 107;
    
    MSG_NEXT();
    MSG_NEXT();
    
    rsp->msg.data = msg;
    rsp->msg.len = len;
    
    return 0;
}

int AK_ParseHTTPVersionN(const char *in, const char **out, unsigned len,
    unsigned char *out_maj, unsigned char *out_min, unsigned nl){
    assert(in);
    assert(out);
    assert(out_min);
    assert(out_maj);

    if((nl && len < 9) || ((!nl) && len < 7))
        return 1;
    else
        return AK_ParseHTTPVersion(in, out, out_maj, out_min, nl);
}

int AK_ParseHTTPVersion(const char *in, const char **out,
    unsigned char *out_maj, unsigned char *out_min, unsigned nl){
    assert(in);
    assert(out);
    assert(out_min);
    assert(out_maj);
    
    if(!(
        in[0] == 'H' &&
        in[1] == 'T' &&
        in[2] == 'T' &&
        in[3] == 'P' &&
        in[4] == '/'))
        return 3;
    if(!(in[5] > '0' && in[5] <= '9'))
        return 5;
    if(in[6] != '.')
        return 7;
    if(!(in[7] >='0' && in[7] <= '9'))
        return 9;
    
    if(nl){
        if(in[8] == '\r' && in[9] == '\n')
            out[0] = in + 10;
        else if(in[8] == '\n')
            out[0] = in + 9;
        else
            return 10;
    }
    else
        out[0] = in + 8;

    out_maj[0] = in[5] - '0';
    out_min[0] = in[7] - '0';
    
    return 0;
}

int AK_ParseHTTPProperty(const char *in, const char **out,
    struct AK_Field *name, struct AK_Field *value){
    assert(in);
    assert(out);
    assert(name);
    assert(value);
    return AK_ParseHTTPPropertyN(in, out, strlen(in), name, value);
}

int AK_ParseHTTPPropertyN(const char *in, const char **out, unsigned len,
    struct AK_Field *name, struct AK_Field *value){
    
    unsigned name_len = 0, value_len = 0;
    const char *name_data = in, *value_data = NULL;
    
    assert(in);
    assert(out);
    assert(name);
    assert(value);
    
    if(len < 5)
        return 1;
    
    while(len && *in != ':'){
        len--;
        name_len++;
        in++;
    }
    
    if(!len)
        return 3;
    if(len && *in == ':'){
        len--;
        in++;
    }

    while(len && *in == ' '){
        len--;
        in++;
    }
    
    if(!len)
        return 5;
    
    value_data = in;
    while(len && *in != '\n' && *in != '\r'){
        len--;
        value_len++;
        in++;
    }
    
    if(in[0] == '\r' && in[1] == '\n')
        in+=2;
    else if(in[0] == '\n')
        in++;
    
    out[0] = in;
    name->data = name_data;
    name->len = name_len;
    value->data = value_data;
    value->len = value_len;
    
    return 0;
}
struct AK_Field *AK_GetHTTPResponseField(struct AK_HTTPResponse *rsp,
    const char *name){
    
    assert(rsp);
    assert(name);
    {
        const unsigned long name_len = strlen(name);
        unsigned i = 0;
        while( i < rsp->num_fields ){
            if(rsp->names[i].len == name_len && memcmp(name, rsp->names[i].data, name_len) == 0)
                return rsp->fields + i;
            i++;
        }
    }
    return NULL;
}

/* Returns the position of the new field. */
unsigned AK_SetHTTPResponseField(struct AK_HTTPResponse *rsp,
    const struct AK_Field *name, const struct AK_Field *value){
    
    assert(rsp);
    assert(name);
    assert(value);
    assert(value->data);
    
    {
        unsigned i = 0;
        
        while( i < rsp->num_fields ){
            if(rsp->names[i].len == name->len && memcmp(name->data, rsp->names[i].data, name->len) == 0){
                rsp->fields[i].len = value->len;
                rsp->fields[i].data = value->data;
                return i;
            }
        }
    }

    return AK_AppendHTTPResponseField(rsp, name, value);
}

unsigned AK_AppendHTTPResponseField(struct AK_HTTPResponse *rsp,
    const struct AK_Field *name, const struct AK_Field *value){
    
    assert(rsp);
    assert(name);
    assert(value);
    assert(value->data);

    if(rsp->num_fields == rsp->fields_capacity){
        rsp->fields_capacity <<= 1;
        rsp->fields = realloc(rsp->fields, sizeof(struct AK_Field) * rsp->fields_capacity);
        rsp->names = realloc(rsp->names, sizeof(struct AK_Field) * rsp->fields_capacity);
    }

    rsp->fields[rsp->num_fields].data = value->data;
    rsp->fields[rsp->num_fields].len = value->len;
    rsp->fields[rsp->num_fields].len = value->len;
    
    rsp->names[rsp->num_fields].data = name->data;
    rsp->names[rsp->num_fields].len = name->len;
    
    return rsp->num_fields++;

}
