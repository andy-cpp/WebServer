#include "php.hpp"

#include <system/system.hpp>
#include "handler.hpp"

ssize_t ReadFromFP(FILE* fp, std::string& buffer)
{
    char cbuf[1024] = {0};
    ssize_t bytes_read = 0;
    while(1)
    {
        auto status = fread(cbuf, 1, sizeof(cbuf), fp);
        buffer.append(cbuf, status);
        bytes_read += status;
        if(status < sizeof(cbuf))
            break;
    }
    return bytes_read;
}

std::string ReadFromFP(FILE* fp)
{
    std::string buf;
    ReadFromFP(fp, buf);
    return buf;
}

HTTP::http_t HandlePHPCGIOutput(Server* server, Socket* client, HTTP::http_t* request, ProgramContext* context)
{
    std::string buffer = ReadFromFP(context->stdout);

    HTTP::http_t response = Response_200;

    auto headers = HTTPParser::ParseHeaders(buffer);
    for(auto& header : headers)
    {
        if(header.key == "Status"){
            size_t lpos = header.value.find(" ", 1);
            response.http_code = header.value.substr(1, lpos);
            response.http_response_status = header.value.substr(lpos + 1);
        }
        else
            response.headers.push_back(header);
    }

    response.message = buffer.substr(buffer.find("\r\n\r\n") + 4);
    
    return response;
}

#include <http/urlparam.hpp>
#include <http/urlencode.hpp>
#include <chrono>

bool PHPHandler(Server* server, Socket* client, HTTP::http_t* request)
{
    std::string file = server->m_config.Get("webroot").ToString() + request->GetFile();

    if(!FS::IsFile(file)){
        return client->Send(Response::ToString(Response_404));
        return false;
    }

    std::vector<std::string> env = {};
    env.push_back("REQUEST_METHOD=" + request->http_request_method);
    env.push_back("SERVER_PROTOCOL=" + request->http_version);
    if(!request->query.empty())
        env.push_back("QUERY_STRING=" + request->query);
    env.push_back("REMOTE_ADDR=" + client->m_details.GetIP());
    env.push_back("REQUEST_URI=" + request->http_path + "?" + request->query);
    env.push_back("SCRIPT_FILENAME=" + GetRealPath(server->m_config.Get("webroot").ToString() + request->http_path));
    env.push_back(std::string("SCRIPT_NAME=") + request->GetFile());
    env.push_back("REDIRECT_STATUS=200");

    auto context = ExecuteProgram("/usr/bin/php-cgi", {}, env);
    if(context.status != 0){
        client->Send(Response::ToString(Response_500));
        std::string buf = ReadFromFP(context.stderr);
        server->logger.Log("%s\n", buf.c_str());
        printf("php-cgi error: \n- %s\n", buf.c_str());
        return true;
    }

    client->Send(Response::ToString(HandlePHPCGIOutput(server, client, request, &context)));


    printf("php-cgi returned with value %d\n", context.status);

    return true; // Return ok :)
}