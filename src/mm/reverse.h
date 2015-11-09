#ifdef HAVE_REVERSE

#pragma once
#ifndef __REVERSE_H
#define __REVERSE_H

#include <sys/types.h>

//#include <mm/dymelor.h>


#define REVWIN_SIZE 1024 * 4	//! Defalut size of the reverse window which will contain the reverse code
#define REVWIN_CODE_SIZE (REVWIN_SIZE - sizeof(revwin_t))
#define REVWIN_STACK_SIZE 1024	//! Default size of the emultated reverse stack window on the heap space
#define REVWIN_RZONE_SIZE 100		//! Size of the red zone in the reverse window that will be skipped to prevent cache misses
#define RANDOMIZE_REVWIN 0 			//! Activate the rendomization of the addresses used by revwin to prevent cache misses

#define HMAP_SIZE		32768		//! Default size ot the address hash map to handle colliding mov addresses
#define HMAP_INDEX_MASK		0xffffffc0	//! Most significant 10 bits are used to index quad-word which contains address bit
#define HMAP_OFFSET_MASK	0x3f	//! Least significant 6 bits are used to intercept address presence
#define HMAP_OFF_MASK_SIZE	6


//#define current_win map->map[tid]


#define CLUSTER_SIZE 64
#define ADDRESS_PREFIX -CLUSTER_SIZE
#define PREFIX_HEAD_SIZE 1024

#define REVERSE_THRESHOLD 0.15

#define STRATEGY_SINGLE 0
#define STRATEGY_CHUNK 1



typedef struct strategy_t {
	int current;
	int switches;
} strategy_t;


typedef struct _cache_entry {
	unsigned long long address;		//! Actual address
	unsigned int touches;			//! Number of times this address has been touched so far
} cache_entry;


typedef struct _prefix_head {
	unsigned long long prefix;		//! Base address prefix of this cluster head
	cache_entry cache[CLUSTER_SIZE];	//! List of registered addresses with current prefix
	unsigned int count;				//! Number of total address in this cluster
	double fragmentation;			//! Internal fragmentation of addresses in the cluster, i.e. how much they are sparse
	unsigned int contiguous;		//! Maximum number of contiguous addresses registered
} prefix_head;


/**
 *
 */
typedef struct _cluster_cache {
	prefix_head cluster[PREFIX_HEAD_SIZE];
	unsigned int count;
} cluster_cache;


// TODO: rimuovere
/**
 * Emulated stack window descriptor
 */
typedef struct _stackwin {
	void *base;			//! Base of the stack
	void *pointer;		//! Current top pointer
	size_t size;		//! Size of the window
	void *original_base;		//! Origninal base pointer of the real stack
	void *original_pointer;		//! Original stack pointer of the real one
} stackwin;


/**
 * Descriptor of a single reverse window
 */
 typedef struct _revwin_t {
	void *top;				//! Pointer to the first instruction byte to be executed
	void *base;				//! Pointer to the logic base of the window
	unsigned int offset;	//! A random offset used to prevent cache lines to be alligned
	size_t size;			//! The actual size of the reverse window executable portion
	char code[];			//! Placeholder for the actual executable reverse code, i.e. from this point there is code
} revwin_t;
// TODO: cambiare nome alla struttura

// typedef struct _revwin {
// 	size_t size;		//! The actual size of the reverse window
// 	int flags;		//! Creation flags
// 	int prot;		//! Protection flags
// 	unsigned int offset;		//! A random offset used to prevent cache lines to be alligned
// 	void *base;		//! Address to which it resides
// 	void *top;		//! Pointer to the new actual next byte in the reverse window
// 	void *estack;	//! Emulated stack window
// 	void *orig_stack;	//! Orignal function's stack
// } revwin;


/**
 * Memory management layout of reverse windows
 */
typedef struct _revwin_mmap {
	size_t size_self;		//! The size of this structure
	void *address;			//! The starting address of the reverse mapping area
	size_t size;			//! The actual size allocated for the mmap allocated area
	unsigned int revwin_count;	//! The number of the reverse windows actually allocated
	revwin_t *map[];			//! Array of pointers to the revwin state for each thread
} revwin_mmap;


/**
 * Software cache to track reversed addresses
 */
 typedef struct _revwin_cache {
 	size_t reversed_bytes;		//! The number of bytes that have been reversed so far
 	double fragmentation;		//! The average percentage of reversing fragmentation within a malloc_area
// 	malloc_area touched[1024];	//! Keep track of which malloc_areas' have been touched so far
 } revwin_cache;



// ========================================= //
// ================== API ================== //


extern void reverse_init(size_t revwin_size);

extern void reverse_fini(void);

//extern void revwin_init(void);


/**
 * This will allocate a window on the HEAP of the exefutable file in order
 * to write directly on it the reverse code to be executed later on demand.
 *
 * @param size The size to which initialize the new reverse window. If the size paramter is 0, default
 * value will be used (REVERSE_WIN_SIZE)
 *
 * @return The address of the created window
 *
 * Note: mmap is invoked with both write and executable access, but actually
 * it does not to be a good idea since could be less portable and further
 * could open to security exploits.
 */
//extern void *create_new_revwin(size_t size);



/**
 * Initializes locally a new reverse window.
 *
 */
extern revwin_t *revwin_create(void);


/**
 * Free the reverse window passed as argument.
 *
 * @param window A pointer to a reverse window
 */
extern void revwin_free(revwin_t *win);


/**
 * Reset local reverse window
 */
extern void revwin_reset(revwin_t *win);


/**
 * Will execute an undo event
 *
 */
extern void execute_undo_event(void);


/**
 * Prints some statistics of the software cache
 */
extern void print_cache_stats();


#endif /* __REVERSE_H */
#endif /* HAVE_REVERSE */