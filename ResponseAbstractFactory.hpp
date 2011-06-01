// using the abstract factory pattern to build differentiate Response objects
#ifndef ResponseAbstractFactory_h__
#define ResponseAbstractFactory_h__
#include "Response.hpp"
#include "Factory.h"

// new template based Abstract Factory
#define TYPELIST_ONE(T1)  cspp51045::Typelist<T1, cspp51045::NullType>
typedef cspp51045::AbstractFactory <TYPELIST_ONE(Response)> ResponseAbstractFactory;
typedef cspp51045::ConcreteFactory <ResponseAbstractFactory,TYPELIST_ONE(ReceivedResponse)> ResponseComingFactory;
typedef cspp51045::ConcreteFactory <ResponseAbstractFactory,TYPELIST_ONE(SendableResponse)> ResponseGoingFactory;

// old standard class based Abstract Factory
/*
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
*/
#endif