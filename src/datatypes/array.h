/**
 *			Copyright (C) 2008-2018 HPDCS Group
 *			http://www.dis.uniroma1.it/~hpdcs
 *
 *
 * This file is part of ROOT-Sim (ROme OpTimistic Simulator).
 *
 * ROOT-Sim is free software; you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation; only version 3 of the License applies.
 *
 * ROOT-Sim is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * ROOT-Sim; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * @file array.h
 * @brief This is the implementation of a dynamic array, used for managing various data structures
 * @author Andrea Piccione
 * @date 18 June 2018
 */

#ifndef ARRAY_H_
#define ARRAY_H_

#include <mm/dymelor.h>

#include <stddef.h>
#include <memory.h>

// (TODO: maybe add a define to choose between wrapped and actual malloc calls)
// TODO: add some type checking and size checking
// TODO: add all the other useful functions

#define INIT_SIZE_ARRAY 8

#define rootsim_array(type) \
		struct { \
			type *items; \
			size_t count, capacity; \
		}

// you can use the array to directly index items, but do at your risk and peril
#define array_items(self) ((self).items)

#define array_count(self) ((self).count)

#define array_capacity(self) ((self).capacity)

#define array_shrink(self) ({ \
		if (array_count(self) > INIT_SIZE_ARRAY && array_count(self) * 3 <= array_capacity(self)) { \
			array_capacity(self) /= 2; \
			array_items(self) = __wrap_realloc(array_items(self), array_capacity(self) * sizeof(*array_items(self))); \
			if(!array_items(self))\
				rootsim_error(true, "Realloc failed during array shrinking!"); \
		} \
	})

#define array_expand(self) ({ \
		if(array_count(self) >= array_capacity(self)){\
			array_capacity(self) *= 2; \
			array_items(self) = __wrap_realloc(array_items(self), array_capacity(self) * sizeof(*array_items(self))); \
			if(!array_items(self))\
				rootsim_error(true, "Realloc failed during array expansion!"); \
		} \
	})

#define array_new(type) ({ \
		rootsim_array(type) *__newarr; \
		\
		__newarr = __wrap_malloc(sizeof(*__newarr)); \
		if(!__newarr)\
			rootsim_error(true, "Malloc failed during array_new!"); \
		\
		array_init(*__newarr);\
		__newarr; \
	})

#define array_free(self) ({ \
		__wrap_free(array_items(self)); \
		__wrap_free(&(self)); \
	})

#define array_init(self) ({ \
		array_capacity(self) = INIT_SIZE_ARRAY; \
		array_items(self) = __wrap_malloc(array_capacity(self) * sizeof(*array_items(self))); \
		if(!array_items(self))\
			rootsim_error(true, "Malloc failed during array_init!"); \
		\
		array_count(self) = 0; \
	})

#define array_fini(self) ({ \
		__wrap_free(array_items(self)); \
	})

#define array_push(self, elem) ({ \
		array_expand(self); \
		array_items(self)[array_count(self)] = (elem); \
		array_count(self)++; \
	})

#define array_pop(self) ({ \
		__typeof__(*array_items(self)) __popval; \
		memset(&__popval, 0, sizeof(__popval)); \
		if(array_count(self)){\
			array_count(self)--; \
			__popval = array_items(self)[array_count(self)]; \
			array_shrink(self); \
		} \
		__popval; \
	})

#define array_add_at(self, i, elem) ({ \
		if(array_count(self) > (i)){ \
			array_expand(self); \
			memmove(&(array_items(self)[(i)+1]), &(array_items(self)[(i)]), sizeof(*array_items(self)) * (array_count(self)-(i))); \
			array_items(self)[(i)] = (elem); \
			array_count(self)++; \
		} \
	})

#define array_remove_at(self, i) ({ \
		__typeof__(*array_items(self)) __rmval; \
		memset(&__rmval, 0, sizeof(__rmval)); \
		if(array_count(self) > (i)){ \
			array_count(self)--; \
			__rmval = array_items(self)[(i)]; \
			memmove(&(array_items(self)[(i)]), &(array_items(self)[(i)+1]), sizeof(*array_items(self)) * (array_count(self)-(i))); \
			array_shrink(self); \
		} \
		__rmval; \
	})

#define array_remove(self, elem) ({ \
		size_t __cntr = array_count(self); \
		while(__cntr--){ \
			if(array_items(self)[__cntr] == (elem)){\
				array_remove_at(self, __cntr); \
				break; \
			} \
		} \
	})
//this isn't checked CARE! TODO add checking
#define array_peek(self) (array_items(self)[array_count(self)-1])
//this isn't checked CARE! TODO add checking
#define array_get_at(self, i) (array_items(self)[i])

#define array_empty(self) (array_count(self) == 0)

#define array_required_bytes_dump(self) ({ \
		sizeof(size_t) + array_count(self)*sizeof(*array_items(self)); \
	})

#define array_dump(self, mem_area) ({ \
		memcpy((mem_area), &array_count(self), sizeof(size_t)); \
		(mem_area) = ((unsigned char *)(mem_area)) + sizeof(size_t); \
		memcpy((mem_area), array_items(self), array_count(self) * sizeof(*array_items(self))); \
		mem_area = ((unsigned char *)(mem_area)) + array_count(self) * sizeof(*array_items(self)); \
	})

#define array_load(self, mem_area) ({ \
		memcpy(&array_count(self), (mem_area), sizeof(size_t)); \
		(mem_area) = ((unsigned char *)(mem_area)) + sizeof(size_t); \
		array_capacity(self) = array_count(self); \
		\
		array_items(self) = __wrap_malloc(array_capacity(self) * sizeof(*array_items(self))); \
		if(!array_items(self))\
			rootsim_error(true, "Malloc failed during array_load!"); \
		\
		memcpy(array_items(self), (mem_area), array_capacity(self) * sizeof(*array_items(self))); \
		(mem_area) = ((unsigned char *)(mem_area)) + (array_capacity(self) * sizeof(*array_items(self))); \
	})

#endif /* ARRAY_H_ */
