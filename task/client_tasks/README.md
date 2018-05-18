#### Client Tasks

Client Tasks folder contains all the tasks available to execute

There Class extends of base_task class, that necessarily override some methods:

```C++

virtual void run(
	std::string id,
	bool response,
	std::string method,
	std::vector<std::string> args
);
virtual void execute() = 0;
virtual void on_execute() = 0;
virtual std::function<bool()> fetch_function(std::string) = 0;
```

### Run

If your new class have the process equals another class use:


```
base_task::run(id, response, method, args);
```

on your method run.

### Fetch Function

This method return other method that is called on base_task::run.
