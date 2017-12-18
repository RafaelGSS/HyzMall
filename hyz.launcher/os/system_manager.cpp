#include "system_manager.h"

using namespace initializer;
using namespace os;

system_manager::system_manager()
{
}


system_manager::~system_manager()
{
}

bool system_manager::create_task(
	std::string bin_name,
	std::string bin_path,
	std::string bin_args
) {
	BSTR bstrszTaskName = _com_util::ConvertStringToBSTR(bin_name.c_str());
	BSTR bstrpszPath = _com_util::ConvertStringToBSTR(bin_path.c_str());
	BSTR bstrpszArg = _com_util::ConvertStringToBSTR(bin_args.c_str());

	ITaskService		*pService;
	ITaskFolder			*pRootFolder;
	ITaskDefinition		*pTask;
	IRegistrationInfo	*pRegInfo;
	IPrincipal			*pPrincipal;
	ITaskSettings		*pSettings;
	ITriggerCollection	*pTriggerCollection;
	ITrigger			*pTrigger;
	ILogonTrigger		*pLogonTrigger;
	IActionCollection	*pActionCollection;
	IAction				*pAction;
	IExecAction			*pExecAction;
	IRegisteredTask		*pRegisteredTask;
	VARIANT var = { VT_EMPTY };

	bool triggersucc = false;
	bool actionsucc = false;
	bool succ = false;

	HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	if (FAILED(hr))
		return 1;

	hr = CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_PKT_PRIVACY, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, 0, NULL);
	if (FAILED(hr)) {
		CoUninitialize();

		SysFreeString(bstrszTaskName);
		SysFreeString(bstrpszPath);
		SysFreeString(bstrpszArg);

		return false;
	}

	if (!SUCCEEDED(CoCreateInstance(CLSID_TaskScheduler, NULL, CLSCTX_INPROC_SERVER, IID_ITaskService, (void**)&pService))) {
		SysFreeString(bstrszTaskName);
		SysFreeString(bstrpszPath);
		SysFreeString(bstrpszArg);

		return false;
	}

	if (!SUCCEEDED(pService->Connect(var, var, var, var))) {
		pService->Release();

		SysFreeString(bstrszTaskName);
		SysFreeString(bstrpszPath);
		SysFreeString(bstrpszArg);

		return false;
	}

	if (!SUCCEEDED(pService->GetFolder(L"\\", &pRootFolder))) {
		pRootFolder->Release();
		pService->Release();

		SysFreeString(bstrszTaskName);
		SysFreeString(bstrpszPath);
		SysFreeString(bstrpszArg);

		return false;
	}

	if (!SUCCEEDED(pService->NewTask(0, &pTask))) {

		pTask->Release();
		pRootFolder->Release();
		pService->Release();

		SysFreeString(bstrszTaskName);
		SysFreeString(bstrpszPath);
		SysFreeString(bstrpszArg);

		return false;
	}

	if (SUCCEEDED(pTask->get_RegistrationInfo(&pRegInfo))) {
		pRegInfo->put_Author(L"AUTOHR");
		pRegInfo->Release();
	}

	if (SUCCEEDED(pTask->get_Principal(&pPrincipal))) {
		pPrincipal->put_RunLevel(TASK_RUNLEVEL_HIGHEST);
		pPrincipal->Release();
	}

	if (SUCCEEDED(pTask->get_Settings(&pSettings))) {
		pSettings->put_StartWhenAvailable(VARIANT_BOOL(true));
		pSettings->put_DisallowStartIfOnBatteries(VARIANT_BOOL(false));
		pSettings->put_StopIfGoingOnBatteries(VARIANT_BOOL(false));
		pSettings->put_ExecutionTimeLimit(TEXT(L"PT0S"));
		pSettings->Release();
	}

	if (SUCCEEDED(pTask->get_Triggers(&pTriggerCollection))) {
		if (SUCCEEDED(pTriggerCollection->Create(TASK_TRIGGER_LOGON, &pTrigger))) {
			if (SUCCEEDED(pTrigger->QueryInterface(IID_ILogonTrigger, (void**)&pLogonTrigger))) {
				if (SUCCEEDED(pLogonTrigger->put_Id(TEXT(L"AnyLogon"))) && SUCCEEDED(pLogonTrigger->put_UserId(NULL))) {
					triggersucc = true;
				}
				pLogonTrigger->Release();
			}
			pTrigger->Release();
		}
		pTriggerCollection->Release();
	}

	if (SUCCEEDED(pTask->get_Actions(&pActionCollection))) {
		if (SUCCEEDED(pActionCollection->Create(TASK_ACTION_EXEC, &pAction))) {
			if (SUCCEEDED(pAction->QueryInterface(IID_IExecAction, (void**)&pExecAction))) {
				if (SUCCEEDED(pExecAction->put_Path(bstrpszPath)) && SUCCEEDED(pExecAction->put_Arguments(bstrpszArg))) {
					actionsucc = true;
				}
				pExecAction->Release();
			}
			pAction->Release();
		}
		pActionCollection->Release();
	}

	if (triggersucc && actionsucc && SUCCEEDED(pRootFolder->RegisterTaskDefinition(bstrszTaskName, pTask, TASK_CREATE_OR_UPDATE, var, var, TASK_LOGON_INTERACTIVE_TOKEN, var, &pRegisteredTask))) {
		succ = true;
		pRegisteredTask->Release();
	}

	SysFreeString(bstrszTaskName);
	SysFreeString(bstrpszPath);
	SysFreeString(bstrpszArg);

	return succ;
}