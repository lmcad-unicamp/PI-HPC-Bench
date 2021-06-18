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
 * Programmer:  Pedro Vicente <pvn@ncsa.uiuc.edu>
 *              April 12, 2002
 *
 * Purpose:     Tests the H5Dset_extent call
 */


#include "hdf5.h"
#include "h5test.h"


/*-------------------------------------------------------------------------
 *
 * Tests the function H5Dset_extent. In the current version of the library 
 * the dataset MUST be chunked.
 *
 *-------------------------------------------------------------------------
 */

#define RANK 2


int main( void )
{

    hid_t   file_id;
    hid_t   dataset_id=(-1);
    hid_t   space_id=(-1);  
    hid_t   plist_id=(-1);
    hsize_t dims[RANK] = { 90, 90 };
    hsize_t dims_new[RANK] = { 70, 70 };
    hsize_t dims_chunk[RANK] = { 20, 20 };
    hsize_t dims_out[RANK];
    hsize_t maxdims[RANK] = { H5S_UNLIMITED, H5S_UNLIMITED };
    int     data[ 90 ][ 90 ];
    int     buf1[ 70 ][ 70 ];
    int     buf2[ 90 ][ 90 ];
    int     i, j, n = 0;
    int     fillvalue = 1;   /* Fill value for the dataset */


    for( i = 0; i < 90; i++ )
        for( j = 0; j < 90; j++ )
            data[i][j] = n++;

    /*-------------------------------------------------------------------------
    * Test H5Dset_extent with chunks on the raw data cache
    *-------------------------------------------------------------------------
    */


    /* Create a new file using default properties. */
    if ((file_id = H5Fcreate( "set_extent_create.h5", H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT ))<0) goto out;

    TESTING("extend dataset create with fill value");

    /* Create the data space with unlimited dimensions. */
    if ((space_id = H5Screate_simple( RANK, dims, maxdims ))<0) goto out;

    /* Modify dataset creation properties, i.e. enable chunking. */
    if ((plist_id = H5Pcreate (H5P_DATASET_CREATE ))<0) goto out;
    if (H5Pset_chunk( plist_id, RANK, dims_chunk )<0) goto out;
    if (H5Pset_fill_value( plist_id, H5T_NATIVE_INT, &fillvalue )<0) goto out; 


    /*-------------------------------------------------------------------------
    * Create and write one dataset
    *-------------------------------------------------------------------------
    */

    /* Create a new dataset */
    if ((dataset_id = H5Dcreate( file_id , "Dataset1", H5T_NATIVE_INT, space_id, plist_id ))<0) goto out;

    /* Write the data. */
    if (H5Dwrite( dataset_id , H5T_NATIVE_INT, space_id, H5S_ALL, H5P_DEFAULT, data )<0) goto out;

    /*-------------------------------------------------------------------------
    * Set new dimensions for the array; shrink it 
    *-------------------------------------------------------------------------
    */

    /* Set new dimensions for the array. */
    if (H5Dset_extent( dataset_id , dims_new )<0) goto out;

    /* Get the space. */
    if ((space_id = H5Dget_space( dataset_id ))<0) goto out;

    /* Get dimensions. */
    if (H5Sget_simple_extent_dims( space_id, dims_out, NULL )<0) goto out;

    if ( dims_out[0] != dims_new[0] ) goto out;


    /*-------------------------------------------------------------------------
    * Read
    *-------------------------------------------------------------------------
    */

    /* Read the new dataset. */
    if (H5Dread( dataset_id, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, buf1 )<0) goto out;


    /* Compare the read array with the original array */
    for( i = 0; i < (int)dims_out[0]; i++ )
        for( j = 0; j < (int)dims_out[1]; j++ )
            if (  buf1[i][j] != data[i][j] ) goto out;


    /*-------------------------------------------------------------------------
    * Set new dimensions for the array; expand it again 
    *-------------------------------------------------------------------------
    */

    /* Set new dimensions for the array. */
    if (H5Dset_extent( dataset_id , dims )<0) goto out;

    /* Get the space. */
    if ((space_id = H5Dget_space( dataset_id ))<0) goto out;

    /* Get dimensions. */
    if (H5Sget_simple_extent_dims( space_id, dims_out, NULL )<0) goto out;

    if ( dims_out[0] != dims[0] ) goto out;


    /*-------------------------------------------------------------------------
    * Read
    *-------------------------------------------------------------------------
    */

    /* Read the new dataset. */
    if (H5Dread( dataset_id, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, buf2 )<0) goto out;

    /* Compare the read array with the original array */
    for( i = 0; i < (int)dims_out[0]; i++ ) {
        for( j = 0; j < (int)dims_out[1]; j++ ) {
            if ( i >= 70 || j >= 70 ) {
                if (  buf2[i][j] != fillvalue ) goto out;
            }
            else {
                if (  buf2[i][j] != data[i][j] ) goto out;
            }
        }
    }


    /*-------------------------------------------------------------------------
    * Close/release resources
    *-------------------------------------------------------------------------
    */

    H5Dclose( dataset_id );
    H5Sclose( space_id );
    H5Pclose( plist_id  );

    PASSED();
    TESTING("extend dataset create without fill value");

    /* Create the data space with unlimited dimensions. */
    if ((space_id = H5Screate_simple( RANK, dims, maxdims ))<0) goto out;

    /* Modify dataset creation properties, i.e. enable chunking. */
    if ((plist_id = H5Pcreate (H5P_DATASET_CREATE ))<0) goto out;
    if (H5Pset_chunk( plist_id, RANK, dims_chunk )<0) goto out;
    if (H5Pset_fill_time( plist_id, H5D_FILL_TIME_ALLOC)<0) goto out;

    /*-------------------------------------------------------------------------
    * Create and write one dataset
    *-------------------------------------------------------------------------
    */

    /* Create a new dataset */
    if ((dataset_id = H5Dcreate( file_id , "Dataset2", H5T_NATIVE_INT, space_id, plist_id ))<0) goto out;

    /* Write the data. */
    if (H5Dwrite( dataset_id , H5T_NATIVE_INT, space_id, H5S_ALL, H5P_DEFAULT, data )<0) goto out;

    /*-------------------------------------------------------------------------
    * Set new dimensions for the array; shrink it 
    *-------------------------------------------------------------------------
    */

    /* Set new dimensions for the array. */
    if (H5Dset_extent( dataset_id , dims_new )<0) goto out;

    /* Get the space. */
    if ((space_id = H5Dget_space( dataset_id ))<0) goto out;

    /* Get dimensions. */
    if (H5Sget_simple_extent_dims( space_id, dims_out, NULL )<0) goto out;

    if ( dims_out[0] != dims_new[0] ) goto out;


    /*-------------------------------------------------------------------------
    * Read
    *-------------------------------------------------------------------------
    */

    /* Read the new dataset. */
    if (H5Dread( dataset_id, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, buf1 )<0) goto out;


    /* Compare the read array with the original array */
    for( i = 0; i < (int)dims_out[0]; i++ )
        for( j = 0; j < (int)dims_out[1]; j++ )
            if (  buf1[i][j] != data[i][j] ) goto out;


    /*-------------------------------------------------------------------------
    * Set new dimensions for the array; expand it again 
    *-------------------------------------------------------------------------
    */

    /* Set new dimensions for the array. */
    if (H5Dset_extent( dataset_id , dims )<0) goto out;

    /* Get the space. */
    if ((space_id = H5Dget_space( dataset_id ))<0) goto out;

    /* Get dimensions. */
    if (H5Sget_simple_extent_dims( space_id, dims_out, NULL )<0) goto out;

    if ( dims_out[0] != dims[0] ) goto out;


    /*-------------------------------------------------------------------------
    * Read
    *-------------------------------------------------------------------------
    */

    /* Read the new dataset. */
    if (H5Dread( dataset_id, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, buf2 )<0) goto out;

    /* Compare the read array with the original array */
    for( i = 0; i < (int)dims_out[0]; i++ ) {
        for( j = 0; j < (int)dims_out[1]; j++ ) {
            if ( i >= 70 || j >= 70 ) {
                if (  buf2[i][j] != 0 ) goto out;
            }
            else {
                if (  buf2[i][j] != data[i][j] ) goto out;
            }
        }
    }


    /*-------------------------------------------------------------------------
    * Close/release resources
    *-------------------------------------------------------------------------
    */

    H5Dclose( dataset_id );
    H5Sclose( space_id );
    H5Pclose( plist_id  );

    H5Fclose( file_id );

    PASSED();



    /*-------------------------------------------------------------------------
    * Test H5Dset_extent with chunks written to file
    *-------------------------------------------------------------------------
    */


    /* Create a new file using default properties. */
    if ((file_id = H5Fcreate( "set_extent_read.h5", H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT ))<0) goto out;


    TESTING("extend dataset read with fill value");

    /* Create the data space with unlimited dimensions. */
    if ((space_id = H5Screate_simple( RANK, dims, maxdims ))<0) goto out; 

    /* Modify dataset creation properties, i.e. enable chunking. */
    if ((plist_id = H5Pcreate (H5P_DATASET_CREATE ))<0) goto out;
    if (H5Pset_chunk( plist_id, RANK, dims_chunk )<0) goto out;
    if (H5Pset_fill_value( plist_id, H5T_NATIVE_INT, &fillvalue )<0) goto out; 

    /* Create a new dataset within the file using cparms creation properties. */
    if ((dataset_id = H5Dcreate( file_id , "Dataset1", H5T_NATIVE_INT, space_id, plist_id ))<0) goto out;

    /* Write the data. */
    if (H5Dwrite( dataset_id , H5T_NATIVE_INT, space_id, H5S_ALL, H5P_DEFAULT, data )<0) goto out;

    /* Close/release resources. */
    H5Dclose( dataset_id );
    H5Sclose( space_id );
    H5Pclose( plist_id  );
    H5Fclose( file_id );


    /* Open the file */
    if ((file_id = H5Fopen( "set_extent_read.h5", H5F_ACC_RDWR, H5P_DEFAULT ))<0) goto out;

    /* Open the dataset */
    if ((dataset_id = H5Dopen( file_id , "Dataset1" ))<0) goto out;

    /* Set new dimensions for the array. */
    if (H5Dset_extent( dataset_id, dims_new )<0) goto out;

    /* Get the space. */
    if ((space_id = H5Dget_space( dataset_id ))<0) goto out;

    /* Get dimensions. */
    if (H5Sget_simple_extent_dims( space_id, dims_out, NULL )<0) goto out;

    if ( dims_out[0] != dims_new[0] ) goto out;

    /* Read the new dataset. */
    if (H5Dread( dataset_id, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, buf1 )<0) goto out;

    /* Compare the read array with the original array */
    for( i = 0; i < (int)dims_out[0]; i++ )
        for( j = 0; j < (int)dims_out[1]; j++ )
            if (  buf1[i][j] != data[i][j] )    goto out;

    /*-------------------------------------------------------------------------
    * Set new dimensions for the array; expand it again 
    *-------------------------------------------------------------------------
    */

    /* Set new dimensions for the array. */
    if (H5Dset_extent( dataset_id , dims )<0) goto out;

    /* Get the space. */
    if ((space_id = H5Dget_space( dataset_id ))<0) goto out;

    /* Get dimensions. */
    if (H5Sget_simple_extent_dims( space_id, dims_out, NULL )<0) goto out;

    if ( dims_out[0] != dims[0] ) goto out;

    /* Read the new dataset. */
    if (H5Dread( dataset_id, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, buf2 )<0) goto out;

    /* Compare the read array with the original array */
    for( i = 0; i < (int)dims_out[0]; i++ ) {
        for( j = 0; j < (int)dims_out[1]; j++ ) {
            if ( i >= 70 || j >= 70 ) {
                if (  buf2[i][j] != fillvalue ) goto out;
            }
            else {
                if (  buf2[i][j] != data[i][j] ) goto out;
            }
        }
    }


    /*-------------------------------------------------------------------------
    * Close/release resources
    *-------------------------------------------------------------------------
    */

    H5Dclose( dataset_id );
    H5Sclose( space_id );

    PASSED();


    TESTING("extend dataset read without fill value");

    /* Create the data space with unlimited dimensions. */
    if ((space_id = H5Screate_simple( RANK, dims, maxdims ))<0) goto out; 

    /* Modify dataset creation properties, i.e. enable chunking. */
    if ((plist_id = H5Pcreate (H5P_DATASET_CREATE ))<0) goto out;
    if (H5Pset_chunk( plist_id, RANK, dims_chunk )<0) goto out;
    if (H5Pset_fill_time( plist_id, H5D_FILL_TIME_ALLOC)<0) goto out;

    /* Create a new dataset within the file using cparms creation properties. */
    if ((dataset_id = H5Dcreate( file_id , "Dataset2", H5T_NATIVE_INT, space_id, plist_id ))<0) goto out;

    /* Write the data. */
    if (H5Dwrite( dataset_id , H5T_NATIVE_INT, space_id, H5S_ALL, H5P_DEFAULT, data )<0) goto out;

    /* Close/release resources. */
    H5Dclose( dataset_id );
    H5Sclose( space_id );
    H5Pclose( plist_id  );
    H5Fclose( file_id );


    /* Open the file */
    if ((file_id = H5Fopen( "set_extent_read.h5", H5F_ACC_RDWR, H5P_DEFAULT ))<0) goto out;

    /* Open the dataset */
    if ((dataset_id = H5Dopen( file_id , "Dataset2" ))<0) goto out;

    /* Set new dimensions for the array. */
    if (H5Dset_extent( dataset_id, dims_new )<0) goto out;

    /* Get the space. */
    if ((space_id = H5Dget_space( dataset_id ))<0) goto out;

    /* Get dimensions. */
    if (H5Sget_simple_extent_dims( space_id, dims_out, NULL )<0) goto out;

    if ( dims_out[0] != dims_new[0] ) goto out;

    /* Read the new dataset. */
    if (H5Dread( dataset_id, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, buf1 )<0) goto out;

    /* Compare the read array with the original array */
    for( i = 0; i < (int)dims_out[0]; i++ )
        for( j = 0; j < (int)dims_out[1]; j++ )
            if (  buf1[i][j] != data[i][j] ) goto out;

    /*-------------------------------------------------------------------------
    * Set new dimensions for the array; expand it again 
    *-------------------------------------------------------------------------
    */

    /* Set new dimensions for the array. */
    if (H5Dset_extent( dataset_id , dims )<0) goto out;

    /* Get the space. */
    if ((space_id = H5Dget_space( dataset_id ))<0) goto out;

    /* Get dimensions. */
    if (H5Sget_simple_extent_dims( space_id, dims_out, NULL )<0) goto out;

    if ( dims_out[0] != dims[0] ) goto out;

    /* Read the new dataset. */
    if (H5Dread( dataset_id, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, buf2 )<0) goto out;

    /* Compare the read array with the original array */
    for( i = 0; i < (int)dims_out[0]; i++ ) {
        for( j = 0; j < (int)dims_out[1]; j++ ) {
            if ( i >= 70 || j >= 70 ) {
                if (  buf2[i][j] != 0 ) goto out;
            }
            else {
                if (  buf2[i][j] != data[i][j] ) goto out;
            }
        }
    }


    /*-------------------------------------------------------------------------
    * Close/release resources
    *-------------------------------------------------------------------------
    */

    H5Dclose( dataset_id );
    H5Sclose( space_id );


    H5Fclose( file_id );

    PASSED();

    puts("All set_extent tests passed.");
    return 0;


out:
    H5Dclose( dataset_id );
    H5Sclose( space_id );
    H5Pclose( plist_id  );
    H5Fclose( file_id );
    H5_FAILED();
    return 1;
}

