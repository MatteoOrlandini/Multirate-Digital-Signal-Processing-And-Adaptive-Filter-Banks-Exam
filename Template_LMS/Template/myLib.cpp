#include "StdAfx.h"
#include "myLib.h"

void write_dat(char *name, double *data, int dim, char *save_name) {

	char name_a[MAX_FILE_NAME_LENGTH];
	memset(name_a,0,MAX_FILE_NAME_LENGTH*sizeof(char));
	strcpy(name_a,save_name);
	strcat(name_a,name);

	char *c;
	c=(char *)(void *)data;

	fstream File;
	// ios::app is the open mode "append" meaning
	// new data will be written to the end of the file.
	File.open(name_a, ios::app | ios::binary);
	File.write(c, dim * sizeof(double));
	File.close();
}

void read_dat(char *name, double *data, int dim) {
	char *c;
	c=(char *)(void *)data;

	ifstream File;
	File.open(name, ios::in | ios::binary);
	File.read(c, dim * sizeof(double));
	File.close();

}