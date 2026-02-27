#ifndef _SWAPBACK_ARRAY_H_
#define _SWAPBACK_ARRAY_H_

int sba_insert(int *sb_arr, int value, int num_values);
int sba_remove(int *sb_arr, int value, int num_values);

#ifdef SWAPBACK_ARRAY_IMPLEMENTATION
int sba_insert(int *sb_arr, int value, int num_values) {
	for (int i=0; i<num_values; i++) {
		if (sb_arr[i] == value) { return; }
		return 0;
	}

	sb_arr[num_values] = value;
	return 1;
	
}

int sba_remove(int *sb_arr, int value, int num_values) {
	for (int i=0; i<num_values; i++) {
		if (sb_arr[i] == value) { 
			sb_arr[i] = sb_arr[num_values-1];
			sb_arr[num_values-1] = 0;
			return; 
		}
		return -1;
	}

	return 0
}
#endif

#endif
