#include "httphandler.h"

HttpHandler::HttpHandler()
    : urlBase(std::string("/var/www"))
    // , logger(nullptr)
{
    
}

HttpHandler::HttpHandler(std::string urlBase)
    : urlBase(urlBase)
{

}

HttpHandler::~HttpHandler()
{

}

/**
 * @param recv: 接收字节
 * @param ret: 响应报文
 * @param logMessage: 日志信息
 * @return -1: 错误, 0: 长连接, 1: 短连接
*/
int HttpHandler::handle(const char * recv, std::string & ret, std::string & logMessage)
{
    if (recv == nullptr) {
        printf("no request!\n");
        ret = ret + RESPONSE_400 + "\r\n";
        return -1;
    }

    std::string request(recv);

    // 提取请求方法
    size_t method_end = request.find(' ');
    if (method_end == std::string::npos) {
        printf("Invalid request\n");
        ret = ret + RESPONSE_400 + "\r\n";
        return -1;
    }
    std::string method = request.substr(0, method_end);

    if (method != "GET") {
        // 非法请求方法
        printf("Invalid method\n");
        ret = ret + RESPONSE_400 + "\r\n";
        return -1;
    }

    // 提取请求路径
    size_t url_start = method_end + 1;
    size_t url_end = request.find(' ', url_start);
    if (url_end == std::string::npos) {
        printf("Invalid request\n");
        ret = ret + RESPONSE_400 + "\r\n";
        return -1;
    }
    std::string url = request.substr(url_start, url_end - url_start);
    if (url == std::string("/")) {
        url = std::string("/index.html");
    }
    url = urlBase + url;
    url = decodeUrl(url);
    url = extractFileName(url);

    // 拼接日志消息
    std::stringstream ssLog;
    ssLog << method << " " << url;
    logMessage = ssLog.str();

    std::string content;

    // 生成响应
    // 生成响应type
    std::string contentType;
    size_t dot_pos = url.find_last_of('.');
    if (dot_pos == std::string::npos) {
        // 如果没有找到 `.` 字符，说明没有后缀名
        contentType = PLAIN;
    }
    // 提取后缀名
    std::string extension = url.substr(dot_pos + 1);
    if (extension == "html") {
        contentType = HTML;
        content = readTextFile(url);
    } else if (extension == "js") {
        contentType = JS;
        content = readTextFile(url);
    } else if (extension == "css") {
        contentType = CSS;
        content = readTextFile(url);
    } else if (extension == "jpg" || extension == "jpeg") {
        contentType = JPG;
        content = readBinaryFile(url);
    } else if (extension == "png") {
        contentType = PNG;
        content = readBinaryFile(url);
    } else {
        // 其他未知文件类型，默认设置为纯文本
        contentType = PLAIN;
        content = readTextFile(url);
    }

    if (content == "") {
        printf("404 not found!\n");
        ret = ret + RESPONSE_404 + "\r\n";
        return -1;
    }

    std::string keepAlive = "Connection: keep-alive\r\n"; // 这个逻辑不对

    std::ostringstream oss;
    oss << "Content-length: " << content.length() << "\r\n\r\n";
    std::string ContenLen = oss.str();
    ret = ret + RESPONSE_200 + contentType + keepAlive + ContenLen;
    ret = ret + content;

    // 判断连接类型
    const char* connectionStr = "Connection: ";
    const char* connectionLine = strstr(recv, connectionStr); // 在 recv 中搜索 "Connection: "
    if (connectionLine != nullptr) {
        connectionLine += strlen(connectionStr); // 跳过 "Connection: " 字符串

        // 查找行末尾的换行符
        const char* endOfLine = strchr(connectionLine, '\n');
        if (endOfLine != nullptr) {
            // 提取行内容
            std::string line(connectionLine, endOfLine - connectionLine);
            // 在行内容中查找连接状态
            size_t pos = line.find("close");
            if (pos != std::string::npos) {
                return 1;
            }
        }
    }
    return 0;
}

std::string HttpHandler::decodeUrl(const std::string & url) {
    std::ostringstream decoded_url;
    char current_char;
    size_t index = 0;
    while (index < url.length()) {
        current_char = url[index];
        if (current_char == '%') {
            // 解码 '%' 字符后面的两位十六进制数
            if (index + 2 < url.length()) {
                std::string hex_code = url.substr(index + 1, 2);
                int hex_value;
                std::stringstream ss;
                ss << std::hex << hex_code;
                ss >> hex_value;
                decoded_url << static_cast<char>(hex_value);
                index += 3; // 跳过 '%' 和两位十六进制数
            } else {
                // 如果 '%' 字符后面不足两位十六进制数，则直接拼接 '%' 字符
                decoded_url << current_char;
                index++;
            }
        } else if (current_char == '+') {
            // 将 '+' 替换成空格
            decoded_url << ' ';
            index++;
        } else {
            // 直接拼接其他字符
            decoded_url << current_char;
            index++;
        }
    }
    return decoded_url.str();
}

std::string HttpHandler::extractFileName(const std::string & url)
{
    // 查找问号的位置
    size_t questionMarkPos = url.find('?');
    // 如果找到问号，只截取到问号之前的部分
    if (questionMarkPos != std::string::npos) {
        return url.substr(0, questionMarkPos);
    }
    // 如果没有找到问号，则返回整个 URL
    return url;
}

std::string HttpHandler::readTextFile(const std::string & url)
{
    std::ifstream file(url);
    if (!file.is_open()) {
        printf("failed to open file: %s\n", url.c_str());
        return "";
    }
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();

    return content;
}

std::string HttpHandler::readBinaryFile(const std::string & url)
{
    std::ifstream file(url, std::ios::binary);
    if (!file.is_open()) {
        printf("failed to open file: %s\n", url.c_str());
        return "";
    }
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();

    return content;
}
