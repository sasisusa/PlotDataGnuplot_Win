#include <windows.h>
#include <stdio.h>

#include "PlotDataGnuplot_Win/PlotDataGnuplot_Win.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	int iRet;
	double adYValues[] = {141.0, 206.0, 357.0, 476.0, 586.0, 659.0};
	double adXValues[] = {20.0, 21.0, 22.0, 23.0, 24.0, 25.0};
	double adY2Values[] = {131.0, 244.0, 312.0, 466.0, 596.0, 686.0, 759.0, 814.0};
	double adX2Values[] = {19.0, 20.0, 21.0, 22.0, 23.0, 24.0, 25.0, 26.0};
	char *sPathGP = "C:\\Programs\\msys64\\mingw64\\bin\\gnuplot.exe";

	iRet = PlotDataGnuplot(sPathGP, 2, (int[]){GP_LINESPOINTS, GP_IMPULSES},
			(double*[]){adXValues, adX2Values},
			(double*[]){adYValues, adY2Values},
			(size_t[]){6, 8},
			(char*[]){"Device 1", "Device 2"},
			"Imput size", "Time [ms]", "E.g.: Execution time for ...");
	printf("%d\n", iRet);

	return 0;
}
