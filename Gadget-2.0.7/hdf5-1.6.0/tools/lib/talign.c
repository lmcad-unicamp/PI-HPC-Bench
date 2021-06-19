/*
 * Small program to illustrate the "misalignment" of members within a compound
 * datatype, in a datatype fixed by h5tools_fixtype().
 */
#include <string.h>
#include <stdlib.h>
#include <unistd.h>	/* Required for unlink() */

#include "hdf5.h"
#include "h5tools.h"

const char *fname = "talign.h5";
const char *setname = "align";

/*
 * This program assumes that there is no extra space between the members 'Ok'
 * and 'Not Ok', (there shouldn't be because they are of the same atomic type
 * H5T_NATIVE_FLOAT, and they are placed within the compound next to one
 * another per construction)
 */

int main(void)
{
	hid_t fil,spc,set;
	hid_t cs6, cmp, fix;
	hid_t cmp1, cmp2, cmp3;
	hid_t plist;
	hid_t array_dt;

	hsize_t dim[2];
	hsize_t cdim[4];

	char string5[5];
	float fok[2] = {1234., 2341.};
	float fnok[2] = {5678., 6785.};
	float *fptr;

	char *data;
	char *mname;

	int result = 0;

	printf("%-70s", "Testing alignment in compound datatypes");

	strcpy(string5, "Hi!");
	unlink(fname);
	fil = H5Fcreate(fname, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);

	if (fil < 0) {
		puts("*FAILED*");
		return 1;
	}

	H5E_BEGIN_TRY {
		H5Gunlink(fil, setname);
	} H5E_END_TRY;

	cs6 = H5Tcopy(H5T_C_S1);
	H5Tset_size(cs6, sizeof(string5));
	H5Tset_strpad(cs6, H5T_STR_NULLPAD);

	cmp = H5Tcreate(H5T_COMPOUND, sizeof(fok) + sizeof(string5) + sizeof(fnok));
	H5Tinsert(cmp, "Awkward length", 0, cs6);

	cdim[0] = sizeof(fok) / sizeof(float);
    array_dt=H5Tarray_create(H5T_NATIVE_FLOAT,1,cdim,NULL);
	H5Tinsert(cmp, "Ok", sizeof(string5), array_dt);
    H5Tclose(array_dt);

	cdim[0] = sizeof(fnok) / sizeof(float);
    array_dt=H5Tarray_create(H5T_NATIVE_FLOAT,1,cdim,NULL);
	H5Tinsert(cmp, "Not Ok", sizeof(fok) + sizeof(string5), array_dt);
    H5Tclose(array_dt);

	fix = h5tools_fixtype(cmp);

	cmp1 = H5Tcreate(H5T_COMPOUND, sizeof(fok));

	cdim[0] = sizeof(fok) / sizeof(float);
    array_dt=H5Tarray_create(H5T_NATIVE_FLOAT,1,cdim,NULL);
	H5Tinsert(cmp1, "Ok", 0, array_dt);
    H5Tclose(array_dt);

	cmp2 = H5Tcreate(H5T_COMPOUND, sizeof(string5));
	H5Tinsert(cmp2, "Awkward length", 0, cs6);

	cmp3 = H5Tcreate(H5T_COMPOUND, sizeof(fnok));

	cdim[0] = sizeof(fnok) / sizeof(float);
    array_dt=H5Tarray_create(H5T_NATIVE_FLOAT,1,cdim,NULL);
	H5Tinsert(cmp3, "Not Ok", 0, array_dt);
    H5Tclose(array_dt);

	plist = H5Pcreate(H5P_DATASET_XFER);
	H5Pset_preserve(plist, 1);

	/*
	 * Create a small dataset, and write data into it we write each field
	 * in turn so that we are avoid alignment issues at this point
	 */
	dim[0] = 1;
	spc = H5Screate_simple(1, dim, NULL);
	set = H5Dcreate(fil, setname, cmp, spc, H5P_DEFAULT);

	H5Dwrite(set, cmp1, spc, H5S_ALL, plist, fok);
	H5Dwrite(set, cmp2, spc, H5S_ALL, plist, string5);
	H5Dwrite(set, cmp3, spc, H5S_ALL, plist, fnok);

	H5Dclose(set);

	/* Now open the set, and read it back in */
	data = malloc(H5Tget_size(fix));

	if (!data) {
		perror("malloc() failed");
		abort();
	}

	set = H5Dopen(fil, setname);

	H5Dread(set, fix, spc, H5S_ALL, H5P_DEFAULT, data);
	fptr = (float *)(data + H5Tget_member_offset(fix, 1));

	if (fok[0] != fptr[0] || fok[1] != fptr[1]
			|| fnok[0] != fptr[2] || fnok[1] != fptr[3]) {
		result = 1;
		printf("%14s (%2d) %6s = %s\n",
			mname = H5Tget_member_name(fix, 0), (int)H5Tget_member_offset(fix,0),
			string5, (char *)(data + H5Tget_member_offset(fix, 0)));
		free(mname);
		fptr = (float *)(data + H5Tget_member_offset(fix, 1));
		printf("Data comparison:\n"
			"%14s (%2d) %6f = %f\n"
			"                    %6f = %f\n",
			mname = H5Tget_member_name(fix, 1), (int)H5Tget_member_offset(fix,1),
			fok[0], fptr[0],
			fok[1], fptr[1]);
		free(mname);
		fptr = (float *)(data + H5Tget_member_offset(fix, 2));
		printf("%14s (%2d) %6f = %f\n"
			"                    %6f = %6f\n",
			mname = H5Tget_member_name(fix, 2), (int)H5Tget_member_offset(fix,2),
			fnok[0], fptr[0],
			fnok[1], fptr[1]);
		free(mname);

		fptr = (float *)(data + H5Tget_member_offset(fix, 1));
		printf("\n"
			"Short circuit\n"
			"                    %6f = %f\n"
			"                    %6f = %f\n"
			"                    %6f = %f\n"
			"                    %6f = %f\n",
			fok[0], fptr[0],
			fok[1], fptr[1],
			fnok[0], fptr[2],
			fnok[1], fptr[3]);
		puts("*FAILED*");
	} else {
		puts(" PASSED");
	}

	free(data);
	H5Sclose(spc);
	H5Tclose(cmp);
	H5Tclose(cmp1);
	H5Tclose(cmp2);
	H5Tclose(cmp3);
	H5Pclose(plist);
	H5Fclose(fil);
	unlink(fname);
	fflush(stdout);
	return result;
}