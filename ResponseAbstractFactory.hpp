#ifndef ResponseAbstractFactory_h__
#define ResponseAbstractFactory_h__
#include "Response.hpp"

class ResponseAbstractFactory
{
public:
	virtual Response* CreateResponse()=0;
};

class ResponseComingFactory : public ResponseAbstractFactory
{
	Response* CreateResponse()
	{
		return new ReceivedResponse;
	}
};
class ResponseGoingFactory : public ResponseAbstractFactory
{
	Response* CreateResponse()
	{
		return new SendableResponse;
	}
};

#endif