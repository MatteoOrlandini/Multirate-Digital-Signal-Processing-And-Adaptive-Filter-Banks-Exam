#include <math.h>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <iomanip>
using namespace std;

#define MAX_FILE_NAME_LENGTH 255


void read_dat(char *name, double *data, int dim);
void write_dat(char *name, double *data, int dim, char*save_dir);

