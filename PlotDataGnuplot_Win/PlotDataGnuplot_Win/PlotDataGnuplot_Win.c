#if defined(_WIN32) || defined(_WIN64)

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>


#include "PlotDataGnuplot_Win.h"


#define STRING_BUF_LEN 4096


typedef struct PROCESSPIPEtag{
	HANDLE hStdInRd;
	HANDLE hStdInWr;
	HANDLE hStdOutRd;
	HANDLE hStdOutWr;
	PROCESS_INFORMATION piProcInfo;
}PROCESSPIPE;


int CloseProcessPipe(PROCESSPIPE *pPP)
{
	HANDLE hHeap;
	int iRet;

	if(pPP){
		hHeap = GetProcessHeap();
		if(hHeap){
			iRet = 1;
			if(pPP->hStdInRd){
				iRet &= CloseHandle(pPP->hStdInRd);
			}
			if(pPP->hStdInWr){
				iRet &= CloseHandle(pPP->hStdInWr);
			}
			if(pPP->hStdOutRd){
				iRet &= CloseHandle(pPP->hStdOutRd);
			}
			if(pPP->hStdOutWr){
				iRet &= CloseHandle(pPP->hStdOutWr);
			}
			if(pPP->piProcInfo.hProcess){
				iRet &= CloseHandle(pPP->piProcInfo.hProcess);
			}
			if(pPP->piProcInfo.hThread){
				iRet &= CloseHandle(pPP->piProcInfo.hThread);
			}
			ZeroMemory(pPP, sizeof(*pPP));
			iRet &= HeapFree(hHeap, 0, pPP);

			return !iRet;
		}
	}

	return 1;
}


////////////////////////////////////////////////////////////////////////////
//	CreateProcessPipe
//
//	e.g.: sCmdLine: "\"C:\\Windows\\System32\\notepad.exe\""
//		with arg: "\"C:\\Programme\\gnuplot\\bin\\gnuplot.exe\" -persist"
//
//  Returns:
//		NULL		on failure
//
PROCESSPIPE *CreateProcessPipe(char *sCmdLine)
{
	HANDLE hHeap;
	PROCESSPIPE *pProcPipe;
	STARTUPINFO siStartInfo;
	SECURITY_ATTRIBUTES saAttr;


	hHeap = GetProcessHeap();
	if(!hHeap){
		return NULL;
	}

	pProcPipe = HeapAlloc(hHeap, HEAP_ZERO_MEMORY, sizeof(*pProcPipe));
	if(!pProcPipe){
		return NULL;
	}

	ZeroMemory(&saAttr, sizeof(saAttr));
	saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
	saAttr.bInheritHandle = TRUE;
	saAttr.lpSecurityDescriptor = NULL;
	if(CreatePipe(&pProcPipe->hStdOutRd, &pProcPipe->hStdOutWr, &saAttr, 0)){
		if(SetHandleInformation(pProcPipe->hStdOutRd, HANDLE_FLAG_INHERIT, 0)){
			if(CreatePipe(&pProcPipe->hStdInRd, &pProcPipe->hStdInWr, &saAttr, 0)){
				if(SetHandleInformation(pProcPipe->hStdInWr, HANDLE_FLAG_INHERIT, 0)){
					ZeroMemory(&pProcPipe->piProcInfo, sizeof(pProcPipe->piProcInfo));
					ZeroMemory(&siStartInfo, sizeof(siStartInfo));
					siStartInfo.cb = sizeof(STARTUPINFO);
					siStartInfo.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
					siStartInfo.hStdError = pProcPipe->hStdOutWr;
					siStartInfo.hStdOutput = pProcPipe->hStdOutWr;
					siStartInfo.hStdInput = pProcPipe->hStdInRd;
					siStartInfo.wShowWindow = SW_SHOW;
					if(CreateProcess(NULL, sCmdLine, NULL, NULL, TRUE, 0, NULL, NULL, &siStartInfo, &pProcPipe->piProcInfo)){
						return pProcPipe;
					}
				}
			}
		}
	}

	CloseProcessPipe(pProcPipe);

	return NULL;
}


static const char *PlotStyleString(int iPlotStyle)
{
	const char *sPlotStyle;

	switch(iPlotStyle){
	case GP_LINESPOINTS:
		sPlotStyle = "lp";
		break;
	case GP_BOXES:
		sPlotStyle = "boxes";
		break;
	case GP_POINTS:
		sPlotStyle = "points";
		break;
	case GP_IMPULSES:
		sPlotStyle = "impulses";
		break;
	case GP_LINES:
	default:
		sPlotStyle = "lines";
		break;
	}

	return sPlotStyle;
}


////////////////////////////////////////////////////////////////////////////
//	PlotDataGnuplot
//
//	@pzN: lenght of data, number of elements
//
//	e.g.:
//	PlotDataGnuplot("D:\\Programme\\gnuplot\\bin\\gnuplot.exe",
//			3,
//			(int[]){GP_LINESPOINTS, GP_IMPULSES, GP_POINTS},
//			(double*[]){pdX0, pdX1, pdX2},
//			(double*[]){pdY0, pdY1, pdY2},
//			(size_t[]){10, 3, sizeof(pdX2)/sizeof(*pdX2)},
//			(char*[]){"Data 0", "Data 1", "Data 2"},
//			"x", "y", "Titel");
//
//	PlotDataGnuplot("D:\\Programme\\gnuplot\\bin\\gnuplot.exe",
//			3,
//			NULL,
//			NULL,
//			(double*[]){pdY0, pdY1, pdY2},
//			(size_t[]){10, 3, sizeof(pdX2)/sizeof(*pdX2)},
//			NULL,
//			NULL, NULL, NULL);
//
//  Returns:
//		0		on success
//
int PlotDataGnuplot(char *sPathGP, unsigned int uiN, int *piPlotStyle, double **ppdDataX, double **ppdDataY, size_t *pzN, char **psData, char *sXLabel, char *sYLabel, char *sTitle)
{
	unsigned int ui, uiDatRun;
	int i, iRet;
	DWORD dw;
	PROCESSPIPE *pProcPipe;
	char *s, sBuf[STRING_BUF_LEN];

	if(!(sPathGP && uiN && ppdDataY && pzN)){
		return 1;
	}

	i = snprintf(sBuf, sizeof(sBuf), "\"%s\" -persist", sPathGP);
	if(i < 22 || i > sizeof(sBuf)/sizeof(*sBuf)){
		return 1;
	}
	pProcPipe = CreateProcessPipe(sBuf);
	if(!pProcPipe){
		return 1;
	}
	CloseHandle(pProcPipe->piProcInfo.hProcess);
	pProcPipe->piProcInfo.hProcess = NULL;
	CloseHandle(pProcPipe->piProcInfo.hThread);
	pProcPipe->piProcInfo.hThread = NULL;

	i = snprintf(sBuf, sizeof(sBuf), "reset\nset key %s\nset xlabel \"%s\"\nset ylabel \"%s\"\nset title \"%s\"\nset grid lw 1\n", psData?"outside":"off", sXLabel?sXLabel:"", sYLabel?sYLabel:"", sTitle?sTitle:"");
	if(i > 0 && i < sizeof(sBuf)){
		iRet = WriteFile(pProcPipe->hStdInWr, sBuf, i, &dw, NULL);
	}
	else{
		iRet = 0;
	}

	i = snprintf(sBuf, sizeof(sBuf), "plot '-' u 1%s w %s lw 2 title \"%.64s\"", ppdDataX?":2":"", PlotStyleString(piPlotStyle?piPlotStyle[0]:-1), psData?psData[0]:" ");
	if(i > 0 && i < sizeof(sBuf)){
		iRet &= WriteFile(pProcPipe->hStdInWr, sBuf, i, &dw, NULL);
	}
	else{
		iRet = 0;
	}
	for(ui=1; ui<uiN && iRet; ++ui){
		i = snprintf(sBuf, sizeof(sBuf), ", '-' u 1%s w %s lw 2 title \"%.64s\"", ppdDataX?":2":"", PlotStyleString(piPlotStyle?piPlotStyle[ui]:-1), psData?psData[ui]:" ");
		if(i > 0 && i < sizeof(sBuf)){
			iRet &= WriteFile(pProcPipe->hStdInWr, sBuf, i, &dw, NULL);
		}
		else{
			iRet = 0;
		}
	}
	s = "\n";
	iRet &= WriteFile(pProcPipe->hStdInWr, s, strlen(s), &dw, NULL);

	for(ui=0; ui<uiN && iRet; ++ui){
		for(uiDatRun=0; uiDatRun<pzN[ui] && iRet; ++uiDatRun){
			if(ppdDataX){
				i = snprintf(sBuf, sizeof(sBuf), "%e %e\n", ppdDataX[ui][uiDatRun], ppdDataY[ui][uiDatRun]);
			}
			else{
				i = snprintf(sBuf, sizeof(sBuf), "%e\n", ppdDataY[ui][uiDatRun]);
			}

			if(i > 0 && i < sizeof(sBuf)){
				iRet &= WriteFile(pProcPipe->hStdInWr, sBuf, i, &dw, NULL);
			}
			else{
				iRet = 0;
			}
		}
		s = "e\n";
		iRet &= WriteFile(pProcPipe->hStdInWr, s, strlen(s), &dw, NULL);
	}


	return CloseProcessPipe(pProcPipe) || !iRet;
}


#endif
