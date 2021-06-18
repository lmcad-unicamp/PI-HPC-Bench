// C++ informative line for the emacs editor: -*- C++ -*-
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

#ifndef _H5FileCreatPropList_H
#define _H5FileCreatPropList_H

#ifndef H5_NO_NAMESPACE
namespace H5 {
#endif

// class for file access properties
class H5_DLLCPP FileCreatPropList : public PropList {
   public:
	static const FileCreatPropList DEFAULT;
	
	// Creates a file create property list.
	FileCreatPropList();

	// Copy constructor: creates a copy of a FileCreatPropList object
	FileCreatPropList( const FileCreatPropList& orig );

	// Retrieves version information for various parts of a file.
	void getVersion( int& boot, int& freelist, int& stab, int& shhdr ) const;

	// Sets the userblock size field of a file creation property list.
	void setUserblock( hsize_t size ) const;

	// Gets the size of a user block in this file creation property list.
	hsize_t getUserblock() const;

	// Sets file size-of addresses and sizes.
	void setSizes( size_t sizeof_addr = 4, size_t sizeof_size = 4 ) const;

	// Retrieves the size-of address and size quantities stored in a 
	// file according to this file creation property list.
	void getSizes( size_t& sizeof_addr, size_t& sizeof_size ) const;

#ifdef H5_WANT_H5_V1_4_COMPAT
	// Sets the size of parameters used to control the symbol table nodes.
	void setSymk( int int_nodes_k, int leaf_nodes_k ) const;

	// Retrieves the size of the symbol table B-tree 1/2 rank and the
	// symbol table leaf node 1/2 size.
	void getSymk( int& int_nodes_k, int& leaf_nodes_k ) const;
#else /* H5_WANT_H5_V1_4_COMPAT */
	// Sets the size of parameters used to control the symbol table nodes.
	void setSymk( int int_nodes_k, unsigned leaf_nodes_k ) const;

	// Retrieves the size of the symbol table B-tree 1/2 rank and the
	// symbol table leaf node 1/2 size.
	void getSymk( int& int_nodes_k, unsigned& leaf_nodes_k ) const;
#endif /* H5_WANT_H5_V1_4_COMPAT */

	// Sets the size of parameter used to control the B-trees for
	// indexing chunked datasets.
	void setIstorek( int ik ) const;

	// Returns the 1/2 rank of an indexed storage B-tree.
	int getIstorek() const;

	// Creates a copy of an existing file create property list
	// using the property list id
	FileCreatPropList (const hid_t plist_id) : PropList( plist_id ) {}

	// Default destructor
	virtual ~FileCreatPropList();
};
#ifndef H5_NO_NAMESPACE
}
#endif
#endif
