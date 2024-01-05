#include <iostream>

#include "suwp_type.h"
#include "http_server.h"
// esp-idf的头文件
#include "esp_http_server.h"

static httpd_handle_t server = NULL;

static suwp_err_t favicon_get_handler(httpd_req_t *req)
{
    extern const unsigned char favicon_ico_start[] asm("_binary_favicon_ico_start");
    extern const unsigned char favicon_ico_end[] asm("_binary_favicon_ico_end");
    const size_t favicon_ico_size = (favicon_ico_end - favicon_ico_start);
    httpd_resp_set_type(req, "image/x-icon");
    httpd_resp_send(req, (const char *)favicon_ico_start, favicon_ico_size);
    return SUWP_OK;
}

static suwp_err_t root_url_handler(httpd_req_t *req)
{
    httpd_resp_sendstr_chunk(req, "<!DOCTYPE html><html><body>");
    extern const uint8_t shieru_html_start[] asm("_binary_shieru_html_start");
    extern const uint8_t shieru_html_end[] asm("_binary_shieru_html_end");
    const size_t shieru_html_size = (shieru_html_end - shieru_html_start);
    // 发送HTML响应
    httpd_resp_set_type(req, "text/html");
    httpd_resp_send_chunk(req, (const char *)shieru_html_start, shieru_html_size);
    httpd_resp_sendstr_chunk(req, NULL);
    return SUWP_OK;
}

suwp_http_server::suwp_http_server()
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    // config.uri_match_fn = httpd_uri_match_wildcard;
#if SUWP_DEBUG
    std::cout << "Starting HTTP Server on port: " << config.server_port << std::endl;
#endif
    if (httpd_start(&server, &config) != 0)
    {
#if SUWP_DEBUG
        std::cout << "Failed to start file server!" << std::endl;
#endif
    }

    httpd_uri_t icon_uri = {
        .uri = "/favicon.ico",
        .method = HTTP_GET,
        .handler = favicon_get_handler,
        .user_ctx = NULL,
    };

    httpd_register_uri_handler(server, &icon_uri);

    httpd_uri_t root_url = {
        .uri = "/",
        .method = HTTP_GET,
        .handler = root_url_handler,
        .user_ctx = NULL,
    };
    httpd_register_uri_handler(server, &root_url);
}

suwp_http_server::~suwp_http_server()
{
    httpd_stop(server);
}