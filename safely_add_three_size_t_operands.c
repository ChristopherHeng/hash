/*
	safely_add_three_size_t_operands.c
	Copyright 2025 by Christopher Heng. All rights reserved.
*/

#include <stddef.h> // size_t
#include <stdint.h> // SIZE_MAX
#include "hash.h"

/*
	safely_add_three_size_t_operands
	Adds 3 size_t values, first, second and third, and stores them in result.
	The function does NOT modify *result if result cannot safely hold the sum.
	Returns
		0	success
		-1	the operation will wrap around; *result remains unchanged
*/
int safely_add_three_size_t_operands( size_t * result, size_t first, size_t second, size_t third )
{
	size_t temp ;

	if ((SIZE_MAX - first) >= second) {
		temp = first + second ;
		if ((SIZE_MAX - temp) >= third) {
			*result = temp + third ;
			return 0 ;
		}
	}
	return -1 ;
}
