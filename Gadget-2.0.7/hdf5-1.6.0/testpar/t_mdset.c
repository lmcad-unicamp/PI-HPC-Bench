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

/* $Id: t_mdset.c,v 1.25 2003/06/09 19:18:45 koziol Exp $ */

#include "testphdf5.h"

#define DIM  2
#define SIZE 32
#define NDATASET 4
#define GROUP_DEPTH 128
enum obj_type { is_group, is_dset };
 
void write_dataset(hid_t, hid_t, hid_t);
int  read_dataset(hid_t, hid_t, hid_t);
void create_group_recursive(hid_t, hid_t, hid_t, int);
void recursive_read_group(hid_t, hid_t, hid_t, int);
void group_dataset_read(hid_t fid, int mpi_rank, int m);
void write_attribute(hid_t, int, int);
int  read_attribute(hid_t, int, int);
int  check_value(DATATYPE *, DATATYPE *);
void get_slab(hssize_t[], hsize_t[], hsize_t[], hsize_t[]);

/*
 * Example of using PHDF5 to create ndatasets datasets.  Each process write
 * a slab of array to the file.
 */
void multiple_dset_write(char *filename, int ndatasets)
{
    int i, j, n, mpi_size, mpi_rank;
    hid_t iof, plist, dataset, memspace, filespace;
    hid_t dcpl;                         /* Dataset creation property list */
    hbool_t use_gpfs = FALSE;           /* Use GPFS hints */
    hssize_t chunk_origin [DIM];
    hsize_t chunk_dims [DIM], file_dims [DIM];
    hsize_t count[DIM]={1,1};
    double outme [SIZE][SIZE];
    double fill=1.0;                    /* Fill value */
    char dname [100];
    herr_t ret;

    MPI_Comm_rank (MPI_COMM_WORLD, &mpi_rank);
    MPI_Comm_size (MPI_COMM_WORLD, &mpi_size);

    VRFY((mpi_size <= SIZE), "mpi_size <= SIZE");

    plist = create_faccess_plist(MPI_COMM_WORLD, MPI_INFO_NULL, facc_type, use_gpfs);
    VRFY((plist>=0), "create_faccess_plist succeeded");
    iof = H5Fcreate (filename, H5F_ACC_TRUNC, H5P_DEFAULT, plist);
    VRFY((iof>=0), "H5Fcreate succeeded");
    ret = H5Pclose (plist);
    VRFY((ret>=0), "H5Pclose succeeded");

    /* decide the hyperslab according to process number. */
    get_slab(chunk_origin, chunk_dims, count, file_dims);  

    memspace = H5Screate_simple (DIM, chunk_dims, NULL);
    filespace = H5Screate_simple (DIM, file_dims, NULL);
    ret = H5Sselect_hyperslab (filespace, H5S_SELECT_SET, chunk_origin, chunk_dims, count, chunk_dims);
    VRFY((ret>=0), "mdata hyperslab selection");

    /* Create a dataset creation property list */
    dcpl = H5Pcreate(H5P_DATASET_CREATE);
    VRFY((dcpl>=0), "dataset creation property list succeeded");

    ret=H5Pset_fill_value(dcpl, H5T_NATIVE_DOUBLE, &fill);
    VRFY((ret>=0), "set fill-value succeeded");

    for (n = 0; n < ndatasets; n++) {
	sprintf (dname, "dataset %d", n);
	dataset = H5Dcreate (iof, dname, H5T_NATIVE_DOUBLE, filespace, dcpl);
	VRFY((dataset > 0), dname); 

	/* calculate data to write */
	for (i = 0; i < SIZE; i++)
	    for (j = 0; j < SIZE; j++)
	        outme [i][j] = n*1000 + mpi_rank;

	H5Dwrite (dataset, H5T_NATIVE_DOUBLE, memspace, filespace, H5P_DEFAULT, outme);

	H5Dclose (dataset);
#ifdef BARRIER_CHECKS
	if (! ((n+1) % 10)) {
	    printf("created %d datasets\n", n+1);
	    MPI_Barrier(MPI_COMM_WORLD);
	}
#endif /* BARRIER_CHECKS */
    }

    H5Sclose (filespace);
    H5Sclose (memspace);
    H5Pclose (dcpl);
    H5Fclose (iof);
}

/* Example of using PHDF5 to create, write, and read compact dataset.  
 * Hyperslab is prohibited for write.
 */
void compact_dataset(char *filename)
{
    int i, j, mpi_size, mpi_rank, err_num=0;
    hbool_t use_gpfs = FALSE;
    hid_t iof, plist, dcpl, dxpl, dataset, memspace, filespace;
    hssize_t chunk_origin [DIM];
    hsize_t chunk_dims [DIM], file_dims [DIM];
    hsize_t count[DIM]={1,1};
    double outme [SIZE][SIZE], inme[SIZE][SIZE];
    char dname[]="dataset";
    herr_t ret;
                                
    MPI_Comm_rank (MPI_COMM_WORLD, &mpi_rank);
    MPI_Comm_size (MPI_COMM_WORLD, &mpi_size);

    VRFY((mpi_size <= SIZE), "mpi_size <= SIZE");

    plist = create_faccess_plist(MPI_COMM_WORLD, MPI_INFO_NULL, facc_type, use_gpfs);
    iof = H5Fcreate (filename, H5F_ACC_TRUNC, H5P_DEFAULT, plist);

    /* decide the hyperslab according to process number. */
    get_slab(chunk_origin, chunk_dims, count, file_dims);  

    /* Define data space */
    memspace = H5Screate_simple (DIM, chunk_dims, NULL);
    filespace = H5Screate_simple (DIM, file_dims, NULL);

    /* Create a compact dataset */
    dcpl = H5Pcreate(H5P_DATASET_CREATE);
    VRFY((dcpl>=0), "dataset creation property list succeeded");
    ret=H5Pset_layout(dcpl, H5D_COMPACT);
    VRFY((dcpl >= 0), "set property list for compact dataset");
    ret=H5Pset_alloc_time(dcpl, H5D_ALLOC_TIME_EARLY);
    VRFY((ret >= 0), "set space allocation time for compact dataset");

    dataset = H5Dcreate (iof, dname, H5T_NATIVE_DOUBLE, filespace, dcpl);
    VRFY((dataset >= 0), "H5Dcreate succeeded");        

    /* Define hyperslab */
    ret = H5Sselect_hyperslab (filespace, H5S_SELECT_SET, chunk_origin, chunk_dims, count, chunk_dims);    
    VRFY((ret>=0), "mdata hyperslab selection");
    
    /* set up the collective transfer properties list */
    dxpl = H5Pcreate (H5P_DATASET_XFER);
    VRFY((dxpl >= 0), "");
    ret=H5Pset_dxpl_mpio(dxpl, H5FD_MPIO_COLLECTIVE);
    VRFY((ret >= 0), "H5Pcreate xfer succeeded");

    /* calculate data to write */
    for (i = 0; i < SIZE; i++)
         for (j = 0; j < SIZE; j++)
              outme [i][j] = (i+j)*1000 + mpi_rank;

    /* Test hyperslab writing.  Supposed to fail */
    H5E_BEGIN_TRY {
        ret=H5Dwrite(dataset, H5T_NATIVE_DOUBLE, memspace, filespace, dxpl, outme);
    } H5E_END_TRY;
    VRFY((ret < 0), "H5Dwrite hyperslab write failed as expected");

    /* Recalculate data to write.  Each process writes the same data. */
    for (i = 0; i < SIZE; i++)
         for (j = 0; j < SIZE; j++)
              outme [i][j] = (i+j)*1000;

    ret=H5Dwrite (dataset, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, dxpl, outme);
    VRFY((ret >= 0), "H5Dwrite succeeded");

    H5Pclose (dcpl);
    H5Pclose (plist);
    H5Dclose (dataset);
    H5Sclose (filespace);
    H5Sclose (memspace);
    H5Fclose (iof);

    /* Open the file and dataset, read and compare the data. */
    plist = create_faccess_plist(MPI_COMM_WORLD, MPI_INFO_NULL, facc_type, use_gpfs);
    iof = H5Fopen(filename, H5F_ACC_RDONLY, plist);
    VRFY((iof >= 0), "H5Fopen succeeded");

    /* set up the collective transfer properties list */
    dxpl = H5Pcreate (H5P_DATASET_XFER);
    VRFY((dxpl >= 0), "");
    ret=H5Pset_dxpl_mpio(dxpl, H5FD_MPIO_COLLECTIVE);
    VRFY((ret >= 0), "H5Pcreate xfer succeeded");

    dataset = H5Dopen(iof, dname);
    VRFY((dataset >= 0), "H5Dcreate succeeded");

    ret = H5Dread(dataset, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, dxpl, inme);
    VRFY((ret >= 0), "H5Dread succeeded");

    /* Verify data value */
    for (i = 0; i < SIZE; i++)
        for (j = 0; j < SIZE; j++)
            if(inme[i][j] != outme[i][j])
                if(err_num++ < MAX_ERR_REPORT || verbose)
                    printf("Dataset Verify failed at [%d][%d]: expect %f, got %f\n", i, j, outme[i][j], inme[i][j]); 
                                                            
    H5Pclose(plist);
    H5Pclose(dxpl);
    H5Dclose(dataset);
    H5Fclose(iof);
}

/* Write multiple groups with a chunked dataset in each group collectively. 
 * These groups and datasets are for testing independent read later.
 */
void collective_group_write(char *filename, int ngroups)
{
    int mpi_rank, mpi_size;
    int i, j, m;
    hbool_t use_gpfs = FALSE;
    char gname[64], dname[32];
    hid_t fid, gid, did, plist, dcpl, memspace, filespace;
    DATATYPE outme[SIZE][SIZE];
    hssize_t chunk_origin[DIM];
    hsize_t chunk_dims[DIM], file_dims[DIM], count[DIM];
    const hsize_t chunk_size[2] = {SIZE/2, SIZE/2};  /* Chunk dimensions */
    herr_t ret1, ret2;

    MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);

    plist = create_faccess_plist(MPI_COMM_WORLD, MPI_INFO_NULL, facc_type, use_gpfs);
    fid = H5Fcreate(filename, H5F_ACC_TRUNC, H5P_DEFAULT, plist);
    H5Pclose(plist);

    /* decide the hyperslab according to process number. */
    get_slab(chunk_origin, chunk_dims, count, file_dims);  

    /* select hyperslab in memory and file spaces.  These two operations are
     * identical since the datasets are the same. */
    memspace  = H5Screate_simple(DIM, file_dims, NULL);
    ret1 = H5Sselect_hyperslab(memspace, H5S_SELECT_SET, chunk_origin, 
                               chunk_dims, count, chunk_dims);
    filespace = H5Screate_simple(DIM, file_dims,  NULL);
    ret2 = H5Sselect_hyperslab(filespace, H5S_SELECT_SET, chunk_origin, 
                               chunk_dims, count, chunk_dims);
    VRFY((memspace>=0), "memspace");
    VRFY((filespace>=0), "filespace");
    VRFY((ret1>=0), "mgroup memspace selection");
    VRFY((ret2>=0), "mgroup filespace selection");   
    
    dcpl = H5Pcreate(H5P_DATASET_CREATE);
    ret1 = H5Pset_chunk (dcpl, 2, chunk_size);
    VRFY((dcpl>=0), "dataset creation property");
    VRFY((ret1>=0), "set chunk for dataset creation property");
        
    /* creates ngroups groups under the root group, writes chunked 
     * datasets in parallel. */
    for(m = 0; m < ngroups; m++) {
        sprintf(gname, "group%d", m);
        gid = H5Gcreate(fid, gname, 0);
        VRFY((gid > 0), gname);

        sprintf(dname, "dataset%d", m);
        did = H5Dcreate(gid, dname, H5T_NATIVE_INT, filespace, dcpl);
        VRFY((did > 0), dname);

        for(i=0; i < SIZE; i++)
            for(j=0; j < SIZE; j++)
     	        outme[i][j] = (i+j)*1000 + mpi_rank;

        H5Dwrite(did, H5T_NATIVE_INT, memspace, filespace, H5P_DEFAULT, 
                 outme);

        H5Dclose(did);
        H5Gclose(gid);

#ifdef BARRIER_CHECKS
        if(! ((m+1) % 10)) {
            printf("created %d groups\n", m+1);
            MPI_Barrier(MPI_COMM_WORLD);
	}
#endif /* BARRIER_CHECKS */
    }
   
    H5Pclose(dcpl);
    H5Sclose(filespace);
    H5Sclose(memspace);
    H5Fclose(fid);
}

/* Let two sets of processes open and read different groups and chunked 
 * datasets independently. 
 */
void independent_group_read(char *filename, int ngroups)
{
    int      mpi_rank, m;
    hid_t    plist, fid;
    hbool_t  use_gpfs = FALSE;

    MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);
    
    plist = create_faccess_plist(MPI_COMM_WORLD, MPI_INFO_NULL, facc_type, use_gpfs);
    fid = H5Fopen(filename, H5F_ACC_RDONLY, plist);
    H5Pclose(plist);

    /* open groups and read datasets. Odd number processes read even number 
     * groups from the end; even number processes read odd number groups 
     * from the beginning. */
    if(mpi_rank%2==0) {
        for(m=ngroups-1; m==0; m-=2) 
            group_dataset_read(fid, mpi_rank, m);
    } else {
        for(m=0; m<ngroups; m+=2)
            group_dataset_read(fid, mpi_rank, m);
    }

    H5Fclose(fid);
}

/* Open and read datasets and compare data */
void group_dataset_read(hid_t fid, int mpi_rank, int m)
{
    int      ret, i, j;
    char     gname[64], dname[32];
    hid_t    gid, did;
    DATATYPE *outdata, *indata;

    indata = (DATATYPE*)malloc(SIZE*SIZE*sizeof(DATATYPE));
    outdata = (DATATYPE*)malloc(SIZE*SIZE*sizeof(DATATYPE));
    
    /* open every group under root group. */
    sprintf(gname, "group%d", m);
    gid = H5Gopen(fid, gname);
    VRFY((gid > 0), gname);

    /* check the data. */
    sprintf(dname, "dataset%d", m);
    did = H5Dopen(gid, dname);
    VRFY((did>0), dname);

    H5Dread(did, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, indata);

    /* this is the original value */
    for(i=0; i<SIZE; i++)
       for(j=0; j<SIZE; j++) { 
           *outdata = (i+j)*1000 + mpi_rank;
           outdata++;
       }
    outdata -= SIZE*SIZE;

    /* compare the original value(outdata) to the value in file(indata).*/
    ret = check_value(indata, outdata);
    VRFY((ret==0), "check the data");

    H5Dclose(did);
    H5Gclose(gid);
}

/*
 * Example of using PHDF5 to create multiple groups.  Under the root group, 
 * it creates ngroups groups.  Under the first group just created, it creates 
 * recursive subgroups of depth GROUP_DEPTH.  In each created group, it 
 * generates NDATASETS datasets.  Each process write a hyperslab of an array
 * into the file.  The structure is like
 *               
 *                             root group
 *                                 |
 *            ---------------------------- ... ... ------------------------
 *           |          |         |        ... ...  |                      |
 *       group0*+'   group1*+' group2*+'   ... ...             group ngroups*+'
 *           |
 *      1st_child_group*' 
 *           |
 *      2nd_child_group*'
 *           |
 *           :
 *           :
 *           |
 * GROUP_DEPTHth_child_group*'
 *
 *      * means the group has dataset(s).
 *      + means the group has attribute(s).
 *      ' means the datasets in the groups have attribute(s).
 */
void multiple_group_write(char *filename, int ngroups)
{
    int mpi_rank, mpi_size;
    int m;
    hbool_t use_gpfs = FALSE;
    char gname[64];
    hid_t fid, gid, plist, memspace, filespace;
    hssize_t chunk_origin[DIM];
    hsize_t chunk_dims[DIM], file_dims[DIM], count[DIM];
    herr_t ret;

    MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);

    plist = create_faccess_plist(MPI_COMM_WORLD, MPI_INFO_NULL, facc_type, use_gpfs);
    fid = H5Fcreate(filename, H5F_ACC_TRUNC, H5P_DEFAULT, plist);
    H5Pclose(plist);

    /* decide the hyperslab according to process number. */
    get_slab(chunk_origin, chunk_dims, count, file_dims);  

    /* select hyperslab in memory and file spaces.  These two operations are
     * identical since the datasets are the same. */
    memspace  = H5Screate_simple(DIM, file_dims, NULL);
    VRFY((memspace>=0), "memspace");
    ret = H5Sselect_hyperslab(memspace, H5S_SELECT_SET, chunk_origin, 
                               chunk_dims, count, chunk_dims);
    VRFY((ret>=0), "mgroup memspace selection");

    filespace = H5Screate_simple(DIM, file_dims,  NULL);
    VRFY((filespace>=0), "filespace");
    ret = H5Sselect_hyperslab(filespace, H5S_SELECT_SET, chunk_origin, 
                               chunk_dims, count, chunk_dims);
    VRFY((ret>=0), "mgroup filespace selection");   

    /* creates ngroups groups under the root group, writes datasets in 
     * parallel. */
    for(m = 0; m < ngroups; m++) {
        sprintf(gname, "group%d", m);
        gid = H5Gcreate(fid, gname, 0);
        VRFY((gid > 0), gname);

        /* create attribute for these groups. */   
	write_attribute(gid, is_group, m);
           
        if(m != 0)
	    write_dataset(memspace, filespace, gid);

        H5Gclose(gid);

#ifdef BARRIER_CHECKS
        if(! ((m+1) % 10)) {
            printf("created %d groups\n", m+1);
            MPI_Barrier(MPI_COMM_WORLD);
	}
#endif /* BARRIER_CHECKS */
    }
    
    /* recursively creates subgroups under the first group. */
    gid = H5Gopen(fid, "group0");
    create_group_recursive(memspace, filespace, gid, 0);
    ret = H5Gclose(gid);
    VRFY((ret>=0), "H5Gclose");
    
    ret = H5Sclose(filespace);
    VRFY((ret>=0), "H5Sclose");
    ret = H5Sclose(memspace);
    VRFY((ret>=0), "H5Sclose");
    ret = H5Fclose(fid);
    VRFY((ret>=0), "H5Fclose");
}

/* 
 * In a group, creates NDATASETS datasets.  Each process writes a hyperslab
 * of a data array to the file.
 */ 
void write_dataset(hid_t memspace, hid_t filespace, hid_t gid)
{
    int i, j, n;
    int mpi_rank, mpi_size;
    char dname[32];
    DATATYPE outme[SIZE][SIZE];
    hid_t did;

  
    MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);

    for(n=0; n < NDATASET; n++) {
         sprintf(dname, "dataset%d", n);
         did = H5Dcreate(gid, dname, H5T_NATIVE_INT, filespace, 
                         H5P_DEFAULT);
         VRFY((did > 0), dname);

         for(i=0; i < SIZE; i++)
             for(j=0; j < SIZE; j++)
     	         outme[i][j] = n*1000 + mpi_rank;

         H5Dwrite(did, H5T_NATIVE_INT, memspace, filespace, H5P_DEFAULT, 
                  outme);

         /* create attribute for these datasets.*/
         write_attribute(did, is_dset, n);
	 
         H5Dclose(did);
    }
}

/* 
 * Creates subgroups of depth GROUP_DEPTH recursively.  Also writes datasets
 * in parallel in each group.
 */
void create_group_recursive(hid_t memspace, hid_t filespace, hid_t gid, 
                            int counter)
{ 
   hid_t child_gid;
   int   mpi_rank;
   char  gname[64];
  
   MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);

#ifdef BARRIER_CHECKS
   if(! ((counter+1) % 10)) {
        printf("created %dth child groups\n", counter+1);
        MPI_Barrier(MPI_COMM_WORLD);
   }
#endif /* BARRIER_CHECKS */
 
   sprintf(gname, "%dth_child_group", counter+1);   
   child_gid = H5Gcreate(gid, gname, 0);
   VRFY((child_gid > 0), gname);

   /* write datasets in parallel. */
   write_dataset(memspace, filespace, gid);  

   if( counter < GROUP_DEPTH ) 
       create_group_recursive(memspace, filespace, child_gid, counter+1);

   H5Gclose(child_gid);
}

/* 
 * This function is to verify the data from multiple group testing.  It opens
 * every dataset in every group and check their correctness.  
 */
void multiple_group_read(char *filename, int ngroups)
{
    int      mpi_rank, mpi_size, error_num;
    int      m;
    hbool_t  use_gpfs = FALSE;
    char     gname[64];
    hid_t    plist, fid, gid, memspace, filespace;
    hssize_t chunk_origin[DIM];
    hsize_t  chunk_dims[DIM], file_dims[DIM], count[DIM];

    MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);

    plist = create_faccess_plist(MPI_COMM_WORLD, MPI_INFO_NULL, facc_type, use_gpfs);
    fid = H5Fopen(filename, H5F_ACC_RDONLY, plist);
    H5Pclose(plist);

    /* decide hyperslab for each process */
    get_slab(chunk_origin, chunk_dims, count, file_dims);

    /* select hyperslab for memory and file space */
    memspace  = H5Screate_simple(DIM, file_dims, NULL);
    H5Sselect_hyperslab(memspace, H5S_SELECT_SET, chunk_origin, chunk_dims,
                        count, chunk_dims);
    filespace = H5Screate_simple(DIM, file_dims, NULL);
    H5Sselect_hyperslab(filespace, H5S_SELECT_SET, chunk_origin, chunk_dims, 
                        count, chunk_dims);
    
    /* open every group under root group. */
    for(m=0; m<ngroups; m++) {
        sprintf(gname, "group%d", m);
        gid = H5Gopen(fid, gname);
        VRFY((gid > 0), gname);
         
        /* check the data. */
        if(m != 0)
            if( (error_num = read_dataset(memspace, filespace, gid))>0)
	        nerrors += error_num;
        
        /* check attribute.*/ 
        error_num = 0;        
        if( (error_num = read_attribute(gid, is_group, m))>0 )
	    nerrors += error_num;

        H5Gclose(gid);

#ifdef BARRIER_CHECKS
        if(!((m+1)%10))
            MPI_Barrier(MPI_COMM_WORLD);
#endif /* BARRIER_CHECKS */
    }

    /* open all the groups in vertical direction. */
    gid = H5Gopen(fid, "group0");
    VRFY((gid>0), "group0");
    recursive_read_group(memspace, filespace, gid, 0);
    H5Gclose(gid);

    H5Sclose(filespace);
    H5Sclose(memspace);
    H5Fclose(fid);

}

/* 
 * This function opens all the datasets in a certain, checks the data using 
 * dataset_vrfy function.
 */
int read_dataset(hid_t memspace, hid_t filespace, hid_t gid)
{
    int i, j, n, mpi_rank, mpi_size, attr_errors=0, vrfy_errors=0;
    char dname[32];
    DATATYPE *outdata, *indata;
    hid_t did;

    MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);

    indata = (DATATYPE*)malloc(SIZE*SIZE*sizeof(DATATYPE));
    outdata = (DATATYPE*)malloc(SIZE*SIZE*sizeof(DATATYPE));

    for(n=0; n<NDATASET; n++) {
        sprintf(dname, "dataset%d", n);
        did = H5Dopen(gid, dname);
        VRFY((did>0), dname);

        H5Dread(did, H5T_NATIVE_INT, memspace, filespace, H5P_DEFAULT, 
                indata);

        /* this is the original value */
        for(i=0; i<SIZE; i++)
	    for(j=0; j<SIZE; j++) { 
	         *outdata = n*1000 + mpi_rank;
                 outdata++;
	    }
        outdata -= SIZE*SIZE;

        /* compare the original value(outdata) to the value in file(indata).*/
        vrfy_errors = check_value(indata, outdata);

        /* check attribute.*/  
        if( (attr_errors = read_attribute(did, is_dset, n))>0 )
            vrfy_errors += attr_errors; 
	       
        H5Dclose(did);
    }

    free(indata);
    free(outdata);

    return vrfy_errors;
}

/* 
 * This recursive function opens all the groups in vertical direction and 
 * checks the data.
 */
void recursive_read_group(hid_t memspace, hid_t filespace, hid_t gid, 
                          int counter)
{
    hid_t child_gid;
    int   mpi_rank, err_num=0;
    char  gname[64];

    MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);
#ifdef BARRIER_CHECKS
    if((counter+1) % 10) 
        MPI_Barrier(MPI_COMM_WORLD);
#endif /* BARRIER_CHECKS */

    if( (err_num = read_dataset(memspace, filespace, gid)) )
        nerrors += err_num;

    if( counter < GROUP_DEPTH ) {
        sprintf(gname, "%dth_child_group", counter+1);
        child_gid = H5Gopen(gid, gname);
        VRFY((child_gid>0), gname);
        recursive_read_group(memspace, filespace, child_gid, counter+1);
        H5Gclose(child_gid);
    }
}

/* Create and write attribute for a group or a dataset.  For groups, attribute
 * is a scalar datum; for dataset, it is a one-dimensional array.
 */ 
void write_attribute(hid_t obj_id, int this_type, int num)
{
    hid_t   sid, aid;
    hsize_t dspace_dims[1]={8};
    int     i, mpi_rank, attr_data[8], dspace_rank=1;
    char    attr_name[32];

    MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);
        
    if(this_type == is_group) {
        sprintf(attr_name, "Group Attribute %d", num);
        sid = H5Screate(H5S_SCALAR);
        aid = H5Acreate(obj_id, attr_name, H5T_NATIVE_INT, sid, H5P_DEFAULT);
        H5Awrite(aid, H5T_NATIVE_INT,  &num);
        H5Aclose(aid);
        H5Sclose(sid);
    }
    else if(this_type == is_dset) {
        sprintf(attr_name, "Dataset Attribute %d", num);
        for(i=0; i<8; i++)
            attr_data[i] = i;
        sid = H5Screate_simple(dspace_rank, dspace_dims, NULL);
        aid = H5Acreate(obj_id, attr_name, H5T_NATIVE_INT, sid, H5P_DEFAULT);
        H5Awrite(aid, H5T_NATIVE_INT, attr_data);   
        H5Aclose(aid);
        H5Sclose(sid);
    }

}

/* Read and verify attribute for group or dataset. */
int read_attribute(hid_t obj_id, int this_type, int num)
{
    hid_t aid;
    hsize_t group_block[2]={1,1}, dset_block[2]={1, 8};
    int  i, mpi_rank, in_num, in_data[8], out_data[8], vrfy_errors = 0;
    char attr_name[32];
   
    MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);
    
    if(this_type == is_group) {
        sprintf(attr_name, "Group Attribute %d", num);
        aid = H5Aopen_name(obj_id, attr_name);
        if(MAINPROCESS) {
            H5Aread(aid, H5T_NATIVE_INT, &in_num);
            vrfy_errors =  dataset_vrfy(NULL, NULL, NULL, group_block, 
                                        &in_num, &num);
	}
        H5Aclose(aid);
    }
    else if(this_type == is_dset) {
        sprintf(attr_name, "Dataset Attribute %d", num);  
        for(i=0; i<8; i++)
            out_data[i] = i;
        aid = H5Aopen_name(obj_id, attr_name);
        if(MAINPROCESS) {
            H5Aread(aid, H5T_NATIVE_INT, in_data);
            vrfy_errors = dataset_vrfy(NULL, NULL, NULL, dset_block, in_data,
                                       out_data);
	}
        H5Aclose(aid);
    }       
    
    return vrfy_errors;
}

/* This functions compares the original data with the read-in data for its 
 * hyperslab part only by process ID. */
int check_value(DATATYPE *indata, DATATYPE *outdata) 
{
    int mpi_rank, mpi_size, err_num=0;
    hsize_t i, j;
    hssize_t chunk_origin[DIM];
    hsize_t  chunk_dims[DIM], count[DIM];

    MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);
    
    get_slab(chunk_origin, chunk_dims, count, NULL);

    indata += chunk_origin[0]*SIZE;
    outdata += chunk_origin[0]*SIZE;
    for(i=chunk_origin[0]; i<(chunk_origin[0]+chunk_dims[0]); i++)
         for(j=chunk_origin[1]; j<(chunk_origin[1]+chunk_dims[1]); j++) {
              if( *indata != *outdata )
	          if(err_num++ < MAX_ERR_REPORT || verbose)
		      printf("Dataset Verify failed at [%ld][%ld](row %ld, col%ld): expect %d, got %d\n", (long)i, (long)j, (long)i, (long)j, *outdata, *indata); 
	 }
    if(err_num > MAX_ERR_REPORT && !verbose)
        printf("[more errors ...]\n");
    if(err_num)
        printf("%d errors found in check_value\n", err_num);
    return err_num;
}

/* Decide the portion of data chunk in dataset by process ID. */
void get_slab(hssize_t chunk_origin[], hsize_t chunk_dims[], hsize_t count[],
              hsize_t file_dims[])
{
    int mpi_rank, mpi_size;
    
    MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);

    if(chunk_origin != NULL) {
        chunk_origin[0] = mpi_rank * (SIZE/mpi_size);
        chunk_origin[1] = 0;
    }
    if(chunk_dims != NULL) {
        chunk_dims[0]   = SIZE/mpi_size;
        chunk_dims[1]   = SIZE;
    }
    if(file_dims != NULL) 
        file_dims[0] = file_dims[1] = SIZE;
    if(count != NULL) 
        count[0] = count[1] = 1;
}

/*=============================================================================
 *                         End of t_mdset.c
 *===========================================================================*/
