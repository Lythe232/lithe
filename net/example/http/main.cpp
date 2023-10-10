#include "include/http/httpRequest.h"
#include "include/http/httpResponse.h"
#include "include/http/httpServer.h"
#include "include/http/httpSession.h"


using namespace lithe;
int main()
{
    std::shared_ptr<IPAddress> address(new IPAddress(AF_INET, "0.0.0.0", 5555));
    HttpServer server(address, 5);

    server.start();
}