
! * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
!   Copyright by the Board of Trustees of the University of Illinois.         *
!   All rights reserved.                                                      *
!                                                                             *
!   This file is part of HDF5.  The full HDF5 copyright notice, including     *
!   terms governing use, modification, and redistribution, is contained in    *
!   the files COPYING and Copyright.html.  COPYING can be found at the root   *
!   of the source code distribution tree; Copyright.html can be found at the  *
!   root level of an installed copy of the electronic HDF5 document set and   *
!   is linked from the top-level documents page.  It can also be found at     *
!   http://hdf.ncsa.uiuc.edu/HDF5/doc/Copyright.html.  If you do not have     *
!   access to either file, you may request a copy from hdfhelp@ncsa.uiuc.edu. *
! * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
!
!
! This file contains FORTRAN90 interfaces for H5I functions
!
      MODULE H5I

        USE H5GLOBAL
      
      CONTAINS

!----------------------------------------------------------------------
! Name:		h5iget_type_f 
!
! Purpose:	Retrieves the type of an object.  	
!
! Inputs: 	obj_id		- object identifier 
! Outputs:  
!		type		- type of the object, possible values:   
!				  H5I_FILE_F
!				  H5I_GROUP_F
!				  H5I_DATATYPE_F
!				  H5I_DATASPACE_F
!				  H5I_DATASET_F
!				  H5I_ATTR_F
!				  H5I_BADID_F
!		hdferr:		- error code		
!				 	Success:  0
!				 	Failure: -1   
! Optional parameters:
!				NONE
!
! Programmer:	Elena Pourmal
!		August 12, 1999	
!
! Modifications: 	Explicit Fortran interfaces were added for 
!			called C functions (it is needed for Windows
!			port).  March 5, 2001 
!
! Comment:		
!----------------------------------------------------------------------
          SUBROUTINE h5iget_type_f(obj_id, type, hdferr) 
!
!This definition is needed for Windows DLLs
!DEC$if defined(BUILD_HDF5_DLL)
!DEC$attributes dllexport :: h5iget_type_f
!DEC$endif
!
            IMPLICIT NONE
            INTEGER(HID_T), INTENT(IN) :: obj_id  !Object identifier 
            INTEGER, INTENT(OUT) :: type !type of an object. 
                                         !possible values are:
                                         !H5I_FILE_F
                                         !H5I_GROUP_F
                                         !H5I_DATATYPE_F
                                         !H5I_DATASPACE_F
                                         !H5I_DATASET_F
                                         !H5I_ATTR_F
                                         !H5I_BADID_F
            INTEGER, INTENT(OUT) :: hdferr  ! Error code

!            INTEGER, EXTERNAL :: h5iget_type_c
!  Interface is needed for MS FORTRAN
!
            INTERFACE
              INTEGER FUNCTION h5iget_type_c(obj_id, type)
              USE H5GLOBAL
              !DEC$ IF DEFINED(HDF5F90_WINDOWS)
              !MS$ATTRIBUTES C,reference,alias:'_H5IGET_TYPE_C':: h5iget_type_c
              !DEC$ ENDIF
              INTEGER(HID_T), INTENT(IN) :: obj_id 
              INTEGER, INTENT(OUT) :: type
              END FUNCTION h5iget_type_c
            END INTERFACE
            hdferr = h5iget_type_c(obj_id, type)
          END SUBROUTINE h5iget_type_f
!----------------------------------------------------------------------
! Name:		h5iget_name_f 
!
! Purpose: 	Gets a name of an object specified by its idetifier.  
!
! Inputs:  
!		obj_id		- attribute identifier
!		buf_size	- size of a buffer to read name in
! Outputs:  
!		buf		- buffer to read name in, name will be truncated if
!                                 buffer is not big enough
!               name_size       - name size
!		hdferr:		- error code		
!				 	Success:  0
!				 	Failure: -1   
! Optional parameters:
!				NONE			
!
! Programmer:	Elena Pourmal
!		March 12, 2003
!
! Modifications: 	
!
!----------------------------------------------------------------------


          SUBROUTINE h5iget_name_f(obj_id, buf, buf_size, name_size, hdferr) 
!This definition is needed for Windows DLLs
!DEC$if defined(BUILD_HDF5_DLL)
!DEC$attributes dllexport :: h5iget_name_f
!DEC$endif
            IMPLICIT NONE
            INTEGER(HID_T), INTENT(IN) :: obj_id     ! Object identifier 
            INTEGER(SIZE_T), INTENT(IN) :: buf_size  ! Buffer size 
            CHARACTER(LEN=*), INTENT(OUT) :: buf   ! Buffer to hold object name
            INTEGER(SIZE_T), INTENT(OUT) :: name_size ! Actual name size
            INTEGER, INTENT(OUT) :: hdferr         ! Error code:
                                                   ! 0 if successful,
                                                   ! -1 if fail
!            INTEGER, EXTERNAL :: h5iget_name_c
!  MS FORTRAN needs explicit interface for C functions called here.
!
            INTERFACE
              INTEGER FUNCTION h5iget_name_c(obj_id, buf, buf_size, name_size)
              USE H5GLOBAL
              !DEC$ IF DEFINED(HDF5F90_WINDOWS)
              !MS$ATTRIBUTES C,reference,alias:'_H5IGET_NAME_C'::h5iget_name_c
              !DEC$ ENDIF
              !DEC$ATTRIBUTES reference :: buf
              INTEGER(HID_T), INTENT(IN) :: obj_id
              CHARACTER(LEN=*), INTENT(OUT) :: buf
              INTEGER(SIZE_T), INTENT(IN) :: buf_size
              INTEGER(SIZE_T), INTENT(OUT) :: name_size
              END FUNCTION h5iget_name_c
            END INTERFACE

            hdferr = h5iget_name_c(obj_id, buf, buf_size, name_size)
          END SUBROUTINE h5iget_name_f


      END MODULE H5I
            

                                     


