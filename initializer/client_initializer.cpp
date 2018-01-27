#include "client_initializer.h"

using namespace initializer;

client_initializer::client_initializer()
{
}


client_initializer::~client_initializer()
{
}

void client_initializer::run()
{
	std::cout << "client::initializer::check instance client\n";
	if (!check_one_instance())
		exit(0);
}

bool client_initializer::check_one_instance()
{
    HANDLE  m_hStartEvent = CreateEventW( NULL, FALSE, FALSE, L"Global\\CSAPP" );

    if(m_hStartEvent == NULL)
    {
    CloseHandle( m_hStartEvent ); 
        return false;
    }


    if ( GetLastError() == ERROR_ALREADY_EXISTS ) {

        CloseHandle( m_hStartEvent ); 
        m_hStartEvent = NULL;
        // already exist
        // send message from here to existing copy of the application
		std::cout << "Application already initialized!\n";
		return false;
    }
    // the only instance, start in a usual way
	std::cout << "Application not initialized!\n";
    return true;
}

client_initializer* client_initializer::get()
{
	static client_initializer* singleton = nullptr;
	if (!singleton)
		singleton = new client_initializer();
	return singleton;
}

