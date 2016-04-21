#pragma once

/**
 * Indicates which HTTP method a request uses
 */
enum AK_HTTPMethod {
    AK_eGet,
    AK_ePost,
    AK_eHead,
    AK_ePut,
    AK_eDelete
};

/**
 * Specifies a memory location and length. This lets us use a single string to
 * represent multiple values inside it.
 */
struct AK_Field { const char *data; unsigned len; };

/**
 * Represents an HTTP request header.
 */
struct AK_HTTPRequest{
    enum AK_HTTPMethod method; /**< Which method the HTTP request invoked */
    unsigned char version_major; /**< HTTP major version */
    unsigned char version_minor; /**< HTTP minor version */

    struct AK_Field data; /**< Payload of the request (HTML, JSON, etc) */
    struct AK_Field host; /**< Hostname that sent the request */
};

struct AK_HTTPResponse{
    unsigned code; /**< HTTP response code (404, 200, 415, etc) */
    unsigned char version_major; /**< HTTP major version */
    unsigned char version_minor; /**< HTTP minor version */
    
    unsigned num_fields, fields_capacity; /**< field array metadata */
    struct AK_Field *fields; /**< Array of fields in the response */
    struct AK_Field *names; /**< Array of names for fields. */
    
    struct AK_Field msg; /**< HTTP response message */
};

/**
 * @brief Retrieves a single field from a response.
 *
 * @param rsp Response to find field in
 * @param name Name of field to find
 * @return Field if found, NULL if the field does not exist in @p rsp
 */
struct AK_Field *AK_GetHTTPResponseField(struct AK_HTTPResponse *rsp,
    const char *name);

/**
 * @brief Sets a field on a response.
 *
 * If the response already contains the field, it is changed to the value
 * specified. Otherwise, the field is appended to the array of fields.
 *
 * @param rsp Response to set field on
 * @param name Name of field to set
 * @param value new value of field
 * @return the position of the new field.
 * @sa AK_AppendHTTPResponseField
 */
unsigned AK_SetHTTPResponseField(struct AK_HTTPResponse *rsp,
    const struct AK_Field *name, const struct AK_Field *value);
/**
 * @brief Appends a field to a response
 * 
 * @note caution must be taken to avoid multiple fields with the same name.
 *
 * @param rsp Response to set field on
 * @param name Name of field to set
 * @param value new value of field
 * @return the position of the new field.
 * @sa AK_SetHTTPResponseField
 */
unsigned AK_AppendHTTPResponseField(struct AK_HTTPResponse *rsp,
    const struct AK_Field *name, const struct AK_Field *value);

/**
 * @brief parses an HTTP request.
 *
 * @note If you already know the length of the message, or wish to truncate
 * input, it is faster to use AK_ParseHTTPRequestN.
 * 
 * @param out (OUT) Request
 * @param msg Message as a null-terminated string
 * @sa AK_ParseHTTPRequestN
 */
unsigned AK_ParseHTTPRequest(struct AK_HTTPRequest *out, const char *msg);

/**
 * @brief parses an HTTP request with a specified length.
 *
 * @param out (OUT) Request
 * @param msg Message as a string
 * @param len Message length
 * @sa AK_ParseHTTPRequest
 */
unsigned AK_ParseHTTPRequestN(
    struct AK_HTTPRequest *out, const char *msg, unsigned len);
/**
 * @brief parses an HTTP response
 * 
 * @note If you already know the length of the message, or wish to truncate
 * input, it is faster to use AK_ParseHTTPResponseN.
 *
 * @param out (OUT) Response
 * @param msg Message to parse as a null-terminated string
 * @sa AK_ParseHTTPResponseN
 */
unsigned AK_ParseHTTPResponse(struct AK_HTTPResponse *out, const char *msg);

/**
 * @brief parses an HTTP response of a certain length
 *
 * @param out (OUT) Response
 * @param msg Message to parse
 * @param len Length of message
 * @sa AK_ParseHTTPResponse
 */
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
