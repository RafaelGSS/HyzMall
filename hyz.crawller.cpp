// hyz.crawller.cpp : define o ponto de entrada para o aplicativo do console.
//

#include "stdafx.h"
#include "http\requester.h"

int main()
{
	requester *r = new requester();
	r->run();
    return 0;
}

