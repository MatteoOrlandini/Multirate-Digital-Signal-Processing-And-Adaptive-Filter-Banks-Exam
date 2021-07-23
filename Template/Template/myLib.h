#include <math.h>
#include "io.h"
#include <fstream>
#include <iostream>
using namespace std;

#define MAX_FILE_NAME_LENGTH 255


void read_dat(char *name, double *data, int dim);
void write_dat(char *name, double *data, int dim, char*save_dir);

