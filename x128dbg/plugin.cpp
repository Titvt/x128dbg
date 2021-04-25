#include "plugin.h"

int hViewManual{};
int hLocalManual{};
vector<string> instructions{};
string manualUrl{};
bool debugging{};
bool localManual{};
duint cursor{};

bool pluginInit(PLUG_INITSTRUCT* initStruct)
{
	return true;
}

void pluginStop()
{

}

void pluginSetup()
{
	dputs(u8">>> x128dbg v1 by:��������");

	initMenu();
	initCallback();
	initInstructions();
	initManual();
}

void initMenu()
{
	hViewManual = _plugin_hash(u8"�鿴�ֲ�", 13);
	hLocalManual = _plugin_hash(u8"ʹ�ñ����ֲ�", 19);

	_plugin_menuaddentry(hMenu, hViewManual, u8"�鿴�ֲ�");
	_plugin_menuentrysethotkey(pluginHandle, hViewManual, "/");
	_plugin_menuaddentry(hMenu, hLocalManual, u8"ʹ�ñ����ֲ�");
	_plugin_menuentrysethotkey(pluginHandle, hLocalManual, "Ctrl+Alt+/");
}

void initCallback()
{
	_plugin_registercallback(pluginHandle, CB_MENUENTRY, cbMenuEntry);
	_plugin_registercallback(pluginHandle, CB_CREATEPROCESS, cbEnterDebugging);
	_plugin_registercallback(pluginHandle, CB_ATTACH, cbEnterDebugging);
	_plugin_registercallback(pluginHandle, CB_EXITPROCESS, cbLeaveDebugging);
	_plugin_registercallback(pluginHandle, CB_DETACH, cbLeaveDebugging);
}

void initInstructions()
{
	fstream file{};
	string line{};

	file.open("x128dbg\\instructions.txt", ios::in);

	if (file.fail())
	{
		return;
	}

	while (!file.eof())
	{
		getline(file, line);

		instructions.push_back(line);
	}

	file.close();

	dprintf(u8">>> һ���� https://software.intel.com/content/www/us/en/develop/download/intel-64-and-ia-32-architectures-sdm-combined-volumes-1-2a-2b-2c-2d-3a-3b-3c-3d-and-4.html ��ȡ��%d���ٷ�ָ���ֲ��ĵ�\n", instructions.size());
}

void initManual()
{
	fstream file{};
	HANDLE handle{};

	file.open("x128dbg\\manual.txt", ios::in);

	if (file.fail())
	{
		manualUrl = "https://x128dbg.titvt.com/manual/";

		dprintf(u8">>> ʹ��Ĭ�������ֲ�Url��%s\n", manualUrl.c_str());
	}
	else
	{
		getline(file, manualUrl);

		file.close();

		dprintf(u8">>> ʹ���Զ��������ֲ�Url��%s\n", manualUrl.c_str());
	}

	handle = CreateFileA("x128dbg\\manual", GENERIC_ALL, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

	if (!handle || GetLastError() == ERROR_FILE_NOT_FOUND)
	{
		_plugin_menuentrysetchecked(pluginHandle, hLocalManual, false);

		dputs(u8">>> δ��⵽�����ֲᣬ��ʹ�������ֲ�");
	}
	else
	{
		localManual = true;

		_plugin_menuentrysetchecked(pluginHandle, hLocalManual, true);

		dputs(u8">>> ��⵽�����ֲᣬ��ʹ�ñ����ֲ�");
	}

	if (handle)
	{
		CloseHandle(handle);
	}
}

string getCurrentInstruction()
{
	SELECTIONDATA selectionData{};
	BASIC_INSTRUCTION_INFO basicInstructionInfo{};
	string instruction{};

	GuiSelectionGet(GUI_DISASSEMBLY, &selectionData);

	DbgDisasmFastAt(selectionData.start, &basicInstructionInfo);

	instruction = basicInstructionInfo.instruction;

	return instruction.substr(0, instruction.find(' '));
}

bool isIncluded(string instruction)
{
	for (auto i : instructions)
	{
		if (i == instruction)
		{
			return true;
		}
	}

	return false;
}

void cbMenuEntry(CBTYPE cbType, void* callbackInfo)
{
	int hEntry{};
	string instruction{};

	hEntry = ((PLUG_CB_MENUENTRY*)callbackInfo)->hEntry;

	if (hEntry == hViewManual)
	{
		if (debugging)
		{
			instruction = getCurrentInstruction();

			if (isIncluded(instruction))
			{
				if (localManual)
				{
					ShellExecuteA(nullptr, "open", ("plugins\\x128dbg\\manual\\" + instruction + "\\" + instruction + ".html").c_str(), nullptr, nullptr, 0);

					dprintf(u8">>> �鿴����ָ���ֲ᣺%s\n", instruction.c_str());
				}
				else
				{
					ShellExecuteA(nullptr, "open", (manualUrl + instruction + "/" + instruction + ".html").c_str(), nullptr, nullptr, 0);

					dprintf(u8">>> �鿴����ָ���ֲ᣺%s\n", instruction.c_str());
				}
			}
			else
			{
				dputs(u8">>> ��ǰָ��δʶ�𣬲鿴ʧ��");
			}

		}
	}
	else if (hEntry == hLocalManual)
	{
		if (localManual = !localManual)
		{
			dputs(u8">>> ʹ�ñ����ֲ�");
		}
		else
		{
			dputs(u8">>> ʹ�������ֲ�");
		}
	}
}

void cbEnterDebugging(CBTYPE cbType, void* callbackInfo)
{
	debugging = true;
}

void cbLeaveDebugging(CBTYPE cbType, void* callbackInfo)
{
	debugging = false;
}