#pragma once
#include "http_base.h"

class requester : public http_base
{
public:
	requester();
	~requester();
	void run();
};

