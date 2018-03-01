#include "HTTPResponse.h"

const char *HTTPResponse::MIME_JS = "text/javascript";
const char *HTTPResponse::MIME_CSS = "text/css";
const char *HTTPResponse::MIME_HTML = "text/html";
const char *HTTPResponse::MIME_JSON = "application/json";
const char *HTTPResponse::MIME_TEXT = "text/plain";

const http_code_t HTTPResponse::CODE_OK = 200;
const http_code_t HTTPResponse::CODE_NOT_FOUND = 404;
const http_code_t HTTPResponse::CODE_SERVER_ERROR = 500;
const http_code_t HTTPResponse::CODE_WRONG_REQUEST = 400;
