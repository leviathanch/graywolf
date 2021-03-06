/*
 *   Copyright (C) 1991-1992 Yale University
 *
 *   This work is distributed in the hope that it will be useful; you can
 *   redistribute it and/or modify it under the terms of the
 *   GNU General Public License as published by the Free Software Foundation;
 *   either version 2 of the License,
 *   or any later version, on the following conditions:
 *
 *   (a) YALE MAKES NO, AND EXPRESSLY DISCLAIMS
 *   ALL, REPRESENTATIONS OR WARRANTIES THAT THE MANUFACTURE, USE, PRACTICE,
 *   SALE OR
 *   OTHER DISPOSAL OF THE SOFTWARE DOES NOT OR WILL NOT INFRINGE UPON ANY
 *   PATENT OR
 *   OTHER RIGHTS NOT VESTED IN YALE.
 *
 *   (b) YALE MAKES NO, AND EXPRESSLY DISCLAIMS ALL, REPRESENTATIONS AND
 *   WARRANTIES
 *   WHATSOEVER WITH RESPECT TO THE SOFTWARE, EITHER EXPRESS OR IMPLIED,
 *   INCLUDING,
 *   BUT NOT LIMITED TO, WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A
 *   PARTICULAR
 *   PURPOSE.
 *
 *   (c) LICENSEE SHALL MAKE NO STATEMENTS, REPRESENTATION OR WARRANTIES
 *   WHATSOEVER TO
 *   ANY THIRD PARTIES THAT ARE INCONSISTENT WITH THE DISCLAIMERS BY YALE IN
 *   ARTICLE
 *   (a) AND (b) above.
 *
 *   (d) IN NO EVENT SHALL YALE, OR ITS TRUSTEES, DIRECTORS, OFFICERS,
 *   EMPLOYEES AND
 *   AFFILIATES BE LIABLE FOR DAMAGES OF ANY KIND, INCLUDING ECONOMIC DAMAGE OR
 *   INJURY TO PROPERTY AND LOST PROFITS, REGARDLESS OF WHETHER YALE SHALL BE
 *   ADVISED, SHALL HAVE OTHER REASON TO KNOW, OR IN FACT SHALL KNOW OF THE
 *   POSSIBILITY OF THE FOREGOING.
 *
 */

/*----------------------------------------------------------------- 
FILE:	    dset.c                                       
DESCRIPTION:disjoint set utility functions 
CONTENTS:   
DATE:	    Apr 26, 1991 
REVISIONS:  Original Version      -WS
            Jun 91 - Hide one level of indirection from user  -RAW
            Oct 91 -  Changed type for stricter gcc -RAW
	    Sun Dec  8 23:28:03 EST 1991 - removed indirection needed
		in comparison functions.
	    Mon Dec  9 15:30:38 EST 1991 - added back the useful
		function - Ydset_enumerate_parents.  Also now free all
		memory correctly.
	    Tue Dec 10 01:46:21 EST 1991 added Ydset_interval and fixed
		problem with Ydset_enumerate_parents.
	    Sun Dec 15 02:28:14 EST 1991 - added dset_dump_tree.
----------------------------------------------------------------- */
#define  YDSET_DEFS
#include <globals.h>

/* ***************** STATIC VARIABLE DEFINITIONS ******************* */
static YDSETPTR dsetS ;   /* current dset info */


/* ***************** STATIC FUNCTION DEFINITIONS ******************* */
/*-------------------------------------------------------------------------
  This comparision function is used to eliminate the extra level of
  indirection in the users comparison function.
  --------------------------------------------------------------------------*/
int compare_dset( data1_p, data2_p )
ELEMENT *data1_p, *data2_p ;
{
   return (*dsetS->compare_func)( data1_p->data, data2_p->data ) ;
} /* end compare_dset() */

/*-----------------------
  -----------------------*/
void dset_free_element( ptr )
ELEMENTPTR ptr;
{
  /* first free the users data */
  if( dsetS->user_delete) {
    (*dsetS->user_delete)(ptr->data);
  }
  /* then free the set element */
  YFREE(ptr);
} /* end dset_free_element() */

/* delete all the trees associated with set */
void dset_free_trees( YDSETPTR dset  )
{
  if ( dset->superset_tree ) {
    Yrbtree_free(dset->superset_tree,NULL);
    dset->superset_tree = NIL(YTREEPTR) ;
  }
  if ( dset->subset_tree ) {
    Yrbtree_free(dset->subset_tree,NULL);
    dset->subset_tree = NIL(YTREEPTR) ;
  }
  if ( dset->parent_tree ) {
    Yrbtree_free(dset->parent_tree,NULL);
    dset->parent_tree = NIL(YTREEPTR) ;
  }
} /* end dset_free_trees() */

/*---------------------------------------------------
  compare parents
  ----------------------------------------------------*/
int compare_parents(p1,p2)
ELEMENTPTR p1, p2 ;
{
  return ( p1->parent - p2->parent );
} 

/*-----------------------
  -----------------------*/
static ELEMENTPTR path_compression( ptr )
ELEMENTPTR ptr ;
{
  if( ptr != ptr->parent ){
    ptr->parent = path_compression( ptr->parent ) ;
  }
  return( ptr->parent ) ;
} /* end path_compression */

/*-----------------------
  -----------------------*/
static ELEMENTPTR find( dset, data )
YDSETPTR dset ;
VOIDPTR data ;
{
  ELEMENT test ;  /* dummy record for comparison */
  ELEMENTPTR ptr ; /* the found/new node in rbtree */
  
  dsetS = dset ;
  test.data = data ;
  /* 
    This section finds the dset data structure
    for the given data.  If it doesn't exist it
    creates a new dset record.
    */
  if( ! (ptr = (ELEMENTPTR) Yrbtree_search( dset->dtree, &test ) ) ) {
    /* create a new element */
    ptr = YMALLOC( 1, ELEMENT ) ;
    ptr->data = data ;
    ptr->parent = ptr ;  /* self index initially */
    ptr->rank = 0 ;
    ptr->size = 1 ;
    Yrbtree_insert( dset->dtree, ptr ) ;
    return( ptr ) ;
  }
  
  /* 
    Now that we have the dset data structure,
    compress the path
    */
  ptr = path_compression( ptr ) ;
  
  return( ptr ) ;
} /* end find */

/*----------------------------------------------
  dset_find_set
  Same as dset_find except avoid call to make set
  ----------------------------------------------*/
static ELEMENTPTR dset_find_set( dset, data )
YDSETPTR dset ;
VOIDPTR data ;
{
  ELEMENT dummy ;
  ELEMENTPTR ptr ;
  
  dsetS = dset ;
  dummy.data = data;
  if( ptr = (ELEMENTPTR) Yrbtree_search( dset->dtree, &dummy )){
    ptr = path_compression(ptr);
  }
  return( ptr ) ;
} /* dset_find_set */


/* ***************** end STATIC functions ******************* */

/* ***************** USER functions ******************* */

/*---------------------------------------------------------------------
  Ydset_init()
  NOTE that the users comparison function will be passed handles rather
  then pointers.  Thus, the user comparison function must do an 
  added level of indirection
  ---------------------------------------------------------------------*/
YDSETPTR Ydset_init( compare_func )
int (*compare_func)() ;
{
  YDSETPTR dset ; /* in reality this is a YTREEPTR not anymore! */
  
  dset = YMALLOC( 1, YDSETBOX ) ;
  dset->dtree = Yrbtree_init( compare_dset );
  dset->compare_func = compare_func ;
  dset->superset_tree = NIL(YTREEPTR) ;
  dset->subset_tree = NIL(YTREEPTR) ;
  dset->parent_tree = NIL(YTREEPTR) ;
  return( dset ) ;
} /* end Ydset_init */


/*-----------------------------------
  Ydset_enumerate
  enumerate all elements in all sets
  -----------------------------------*/
VOIDPTR Ydset_enumerate( dset , startFlag )
YDSETPTR dset;
BOOL startFlag;
{

  ELEMENTPTR ptr ;

  D("Ydset_enumerate",
    printf("start Ydset_enumerate\n");
  );
  
  dsetS = dset ;
  ptr = (ELEMENTPTR) Yrbtree_enumerate( dset->dtree, startFlag );

  D("Ydset_enumerate",
    printf("end Ydset_enumerate\n");
  );

  if( ptr ) {
    return( ptr->data ) ;
  } else {
    return ( NIL(VOIDPTR) );
  }

} /* end Ydset_enumerate() */

/*-----------------------
  Ydset_enumerate_superset
  Enumerates all the elements of all subsets .
  Items are sorted by set, so all elements of each set
  will be returned together.
  -----------------------*/
/*** Enumerates all the elements of the whole superset ***/
VOIDPTR Ydset_enumerate_superset( YDSETPTR dset , BOOL startFlag )
{
  ELEMENTPTR ptr ;
  YTREEPTR dtree ;
  
  D("Ydset_enumerate_superset",
    printf("start Ydset_enumerate_superset\n");
  );

  dtree = dset->dtree ;
  if ( startFlag ) {
    if ( dset->superset_tree ) {
      Yrbtree_free(dset->superset_tree,NULL);
    }
    dset->superset_tree = Yrbtree_init(compare_parents);
      
    /* sort elements by set */
    Yrbtree_enumeratePush(dtree);
    for ( ptr = (ELEMENTPTR)Yrbtree_enumerate( dtree, TRUE); ptr;
	 ptr = (ELEMENTPTR)Yrbtree_enumerate( dtree, FALSE) ) {
      /* compress path so everyone will point to same parent */
      path_compression( ptr ) ; /* compress the path */
      Yrbtree_insert(dset->superset_tree,ptr);
    }
    Yrbtree_enumeratePop(dtree);

    ASSERTNFAULT(Yrbtree_size(dset->superset_tree) == Yrbtree_size(dtree),
       "Ydset_enumerate_superset","bogus superset sizes");
    
  }   /* end if start flag */
  
  ptr = (ELEMENTPTR)Yrbtree_enumerate( dset->superset_tree, startFlag);
  
  D("Ydset_enumerate_superset",
    printf("end Ydset_enumerate_superset\n");
  );

  if (ptr) {
    return ( ptr->data );
  }
  
  return ( NIL(VOIDPTR) );
} /* end Ydset_enumerate_superset() */

/*----------------------------------
  Returns the parents of each subset
  ----------------------------------*/
/*** Enumerates the parents of each subset ***/
VOIDPTR Ydset_enumerate_parents( YDSETPTR dset  , BOOL startFlag )
{

  ELEMENTPTR ptr ;
  YTREEPTR dtree ;
  
  D("Ydset_enumerate_parents",
    printf("start Ydset_enumerate_parents\n");
  );

  dtree = dset->dtree ;
  if ( startFlag ) {
    if ( dset->parent_tree ) {
      Yrbtree_free(dset->parent_tree,NULL);
    }
    dset->parent_tree = Yrbtree_init(compare_parents);
      
    /* sort elements by set */
    Yrbtree_enumeratePush(dtree);
    for ( ptr = (ELEMENTPTR)Yrbtree_enumerate( dtree, TRUE); ptr;
	 ptr = (ELEMENTPTR)Yrbtree_enumerate( dtree, FALSE) ) {
      /* compress path so everyone will point to same parent */
      ptr = path_compression( ptr ) ; /* compress the path */

      /* only add to tree if it is a unique parent */
      if(!(Yrbtree_search( dset->parent_tree, ptr ))){
	  Yrbtree_insert(dset->parent_tree,ptr);
      }
    }
    Yrbtree_enumeratePop(dtree);
    
  }   /* end if start flag */
  
  ptr = (ELEMENTPTR)Yrbtree_enumerate( dset->parent_tree, startFlag);
  
  D("Ydset_enumerate_parents",
    printf("end Ydset_enumerate_parents\n");
  );

  if (ptr) {
    return ( ptr->data );
  }
  
  return ( NIL(VOIDPTR) );
  
} /* end Ydset_enumerate_parents() */

/*-----------------------------------------------------
  Ydset_enumerate_subset
  Enumerates all the elements of a subset.
  Items are sorted by set, so all elements of each set
  will be returned together.
  ------------------------------------------------------*/
VOIDPTR Ydset_enumerate_subset( YDSETPTR dset , VOIDPTR subsetData, BOOL startFlag )
{
  ELEMENTPTR parent ;
  ELEMENTPTR ptr ;
  YTREEPTR dtree ;
  
  D("Ydset_enumerate_subset",
    printf("start Ydset_enumerate_subset\n");
  );

  if ( startFlag ) {
    if ( dset->subset_tree ) {
      Yrbtree_free(dset->subset_tree,NULL);
    }
    dset->subset_tree = Yrbtree_init(compare_parents);
      
    Ydset_find(dset,subsetData);  /* compress the path */
    parent = dset_find_set(dset,subsetData);  /* find parent of subset */

    /* build a tree of all elements in the same set as subsetData */
    /* this means they have the same parent */
    dtree = dset->dtree ;
    Yrbtree_enumeratePush(dtree);
    for ( ptr = (ELEMENTPTR)Yrbtree_enumerate( dtree, TRUE); ptr;
	 ptr = (ELEMENTPTR)Yrbtree_enumerate( dtree, FALSE) ) {
      path_compression( ptr ) ; /* compress the path */
      if ( ptr->parent == parent ) {
        Yrbtree_insert(dset->subset_tree,ptr);
      }
    }
    Yrbtree_enumeratePop(dtree);
    
  }   /* end if start flag */
  
  ptr = (ELEMENTPTR)Yrbtree_enumerate( dset->subset_tree, startFlag);
  
  D("Ydset_enumerate_subset",
    printf("end Ydset_enumerate_subset\n");
  );

  if (ptr) {
    return ( ptr->data );
  }
  
  return ( NIL(VOIDPTR) );
  
} /* end Ydset_enumerate_subset */

/*-----------------------
  Ydset_search
  -----------------------*/
/*
  Searches for an item in the set basically like 
  rbtree_search works ; avoids makeset of the item
  which Ydset_find does by default.  Does not do
  path compression.  Good if no unions have been
  performed (during initialization) and interested
  in making all elements of the set unique.
  */
VOIDPTR Ydset_search( YDSETPTR dset, VOIDPTR data )
{
  ELEMENT dummy ;
  ELEMENTPTR ptr ;
  
  dsetS = dset ;
  dummy.data = data;
  if( ptr = (ELEMENTPTR) Yrbtree_search( dset->dtree, &dummy )){
    return( ptr->data ) ;
  }
  return( NIL(VOIDPTR) ) ;
} /* Ydset_search */

/*-------------------------------------------------------------
  Ydset_free
  Free all elements in the set and the set.
  This can now be used recursively.
  --------------------------------------------------------------*/
void Ydset_free( YDSETPTR dset , void (*userDelete)() )
{
  dsetS = dset ;
  dsetS->user_delete = userDelete ;
  Yrbtree_free(dset->dtree,dset_free_element);
  dset_free_trees( dset ) ;
  YFREE( dset ) ;
} /* end Ydset_free */

/*------------------------------------------------------------
  Ydset_empty
  Free all elements in the set but leaves the set intact
  -------------------------------------------------------------*/
void Ydset_empty( YDSETPTR dset , void (*userDelete)())
{
  dsetS = dset ;
  dsetS->user_delete = userDelete ;
  Yrbtree_empty(dset->dtree,dset_free_element);
  dset_free_trees( dset ) ;
  if ( dset->superset_tree ) {
    Yrbtree_free(dset->superset_tree,NULL);
    dset->superset_tree = NIL(YTREEPTR) ;
  }
  if ( dset->subset_tree ) {
    Yrbtree_free(dset->subset_tree,NULL);
    dset->subset_tree = NIL(YTREEPTR) ;
  }
  if ( dset->parent_tree ) {
    Yrbtree_free(dset->parent_tree,NULL);
    dset->parent_tree = NIL(YTREEPTR) ;
  }
} /* end Ydset_empty */

/*-----------------------
  -----------------------*/
VOIDPTR Ydset_union( dset, x, y )
YDSETPTR dset ;
VOIDPTR x, y ;
{
  ELEMENTPTR p1, p2, p3 ;
  
  D("Ydset_union",
    printf("start Ydset_union size = %d\n",Yrbtree_size(dset->dtree));
  );

  p1 = find( dset, x ) ;
  ASSERT( p1, "Ydset_union", "Problem with finding x\n" ) ;
  p2 = find( dset, y ) ;
  ASSERT( p2, "Ydset_union", "Problem with finding y\n" ) ;

  /* if p1 == p2 then we are already done */
  if ( p1 != p2 ) {
    p3 = link( p1, p2 );
  } else {
    p3 = p1;
  }
  
  D("Ydset_union",
    printf("end Ydset_union size = %d\n",Yrbtree_size(dset->dtree));
  );

  return( p3->data );
} /* end Ydset_union */

/*-------------------------------------------------
  Ydset_find_set()
  Searches for the set an item belongs to.
  This routine avoids makeset of the item
  which Ydset_find does by default
  -------------------------------------------------*/
VOIDPTR Ydset_find_set( YDSETPTR dset, VOIDPTR data )
{
  ELEMENTPTR ptr ;
  
  if((ptr = dset_find_set(dset, data))) {
    return( ptr->data ) ;
  }
  return( NIL(VOIDPTR) ) ;
} /* Ydset_find */

/*-------------------------------------------------
  Ydset_find
  Returns the set to which data is an element
  If the element is any set, a set is created
  --------------------------------------------------*/
VOIDPTR Ydset_find( YDSETPTR dset, VOIDPTR data )
{
  ELEMENTPTR ptr ;
  
  if((ptr = find( dset, data))){
    return( ptr->data ) ;
  }
  return( NIL(VOIDPTR) ) ;
} /* end Ydset_find */

/*-----------------------
  -----------------------*/
int Ydset_superset_size(YDSETPTR dset)
{
  return(Yrbtree_size(dset->dtree));
} /* end Ydset_superset_size */

/*-----------------------
  -----------------------*/
int Ydset_subset_size( YDSETPTR dset, VOIDPTR data )
{
  ELEMENT dummy ;
  ELEMENTPTR ptr ;
  
  if (dset){
    dsetS = dset ;
    dummy.data = data;
    if( ptr = (ELEMENTPTR)Yrbtree_search( dset->dtree, &dummy )){
      return( ptr->parent->size ) ;
    } else {
      M(ERRMSG,"Ydset_subset_size","Problem with finding data sent\n");
    }
  } else {
    M(ERRMSG,"Ydset_subset_size","NULL dset pointer (super set)\n");
  }
  return (-1);
} /* end Ydset_subset_size */

/*-----------------------
  Ydset_verify
  -----------------------*/
int Ydset_verify( YDSETPTR dset )
{
  ELEMENTPTR ptr ;
  int sizeIn;
  int sizeOut;  
  int rc = TRUE;
  YTREEPTR dtree ;
  
  dsetS = dset ;
  dtree = dset->dtree ;
  sizeIn = Ydset_superset_size(dset);

  /* exercise the dset tree */
  if ( !Yrbtree_verify(dtree) ) {
    M(ERRMSG,"Ydset_verify","dset tree verify failed\n");
    rc = FALSE;
  }

  Yrbtree_enumeratePush(dtree);
  /* walk through all elements user find routines to recover itself */
  for ( ptr = (ELEMENTPTR)Yrbtree_enumerate( dtree, TRUE); ptr;
       ptr = (ELEMENTPTR)Yrbtree_enumerate( dtree, FALSE) ) {
    if ( YcheckDebug(ptr) < sizeof(ELEMENT) ) { 
      M(ERRMSG,"Ydset_verify","corrupt element memory\n");
      rc = FALSE;
    }

  }
  Yrbtree_enumeratePop(dtree);
  
  sizeOut = Ydset_superset_size(dset);

  /* make sure the size has not changed, this tell us if makeset is called */
  if ( sizeIn != sizeOut ) {
    printf("sizeIn:%d sizeOut:%d\n",sizeIn,sizeOut);
    M(ERRMSG,"Ydset_verify","dset size changed during verification\n");
    rc = FALSE;
  }

  return( rc );
} /* end Ydset_verify() */

/*------------------------
  Ydset_dump
  ------------------------*/
void Ydset_dump(YDSETPTR dset, void (*printFunc)())
{
  VOIDPTR ptr ;
  int count = 1;
  ELEMENTPTR lastParent;
  ELEMENTPTR parent;
  ELEMENT dummy;
  
  D("Ydset_dump",
    printf("start Ydset_dump\n");
    );

  if ( !printFunc ) {
    M(WARNMSG,"Ydset_dump","NULL print function\n");
  }

  lastParent = &dummy;

  /* dump elements by set */
  printf("set contains %d items\n",Yrbtree_size(dset->dtree));
  for ( ptr = (VOIDPTR)Ydset_enumerate_superset( dset, TRUE); ptr;
       ptr = (VOIDPTR)Ydset_enumerate_superset( dset, FALSE) ) {
    
    parent = dset_find_set( dset,ptr );
    if ( parent != lastParent ) {
      printf("\n%d items in set #%d [parent ",parent->size,count++);
      if ( printFunc ) {
	(*printFunc)(parent);
      } else {
	printf("%ld ",parent);
      }
      printf("]:\n" ) ;
    }
    if ( printFunc ) {
      (*printFunc)(ptr);
    } else {
      printf("%ld ",ptr);
    }
    lastParent = parent;
  }

  printf("\n");

  D("Ydset_dump",
    printf("end Ydset_dump\n");
    );
} /* end Ydset_dump() */

/*-----------------------------------
  Ydset_interval
  enumerate all elements in the superset within an interval.
  -----------------------------------*/
VOIDPTR Ydset_interval( YDSETPTR dset, VOIDPTR low_key, VOIDPTR high_key, BOOL startFlag )
{

  ELEMENTPTR ptr ;

  D("Ydset_interval",
    printf("start Ydset_interval\n");
  );
  
  dsetS = dset ;
  /* note we have to take address here because we will do dereferencing later */
  ptr = (ELEMENTPTR) 
    Yrbtree_interval( dset->dtree, &low_key, &high_key, startFlag );

  D("Ydset_interval",
    printf("end Ydset_interval\n");
  );

  if( ptr ) {
    return( ptr->data ) ;
  } else {
    return ( NIL(VOIDPTR) );
  }

} /* end Ydset_interval() */

/*------------------------
  Ydset_dump_tree
  ------------------------*/
void Ydset_dump_tree(YDSETPTR dset, void (*print_key)())
{
  VOIDPTR ptr ;
  int count = 1;
  ELEMENTPTR lastParent;
  ELEMENTPTR parent;
  ELEMENT dummy;
  
  D("Ydset_dump_tree",
    printf("start Ydset_dump_tree\n");
  );

  if ( !print_key ) {
    M(WARNMSG,"Ydset_dump_tree","NULL print function\n");
  }

  dsetS = dset ;
  Yrbtree_dump( dset->dtree, print_key ) ;

  D("Ydset_dump_tree",
    printf("end Ydset_dump_tree\n");
  );
} /* end Ydset_dump_tree() */

/* Ydset_interval push and pop are also possible... Implement when
we need them....Just call Yrbtree push and pop. */

