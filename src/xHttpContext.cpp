#include "xHttpContext.h"
#include "xBuffer.h"

bool xHttpContext::processRequestLine(const char *begin,const char *end)
{
	bool succeed = false;
	const char *start = begin;
	const  char *space = std::find(start,end,' ');
	if(space != end && request.setMethod(start,space))
	{
		start = space+1;
		space = std::find(start, end, ' ');
		if (space != end)
		{
			const char *question = std::find(start,space ,'?');
			if(question != space)
			{
				request.setPath(start,question);
				request.setQuery(question + 1,space);
			}
			else
			{
				request.setPath(start,space);
			}

			start = space + 1;
			succeed = end - start == 8 && std::equal(start,end - 1,"HTTP/1.");
			if(succeed)
			{
				if (*(end-1) == '1')
				{
					request.setVersion(xHttpRequest::kHttp11);
				}
				else if (*(end-1) == '0')
				{
					request.setVersion(xHttpRequest::kHttp10);
				}
				else
				{
					succeed = false;
				}
			}
		}
	}
	return succeed;
}


bool xHttpContext::parseServerRequest(xBuffer *buf)
{
	const char * crlfcrlf = buf->findCRLFCRLF();
	if(crlfcrlf == nullptr)
	{
		return false;
	}

	const char * content = buf->findCONTENT();
	if(content != nullptr)
	{
		buf->retrieveUntil(content);
		const char *crlf = buf->findCRLF();
		if(crlf)
		{
			const char * colon = std::find(buf->peek(),crlf,':');
			if(colon != crlf)
			{
				request.setMethod();
				request.addContent(buf->peek(),colon,crlf);
				buf->retrieveUntil(crlfcrlf + 4);
			}
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}

	}

	return true;
}


bool xHttpContext::parseClientRequest(xBuffer *buf)
{
	bool ok = true;
	bool hasMore = true;
	while(hasMore)
	{
		if(state == kExpectRequestLine)
		{
			const char * crlf = buf->findCRLF();
			if(crlf)
			{
				ok = processRequestLine(buf->peek(),crlf);
				if(ok)
				{
					request.setReceiveTime(time(0));
					buf->retrieveUntil(crlf + 2);
					state = kExpectHeaders;
				}
				else
				{
					hasMore = false;
				}
			}
			else
			{
				hasMore = false;
			}
		}
		else if(state == kExpectHeaders)
		{
			const char * crlf = buf->findCRLF();
			if(crlf)
			{
				const char *colon = std::find(buf->peek(), crlf, ':');
				if(colon != crlf)
				{
					request.addHeader(buf->peek(),colon,crlf);
				}
				else
				{
					state = kGotAll;
					hasMore = false;
				}
				buf->retrieveUntil(crlf + 2);
			}
			else
			{
				hasMore = false;
			}
		}
		else if(state == kExpectBody)
		{

		}
	}
	return ok;
}




























