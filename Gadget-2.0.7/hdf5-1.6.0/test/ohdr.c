/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright by the Board of Trustees of the University of Illinois.         *
 * All rights reserved.                                                      *
 *                                                                           *
 * This file is part of HDF5.  The full HDF5 copyright notice, including     *
 * terms governing use, modification, and redistribution, is contained in    *
 * the files COPYING and Copyright.html.  COPYING can be found at the root   *
 * of the source code distribution tree; Copyright.html can be found at the  *
 * root level of an installed copy of the electronic HDF5 document set and   *
 * is linked from the top-level documents page.  It can also be found at     *
 * http://hdf.ncsa.uiuc.edu/HDF5/doc/Copyright.html.  If you do not have     *
 * access to either file, you may request a copy from hdfhelp@ncsa.uiuc.edu. *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* Programmer:  Robb Matzke <matzke@llnl.gov>
 *              Tuesday, November 24, 1998
 */
#include "h5test.h"
#include "H5Iprivate.h"
/*
 * This file needs to access private datatypes from the H5O package.
 */
#define H5O_PACKAGE
#include "H5Opkg.h"

/*
 * This file needs to access private datatypes from the H5G package.
 */
#define H5G_PACKAGE
#include "H5Gpkg.h"

const char *FILENAME[] = {
    "ohdr",
    NULL
};

/* The tbogus.h5 is generated from gen_bogus.c in HDF5 'test' directory.
 * To get this data file, define H5O_ENABLE_BOGUS in src/H5Oprivate, rebuild
 * the library and simply compile gen_bogus.c with that HDF5 library and run it. */
#define FILE_BOGUS "tbogus.h5"


/*-------------------------------------------------------------------------
 * Function:	main
 *
 * Purpose:	
 *
 * Return:	Success:	
 *
 *		Failure:	
 *
 * Programmer:	Robb Matzke
 *              Tuesday, November 24, 1998
 *
 * Modifications:
 *
 *-------------------------------------------------------------------------
 */
int
main(void)
{
    hid_t	fapl=-1, file=-1;
    hid_t	dset=-1;
    H5F_t	*f=NULL;
    char	filename[1024];
    H5G_entry_t	oh_ent;
    H5O_stab_t	stab, ro;
    int		i;

    /* Reset library */
    h5_reset();
    fapl = h5_fileaccess();
    h5_fixname(FILENAME[0], fapl, filename, sizeof filename);
    if ((file=H5Fcreate(filename, H5F_ACC_TRUNC, H5P_DEFAULT, fapl))<0)
	goto error;
    if (NULL==(f=H5I_object(file))) {
	H5Eprint(stdout);
	goto error;
    }

    /*
     * Test object header creation
     */
    TESTING("object header creation");
    HDmemset(&oh_ent,0,sizeof(H5G_entry_t));
    if (H5O_create(f, H5P_DATASET_XFER_DEFAULT, 64, &oh_ent/*out*/)<0) {
	H5_FAILED();
	H5Eprint(stdout);
	goto error;
    }
    PASSED();

    /* create a new message */
    TESTING("message creation");
    stab.btree_addr = 11111111;
    stab.heap_addr = 22222222;
    if (H5O_modify(&oh_ent, H5O_STAB_ID, H5O_NEW_MESG, 0, 1, &stab, H5P_DATASET_XFER_DEFAULT)<0) {
	H5_FAILED();
	H5Eprint(stdout);
	goto error;
    }
    if (H5AC_flush(f, H5P_DATASET_XFER_DEFAULT, NULL, HADDR_UNDEF, TRUE)<0) {
	H5_FAILED();
	H5Eprint(stdout);
	goto error;
    }
    if (NULL==H5O_read(&oh_ent, H5O_STAB_ID, 0, &ro, H5P_DATASET_XFER_DEFAULT)) {
	H5_FAILED();
	H5Eprint(stdout);
	goto error;
    }
    if (H5F_addr_ne(ro.btree_addr, stab.btree_addr) ||
	H5F_addr_ne(ro.heap_addr, stab.heap_addr)) {
	H5_FAILED();
	HDfprintf(stdout, "    got: {%a, %a}\n",
		  ro.btree_addr, ro.heap_addr);
	HDfprintf(stdout, "    ans: {%a, %a}\n",
		  stab.btree_addr, stab.heap_addr);
	goto error;
    }
    PASSED();

    /*
     * Test modification of an existing message.
     */
    TESTING("message modification");
    stab.btree_addr = 33333333;
    stab.heap_addr = 44444444;
    if (H5O_modify(&oh_ent, H5O_STAB_ID, 0, 0, 1, &stab, H5P_DATASET_XFER_DEFAULT)<0) {
	H5_FAILED();
	H5Eprint(stdout);
	goto error;
    }
    if (H5AC_flush(f, H5P_DATASET_XFER_DEFAULT, NULL, HADDR_UNDEF, TRUE)<0) {
	H5_FAILED();
	H5Eprint(stdout);
	goto error;
    }
    if (NULL==H5O_read(&oh_ent, H5O_STAB_ID, 0, &ro, H5P_DATASET_XFER_DEFAULT)) {
	H5_FAILED();
	H5Eprint(stdout);
	goto error;
    }
    if (H5F_addr_ne(ro.btree_addr, stab.btree_addr) ||
	H5F_addr_ne(ro.heap_addr, stab.heap_addr)) {
	H5_FAILED();
	HDfprintf(stdout, "    got: {%a, %a}\n",
		  ro.btree_addr, ro.heap_addr);
	HDfprintf(stdout, "    ans: {%a, %a}\n",
		  stab.btree_addr, stab.heap_addr);
	goto error;
    }
    PASSED();


    /*
     * Test creation of a second message of the same type.
     */
    TESTING("duplicate message creation");
    stab.btree_addr = 55555555;
    stab.heap_addr = 66666666;
    if (H5O_modify(&oh_ent, H5O_STAB_ID, H5O_NEW_MESG, 0, 1, &stab, H5P_DATASET_XFER_DEFAULT)<0) {
	H5_FAILED();
	H5Eprint(stdout);
	goto error;
    }
    if (H5AC_flush(f, H5P_DATASET_XFER_DEFAULT, NULL, HADDR_UNDEF, TRUE)<0) {
	H5_FAILED();
	H5Eprint(stdout);
	goto error;
    }
    if (NULL==H5O_read(&oh_ent, H5O_STAB_ID, 1, &ro, H5P_DATASET_XFER_DEFAULT)) {
	H5_FAILED();
	H5Eprint(stdout);
	goto error;
    }
    if (H5F_addr_ne(ro.btree_addr, stab.btree_addr) ||
	H5F_addr_ne(ro.heap_addr, stab.heap_addr)) {
	H5_FAILED();
	HDfprintf(stdout, "    got: {%a, %a}\n",
		  ro.btree_addr, ro.heap_addr);
	HDfprintf(stdout, "    ans: {%a, %a}\n",
		  stab.btree_addr, stab.heap_addr);
	goto error;
    }
    PASSED();
	
    /*
     * Test modification of the second message with a symbol table.
     */
    TESTING("duplicate message modification");
    stab.btree_addr = 77777777;
    stab.heap_addr = 88888888;
    if (H5O_modify(&oh_ent, H5O_STAB_ID, 1, 0, 1, &stab, H5P_DATASET_XFER_DEFAULT)<0) {
	H5_FAILED();
	H5Eprint(stdout);
	goto error;
    }
    if (H5AC_flush(f, H5P_DATASET_XFER_DEFAULT, NULL, HADDR_UNDEF, TRUE)<0) {
	H5_FAILED();
	H5Eprint(stdout);
	goto error;
    }
    if (NULL==H5O_read(&oh_ent, H5O_STAB_ID, 1, &ro, H5P_DATASET_XFER_DEFAULT)) {
	H5_FAILED();
	H5Eprint(stdout);
	goto error;
    }
    if (H5F_addr_ne(ro.btree_addr, stab.btree_addr) ||
	H5F_addr_ne(ro.heap_addr, stab.heap_addr)) {
	H5_FAILED();
	HDfprintf(stdout, "    got: {%a, %a}\n",
		  ro.btree_addr, ro.heap_addr);
	HDfprintf(stdout, "    ans: {%a, %a}\n",
		  stab.btree_addr, stab.heap_addr);
	goto error;
    }
    PASSED();

    /*
     * Test creation of a bunch of messages one after another to see
     * what happens when the object header overflows in core.
     */
    TESTING("object header overflow in memory");
    for (i=0; i<40; i++) {
        stab.btree_addr = (i+1)*1000+1;
        stab.heap_addr = (i+1)*1000+2;
        if (H5O_modify(&oh_ent, H5O_STAB_ID, H5O_NEW_MESG, 0, 1, &stab, H5P_DATASET_XFER_DEFAULT)<0) {
	    H5_FAILED();
	    H5Eprint(stdout);
	    goto error;
	}
    }
    if (H5AC_flush(f, H5P_DATASET_XFER_DEFAULT, NULL, HADDR_UNDEF, TRUE)<0) {
	H5_FAILED();
	H5Eprint(stdout);
	goto error;
    }
    PASSED();

    /*
     * Test creation of a bunch of messages one after another to see
     * what happens when the object header overflows on disk.
     */
    TESTING("object header overflow on disk");
    for (i=0; i<10; i++) {
        stab.btree_addr = (i + 1) * 1000 + 10;
        stab.heap_addr = (i + 1) * 1000 + 20;
        if (H5O_modify(&oh_ent, H5O_STAB_ID, H5O_NEW_MESG, 0, 1, &stab, H5P_DATASET_XFER_DEFAULT)<0) {
	    H5_FAILED();
	    H5Eprint(stdout);
	    goto error;
	}
        if (H5AC_flush(f, H5P_DATASET_XFER_DEFAULT, NULL, HADDR_UNDEF, TRUE)<0) {
	    H5_FAILED();
	    H5Eprint(stdout);
	    goto error;
	}
    }
    PASSED();

    /*
     * Delete all symbol table messages.
     */
    TESTING("message deletion");
    if (H5O_remove(&oh_ent, H5O_STAB_ID, H5O_ALL, H5P_DATASET_XFER_DEFAULT)<0) {
	H5_FAILED();
	H5Eprint(stdout);
	goto error;
    }
    if (H5O_read(&oh_ent, H5O_STAB_ID, 0, &ro, H5P_DATASET_XFER_DEFAULT)) {
	H5_FAILED();
	puts("    H5O_read() should have failed but didn't");
	H5Eclear();
	goto error;
    }
    PASSED();
    

    /* release resources */
    TESTING("object header closing");
    if (H5O_close(&oh_ent)<0) {
	H5_FAILED();
	H5Eprint(stdout);
	goto error;
    }
    if (H5Fclose(file)<0) goto error;
    PASSED();

    /* Test reading dataset with undefined object header message */
    TESTING("reading object with unknown header message");
    {
        char testfile[512]="";
        char *srcdir = getenv("srcdir");

        /* Build path to test file */
        if (srcdir && ((HDstrlen(srcdir) + HDstrlen(FILE_BOGUS) + 1) < sizeof(testfile))){
            HDstrcpy(testfile, srcdir);
            HDstrcat(testfile, "/");
        }
        HDstrcat(testfile, FILE_BOGUS);

        if ((file=H5Fopen(testfile, H5F_ACC_RDONLY, fapl))<0)
            goto error;

        /* Open the dataset with the unknown header message (generated with gen_bogus.c) */
        if((dset=H5Dopen(file,"/Dataset1"))<0)
            goto error;
        if (H5Dclose(dset)<0) goto error;

        if (H5Fclose(file)<0) goto error;
    }
    PASSED();

    puts("All object header tests passed.");
    h5_cleanup(FILENAME, fapl);
    return 0;

 error:
    H5E_BEGIN_TRY {
	H5Fclose(file);
    } H5E_END_TRY;
    return 1;
}
