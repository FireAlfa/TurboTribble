#ifndef __LOG_CPP__
#define __LOG_CPP__

#include "Globals.h"

#include "Application.h"
#include "ModuleEditor.h"



void TTLog(const char file[], int line, const char* format, ...)
{
	static char tmpString1[4096];
	static char tmpString2[4096];
	static va_list ap;

	// Construct the string from variable arguments
	va_start(ap, format);
	vsprintf_s(tmpString1, 4096, format, ap);
	va_end(ap);
	sprintf_s(tmpString2, 4096, "\n%s(%d) : %s", file, line, tmpString1);
	OutputDebugString(tmpString2);
	if (app != nullptr) app->editor->UpdateText(tmpString1);
}

#endif // !__LOG_CPP__