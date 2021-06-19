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

/*
 * Programmer:  Robb Matzke <matzke@llnl.gov>
 *              Tuesday, October 12, 1999
 *
 * Purpose:	Creates an HDF5 file from a PDB file.  The raw data can be
 *		left in the PDB file, creating an HDF5 file that contains
 *		meta data that points into the PDB file.
 */
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "hdf5.h"
#include "pdb.h"
#include "score.h"

/*
 * libsilo renames all the PDB functions. However, this source files uses
 * their documented names, so we have #define's to translate them to Silo
 * terminology.
 */
#ifdef H5_HAVE_LIBSILO
#   define PD_open			lite_PD_open
#   define PD_close			lite_PD_close
#   define PD_ls			lite_PD_ls
#   define PD_cd			lite_PD_cd
#   define PD_inquire_entry		lite_PD_inquire_entry
#   define PD_read			lite_PD_read
#   define _PD_fixname			_lite_PD_fixname
#   define _PD_rl_defstr		_lite_PD_rl_defstr
#   define SC_free			lite_SC_free
#endif

static int verbose_g = 0;		/*verbose output?		*/
static int cached_g = 0;		/*use core file driver?		*/


/*-------------------------------------------------------------------------
 * Function:	usage
 *
 * Purpose:	Print a usage message.
 *
 * Return:	void
 *
 * Programmer:	Robb Matzke
 *              Tuesday, October 12, 1999
 *
 * Modifications:
 *
 *-------------------------------------------------------------------------
 */
static void
usage(const char *arg0)
{
    char	*progname;

    if ((progname=strrchr(arg0, '/')) && progname[1]) progname++;
    else progname = arg0;

    fprintf(stderr, "\
usage: %s [OPTIONS] [PDBFILE ...]\n\
   OPTIONS\n\
      -h, -?, --help   Print a usage message and exit\n\
      -c, --cached     Cache all data in memory before writing the output\n\
      -v, --verbose    Print the name of each object processed\n\
      -V, --version    Show the version number of this program\n\
\n\
   The options and PDB file names may be interspersed and are processed from\n\
   left to right.\n\
\n\
   The name of the HDF5 file is generated by taking the basename of the PDB\n\
   file and replacing the last extension (or appending if no extension) with\n\
   the characters \".h5\". For example, \"/tmp/test/eos.data\" would result\n\
   in an HDF5 file called \"eos.h5\" in the current directory.\n",
	    progname);
    
}


/*-------------------------------------------------------------------------
 * Function:	version
 *
 * Purpose:	Print the version number.
 *
 * Return:	void
 *
 * Programmer:	Robb Matzke
 *              Friday, October 15, 1999
 *
 * Modifications:
 *
 *-------------------------------------------------------------------------
 */
static void
version(const char *arg0)
{
    const char	*progname;

    if ((progname=strrchr(arg0, '/')) && progname[1]) progname++;
    else progname = arg0;
    print_version(progname);
}


/*-------------------------------------------------------------------------
 * Function:	fix_name
 *
 * Purpose:	Given a PDB file name create the corresponding HDF5 file
 *		name. This is done by taking the base name of the PDB file
 *		and replacing (or appending) the last extension with ".h5".
 *
 * Return:	Success:	HDF_NAME
 *
 *		Failure:	NULL
 *
 * Programmer:	Robb Matzke
 *              Tuesday, October 12, 1999
 *
 * Modifications:
 *
 *-------------------------------------------------------------------------
 */
static char *
fix_name(const char *pdb_name, char *hdf_name, size_t size)
{
    char	*s;
    const char	*ext;
    
    if (!pdb_name || !hdf_name) return NULL;
    if ((s=strrchr(pdb_name, '/'))) pdb_name = s;
    if (NULL==(ext=strrchr(pdb_name, '.'))) ext = pdb_name + strlen(pdb_name);
    if ((size_t)((ext-pdb_name)+4) > size) return NULL; /*overflow*/
    memcpy(hdf_name, pdb_name, ext-pdb_name);
    strcpy(hdf_name+(ext-pdb_name), ".h5");
    return hdf_name;
}


/*-------------------------------------------------------------------------
 * Function:	fix_type
 *
 * Purpose:	Given a PDB datatype return a corresponding hdf5 datatype.
 *		The hdf5 datatype should be closed when the caller is
 *		finished using it.
 *
 * Return:	Success:	HDF5 datatype
 *
 *		Failure:	negative
 *
 * Programmer:	Robb Matzke
 *              Tuesday, October 12, 1999
 *
 * Modifications:
 *
 *-------------------------------------------------------------------------
 */
static hid_t
fix_type(PDBfile *pdb, const char *s)
{
    hid_t	type = -1;
    defstr 	*d = _lite_PD_lookup_type((char*)s, pdb->chart);

    /* PDB checking */
    assert(d);
    assert(d->size>0);
    if (d->onescmp) return -1;
    
    
    if (!strcmp(s, "char")) {
	/*
	 * Character datatypes. Use whatever sign the native system uses by
	 * default.
	 */
	type = H5Tcopy(H5T_NATIVE_CHAR);
	
    } else if (!strcmp(s, "integer")) {
	/*
	 * Integer datatypes. PDB supports various sizes of signed or
	 * unsigned integers.
	 */
	type = H5Tcopy(d->unsgned?H5T_NATIVE_UINT:H5T_NATIVE_INT);
	H5Tset_size(type, d->size);
	H5Tset_precision(type, 8*d->size);
	assert(NORMAL_ORDER==d->order_flag || REVERSE_ORDER==d->order_flag);
	H5Tset_order(type,
		     NORMAL_ORDER==d->order_flag?H5T_ORDER_BE:H5T_ORDER_LE);
	
    } else if (!strcmp(s, "float") || !strcmp(s, "double")) {
	/*
	 * Floating-point datatypes
	 */
	size_t	nbits, spos, epos, esize, mpos, msize;

	type = H5Tcopy(H5T_NATIVE_FLOAT);
	H5Tset_size(type, d->size);
	H5Tset_precision(type, 8*d->size);
	assert(d->order);
	H5Tset_order(type, 1==d->order[0]?H5T_ORDER_BE:H5T_ORDER_LE);
	
	/*
	 * format[0] = # of bits per number                     
	 * format[1] = # of bits in exponent                    
	 * format[2] = # of bits in mantissa                    
	 * format[3] = start bit of sign                        
	 * format[4] = start bit of exponent                    
	 * format[5] = start bit of mantissa                    
	 * format[6] = high order mantissa bit (CRAY needs this)
	 * format[7] = bias of exponent
	 */
	assert(d->format && d->format[0] == 8*d->size);
	nbits = d->format[0];
	spos = nbits - (d->format[3]+1);
	esize = d->format[1];
	epos = nbits - (d->format[4]+esize);
	msize = d->format[2];
	mpos = nbits - (d->format[5]+msize);
	H5Tset_fields(type, spos, epos, esize, mpos, msize);
	H5Tset_ebias(type, d->format[7]);
    }
    return type;
}


/*-------------------------------------------------------------------------
 * Function:	fix_space
 *
 * Purpose:	Convert a PDB dimension list into an HDF5 data space.
 *
 * Return:	Success:	HDF5 data space
 *
 *		Failure:	negative
 *
 * Programmer:	Robb Matzke
 *              Tuesday, October 12, 1999
 *
 * Modifications:
 *
 *-------------------------------------------------------------------------
 */
static hid_t
fix_space(const dimdes *dim)
{
    hsize_t	size[H5S_MAX_RANK];
    int		rank;

    for (rank=0; rank<H5S_MAX_RANK && dim; rank++, dim=dim->next) {
	size[rank] = dim->number;
    }
    if (rank>=H5S_MAX_RANK) return -1;
    return H5Screate_simple(rank, size, NULL);
}


/*-------------------------------------------------------------------------
 * Function:	fix_external
 *
 * Purpose:	Sets the external file information for a dataset creation
 *		property list based on information from PDB.
 *
 * Return:	Success:	non-negative
 *
 *		Failure:	negative
 *
 * Programmer:	Robb Matzke
 *              Tuesday, October 12, 1999
 *
 * Modifications:
 *
 *-------------------------------------------------------------------------
 */
static int
fix_external(hid_t dcpl, const char *pdb_file_name, long nelmts,
	     hsize_t elmt_size, symblock *block)
{
    int		i;
    
    for (i=0; nelmts>0; i++) {
	hsize_t nbytes = block[i].number * elmt_size;
	H5Pset_external(dcpl, pdb_file_name, block[i].diskaddr, nbytes);
	nelmts -= block[i].number;
    }
    return 0;
}


/*-------------------------------------------------------------------------
 * Function:	traverse
 *
 * Purpose:	Traverse the current working directory of the PDB file.
 *
 * Return:	Success:	0
 *
 *		Failure:	-1
 *
 * Programmer:	Robb Matzke
 *              Tuesday, October 12, 1999
 *
 * Modifications:
 *
 *-------------------------------------------------------------------------
 */
static int
traverse(PDBfile *pdb, const char *pdb_file_name, hid_t hdf)
{
    int			nitems, i, in_subdir=FALSE;
    char		**list=NULL;
    hid_t		group=-1, h_type=-1, h_space=-1, dset=-1, dcpl=-1;
    hsize_t		elmt_size;
    const syment	*ep=NULL;

    if (NULL==(list=PD_ls(pdb, ".", NULL, &nitems))) {
	fprintf(stderr, "cannot obtain PDB directory contents\n");
	goto error;
    }

    for (i=0; i<nitems; i++) {
	ep = PD_inquire_entry(pdb, list[i], TRUE, NULL);
	if (verbose_g) {
	    printf("%s %s\n", _PD_fixname(pdb, list[i]), ep->type);
	    fflush(stdout);
	}
	

	if ('/'==list[i][strlen(list[i])-1]) {
	    /*
	     * This is a PDB directory. Make a corresponding HDF5 group and
	     * traverse into that PDB directory and HDF5 group
	     */
	    if ((group=H5Gcreate(hdf, list[i], 0))<0) {
		fprintf(stderr, "cannot create HDF group %s\n", list[i]);
		goto error;
	    }
	    if (!PD_cd(pdb, list[i])) {
		fprintf(stderr, "cannot cd into PDB directory %s\n", list[i]);
		goto error;
	    } else {
		in_subdir = TRUE;
	    }
	    
	    traverse(pdb, pdb_file_name, group);
	    if (!PD_cd(pdb, "..")) {
		fprintf(stderr, "cannot traverse out of PDB %s\n", list[i]);
		goto error;
	    }
	    H5Gclose(group);
	    
	} else {
	    /* This is some non-directory PDB object */

	    /* Create an HDF5 datatype from the PDB type */
	    if ((h_type=fix_type(pdb, ep->type))<0) {
		fprintf(stderr, "cannot create datatype for %s (%s)\n",
		       list[i], ep->type);
		continue;
	    }
	    elmt_size = H5Tget_size(h_type);

	    /* Create an HDF5 dataspace from the PDB dimensions */
	    if ((h_space=fix_space(ep->dimensions))<0) {
		fprintf(stderr, "cannot create datatype for %s\n", list[i]);
		continue;
	    }

	    /* Create pointers to the external PDB data */
	    dcpl = H5Pcreate(H5P_DATASET_CREATE);
	    fix_external(dcpl, pdb_file_name, ep->number, elmt_size,
			 ep->blocks);

	    /* Create the dataset */
	    if ((dset=H5Dcreate(hdf, list[i], h_type, h_space, dcpl))<0) {
		fprintf(stderr, "cannot create dataset for %s\n", list[i]);
	    }

	    H5Pclose(dcpl);
	    H5Dclose(dset);
	    H5Sclose(h_space);
	    H5Tclose(h_type);
	}
	
    }

    for (i=0; i<nitems; i++) {
	SC_free(list[i]);
    }
    SC_free(list);
    return 0;

 error:
    if (group>=0) H5Gclose(group);
    if (in_subdir) PD_cd(pdb, "..");
    if (list) {
	for (i=0; i<nitems; i++) {
	    SC_free(list[i]);
	}
	SC_free(list);
    }
    return -1;
}


/*-------------------------------------------------------------------------
 * Function:	main
 *
 * Purpose:	Create an HDF5 file from a PDB file.
 *
 * Return:	Success:	0
 *
 *		Failure:	non-zero
 *
 * Programmer:	Robb Matzke
 *              Tuesday, October 12, 1999
 *
 * Modifications:
 *
 *-------------------------------------------------------------------------
 */
int
main(int argc, char *argv[])
{
    int		argno;
    char	_hdf_name[512], *hdf_name, *pdb_name, *s;
    PDBfile	*pdb;
    hid_t	hdf, fapl;

    /* Print a help message if called with no arguments */
    if (1==argc) {
	usage(argv[0]);
	exit(1);
    }

    /* Process arguments in order; switches interspersed with files */
    for (argno=1; argno<argc; argno++) {
	if (!strcmp("--help", argv[argno])) {
	    usage(argv[0]);
	    exit(1);
	} else if (!strcmp("--verbose", argv[argno])) {
	    verbose_g++;
	} else if (!strcmp("--cached", argv[argno])) {
	    cached_g++;
	} else if (!strcmp("--version", argv[argno])) {
	    version(argv[0]);
	} else if ('-'==argv[argno][0] && '-'!=argv[argno][1]) {
	    for (s=argv[argno]+1; *s; s++) {
		switch (*s) {
		case '?':
		case 'h':		/*--help*/
		    usage(argv[0]);
		    exit(0);
		case 'c':		/*--cached*/
		    cached_g++;
		    break;
		case 'v':		/*--verbose*/
		    verbose_g++;
		    break;
		case 'V':		/*--version*/
		    version(argv[0]);
		    break;
		default:
		    usage(argv[0]);
		    exit(1);
		}
	    }
	} else if ('-'==argv[argno][0]) {
	    usage(argv[0]);
	    exit(1);
	} else {
	    /* This must be a file name. Process it. */
	    fapl = H5Pcreate(H5P_FILE_ACCESS);
	    if (cached_g) H5Pset_fapl_core(fapl, 1024*1024, TRUE);

	    pdb_name = argv[argno];
	    hdf_name = fix_name(argv[argno], _hdf_name, sizeof _hdf_name);
	    if (NULL==(pdb=PD_open(pdb_name, "r"))) {
		fprintf(stderr, "%s: unable to open PDB file\n", pdb_name);
		exit(1);
	    }
	    if ((hdf=H5Fcreate(hdf_name, H5F_ACC_TRUNC, H5P_DEFAULT,
			       fapl))<0) {
		fprintf(stderr, "%s: unable to open HDF file\n", hdf_name);
		exit(1);
	    }
	    H5Pclose(fapl);
	    
	    /*
	     * Traverse the PDB file to create the HDF5 file.
	     */
	    traverse(pdb, pdb_name, hdf);

	    /* Close the files */
	    if (!PD_close(pdb)) {
		fprintf(stderr, "%s: problems closing PDB file\n", pdb_name);
		exit(1);
	    }
	    if (H5Fclose(hdf)<0) {
		fprintf(stderr, "%s: problems closing HDF file\n", hdf_name);
		exit(1);
	    }
	}
    }
    return 0;
}