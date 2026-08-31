#include "web_server.h"
#include "esp_log.h"
#include "esp_http_server.h"
#include "wifi_sta.h"

static const char *TAG = "WEB";

/* =========================
 *  WIFI POST 处理函数
 * ========================= */
static esp_err_t wifi_post_handler(httpd_req_t *req)
{
    char buf[128] = {0};

    size_t recv_size = (req->content_len < (sizeof(buf) - 1)) ? req->content_len : (sizeof(buf) - 1);
    int len = httpd_req_recv(req, buf, recv_size);
    if (len <= 0)
    {
        ESP_LOGE(TAG, "recv failed");
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }

    buf[len] = '\0';

    ESP_LOGI(TAG, "POST DATA: %s", buf);

    char ssid[32] = {0};
    char password[64] = {0};

    httpd_query_key_value(buf, "ssid", ssid, sizeof(ssid));
    httpd_query_key_value(buf, "password", password, sizeof(password));

    ESP_LOGI(TAG, "SSID: %s", ssid);
    ESP_LOGI(TAG, "PASSWORD: %s", password);

    /* TODO: 这里接你的 STA 连接函数 */
    wifi_sta_connect(ssid,password,true);

    httpd_resp_sendstr(req, "正在连接 WiFi...");

    return ESP_OK;
}

/* =========================
 *  GET / 首页
 * ========================= */
static esp_err_t index_handler(httpd_req_t *req)
{
    const char *html =
       "<!DOCTYPE html>"
"<html lang=\"zh\">"
""
"<head>"
"    <meta charset=\"UTF-8\">"
"    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">"
"    <title>Orange 配网</title>"
""
"    <style>"
""
"        *{"
"            margin:0;"
"            padding:0;"
"            box-sizing:border-box;"
"        }"
""
"        body{"
""
"            background:#f4f8fb;"
""
"            font-family:"
"            Arial,"
"            \"Microsoft YaHei\","
"            sans-serif;"
""
"            display:flex;"
""
"            justify-content:center;"
""
"            align-items:center;"
""
"            height:100vh;"
"        }"
""
"        .box{"
""
"            width:360px;"
""
"            background:white;"
""
"            border-radius:14px;"
""
"            padding:28px;"
""
"            box-shadow:0 8px 25px rgba(0,0,0,.08);"
"        }"
""
"        h2{"
""
"            text-align:center;"
""
"            margin-bottom:10px;"
""
"            color:#333;"
"        }"
""
"        p{"
""
"            color:#777;"
""
"            margin-bottom:20px;"
"        }"
""
"        label{"
""
"            display:block;"
""
"            margin-bottom:6px;"
""
"            color:#555;"
"        }"
""
"        input{"
""
"            width:100%;"
""
"            padding:12px;"
""
"            border:none;"
""
"            outline:none;"
""
"            border-radius:8px;"
""
"            background:#edf2f7;"
""
"            margin-bottom:18px;"
""
"            font-size:15px;"
"        }"
""
"        input:focus{"
""
"            background:white;"
""
"            border:2px solid #1e88e5;"
"        }"
""
"        button{"
""
"            width:100%;"
""
"            padding:12px;"
""
"            border:none;"
""
"            border-radius:8px;"
""
"            background:#1e88e5;"
""
"            color:white;"
""
"            font-size:16px;"
""
"            cursor:pointer;"
"        }"
""
"        button:hover{"
""
"            background:#1976d2;"
"        }"
""
"        #status{"
""
"            margin-top:20px;"
""
"            text-align:center;"
""
"            color:#1e88e5;"
""
"            font-weight:bold;"
"        }"
""
"    </style>"
""
"</head>"
""
"<body>"
""
"<div class=\"box\">"
""
"    <h2>Orange 配网</h2>"
""
"    <p>请输入需要连接的 Wi-Fi</p>"
""
"    <label>Wi-Fi 名称</label>"
""
"    <input"
"            id=\"ssid\""
"            type=\"text\""
"            placeholder=\"SSID\">"
""
"    <label>Wi-Fi 密码</label>"
""
"    <input"
"            id=\"password\""
"            type=\"password\""
"            placeholder=\"Password\">"
""
"    <button onclick=\"connectWifi()\">"
"        连接"
"    </button>"
""
"    <div id=\"status\">"
""
"        等待配置..."
""
"    </div>"
""
"</div>"
""
"<script>"
""
"function connectWifi()"
"{"
""
"    let ssid ="
"        document.getElementById(\"ssid\").value;"
""
"    let password ="
"        document.getElementById(\"password\").value;"
""
"    document.getElementById(\"status\").innerHTML ="
"        \"正在连接...\";"
""
"    fetch(\"/wifi\","
"    {"
""
"        method:\"POST\","
""
"        headers:"
"        {"
"            \"Content-Type\":\"application/x-www-form-urlencoded\""
"        },"
""
"        body:"
"            \"ssid=\" +"
"            encodeURIComponent(ssid)"
"            +"
"            \"&password=\" +"
"            encodeURIComponent(password)"
""
"    })"
""
"    .then(response=>response.text())"
""
"    .then(text=>"
"    {"
"        document.getElementById(\"status\").innerHTML=text;"
"    })"
""
"    .catch(()=>"
"    {"
"        document.getElementById(\"status\").innerHTML=\"连接失败\";"
"    });"
""
"}"
""
"</script>"
""
"</body>"
""
"</html>";


    httpd_resp_set_type(req, "text/html; charset=utf-8");
    httpd_resp_send(req, html, HTTPD_RESP_USE_STRLEN);

    return ESP_OK;
}

/* =========================
 *  URI 注册表（核心）
 * ========================= */
static const httpd_uri_t index_uri =
{
    .uri = "/",
    .method = HTTP_GET,
    .handler = index_handler,
    .user_ctx = NULL
};

static const httpd_uri_t wifi_uri =
{
    .uri = "/wifi",
    .method = HTTP_POST,
    .handler = wifi_post_handler,
    .user_ctx = NULL
};

/* =========================
 *  启动 HTTP Server
 * ========================= */
void web_server_start(void)
{
    httpd_handle_t server = NULL;

    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    if (httpd_start(&server, &config) == ESP_OK)
    {
        httpd_register_uri_handler(server, &index_uri);
        httpd_register_uri_handler(server, &wifi_uri);

        ESP_LOGI(TAG, "HTTP server started");
    }
}
