#include "xHttpResponse.h"
#include "xBuffer.h"
void xHttpResponse::appendToBuffer(xBuffer* output) const
{
  char buf[32];
  snprintf(buf, sizeof buf, "HTTP/1.1 %d ", statusCode);
  output->append(buf);
  output->append(statusMessage);
  output->append("\r\n");

  if (closeConnection)
  {
    output->append("Connection: close\r\n");
  }
  else
  {
    snprintf(buf, sizeof buf, "Content-Length: %zd\r\n", body.size());
    output->append(buf);
    output->append("Connection: Keep-Alive\r\n");
  }

  for (std::map<std::string, std::string>::const_iterator it = headers.begin();
       it != headers.end(); ++it)
  {
    output->append(it->first);
    output->append(": ");
    output->append(it->second);
    output->append("\r\n");
  }

  output->append("\r\n");
  output->append(body);
}
