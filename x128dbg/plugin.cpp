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
	dputs(u8">>> x128dbg v1 by:古月浪子");

	initMenu();
	initCallback();
	initInstructions();
	initManual();
}

void initMenu()
{
	hViewManual = _plugin_hash(u8"查看手册", 13);
	hLocalManual = _plugin_hash(u8"使用本地手册", 19);

	_plugin_menuaddentry(hMenu, hViewManual, u8"查看手册");
	_plugin_menuentrysethotkey(pluginHandle, hViewManual, "/");
	_plugin_menuaddentry(hMenu, hLocalManual, u8"使用本地手册");
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

	dprintf(u8">>> 一共从 https://software.intel.com/content/www/us/en/develop/download/intel-64-and-ia-32-architectures-sdm-combined-volumes-1-2a-2b-2c-2d-3a-3b-3c-3d-and-4.html 获取了%d条官方指令手册文档\n", instructions.size());
}

void initManual()
{
	fstream file{};
	HANDLE handle{};

	file.open("x128dbg\\manual.txt", ios::in);

	if (file.fail())
	{
		manualUrl = "https://x128dbg.titvt.com/manual/";

		dprintf(u8">>> 使用默认网络手册Url：%s\n", manualUrl.c_str());
	}
	else
	{
		getline(file, manualUrl);

		file.close();

		dprintf(u8">>> 使用自定义网络手册Url：%s\n", manualUrl.c_str());
	}

	handle = CreateFileA("x128dbg\\manual", GENERIC_ALL, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

	if (!handle || GetLastError() == ERROR_FILE_NOT_FOUND)
	{
		_plugin_menuentrysetchecked(pluginHandle, hLocalManual, false);

		dputs(u8">>> 未检测到本地手册，将使用网络手册");
	}
	else
	{
		localManual = true;

		_plugin_menuentrysetchecked(pluginHandle, hLocalManual, true);

		dputs(u8">>> 检测到本地手册，将使用本地手册");
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

					dprintf(u8">>> 查看本地指令手册：%s\n", instruction.c_str());
				}
				else
				{
					ShellExecuteA(nullptr, "open", (manualUrl + instruction + "/" + instruction + ".html").c_str(), nullptr, nullptr, 0);

					dprintf(u8">>> 查看网络指令手册：%s\n", instruction.c_str());
				}
			}
			else
			{
				dputs(u8">>> 当前指令未识别，查看失败");
			}

		}
	}
	else if (hEntry == hLocalManual)
	{
		if (localManual = !localManual)
		{
			dputs(u8">>> 使用本地手册");
		}
		else
		{
			dputs(u8">>> 使用网络手册");
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