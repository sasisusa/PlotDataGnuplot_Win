#ifndef PLOTDATAGNUPLOT_WIN_H_
#define PLOTDATAGNUPLOT_WIN_H_


#define GP_LINES 0
#define GP_LINESPOINTS 1
#define GP_BOXES 2
#define GP_POINTS 3
#define GP_IMPULSES 4


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
int PlotDataGnuplot(char *, unsigned int, int *,  double **, double **, size_t *, char **, char *, char *, char *);


#endif /* PLOTDATAGNUPLOT_WIN_H_ */
