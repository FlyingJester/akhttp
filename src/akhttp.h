#pragma once

enum AK_HTTPMethod { AK_eGet, AK_ePost, AK_eHead, AK_ePut, AK_eDelete };

struct AK_Field { const char *data; unsigned len; };

struct AK_HTTPRequest{
    enum AK_HTTPMethod method;
    unsigned char version_major, version_minor;

    struct AK_Field data, host;
};

struct AK_HTTPResponse{
    unsigned char version_major, version_minor;
    unsigned code;
    
    unsigned num_fields, fields_capacity;
    struct AK_Field *fields, *names;
    
    struct AK_Field msg;
};

struct AK_Field *AK_GetHTTPResponseField(struct AK_HTTPResponse *rsp,
    const char *name);

/* Returns the position of the new field. */
unsigned AK_SetHTTPResponseField(struct AK_HTTPResponse *rsp,
    const struct AK_Field *name, const struct AK_Field *value);
unsigned AK_AppendHTTPResponseField(struct AK_HTTPResponse *rsp,
    const struct AK_Field *name, const struct AK_Field *value);

unsigned AK_ParseHTTPRequest(struct AK_HTTPRequest *out, const char *msg);
unsigned AK_ParseHTTPRequestN(
    struct AK_HTTPRequest *out, const char *msg, unsigned len);
unsigned AK_ParseHTTPResponse(struct AK_HTTPResponse *out, const char *msg);
unsigned AK_ParseHTTPResponseN(
    struct AK_HTTPResponse *out, const char *msg, unsigned len);

/* Semi-private, these functions are generally only useful for deconstructing 
 * HTTP messages, and these are used to construct other library functions. */

/* It is acceptable if &in == out */
int AK_ParseHTTPVersion(const char *in, const char **out,
    unsigned char *out_maj, unsigned char *out_min, unsigned nl);
int AK_ParseHTTPVersionN(const char *in, const char **out, unsigned len,
    unsigned char *out_maj, unsigned char *out_min, unsigned nl);

int AK_ParseHTTPProperty(const char *in, const char **out,
    struct AK_Field *name, struct AK_Field *value);
int AK_ParseHTTPPropertyN(const char *in, const char **out, unsigned len,
    struct AK_Field *name, struct AK_Field *value);
