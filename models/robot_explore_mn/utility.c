#include <ROOT-Sim.h>
#include <math.h>

unsigned int get_tot_regions(void){
	#ifdef ECS_TEST
		unsigned int tot_region,check_value;

		tot_region = n_prc_tot * PERC_REGION;
		check_value = sqrt(tot_region);

		if(check_value * check_value != tot_region) {
		       printf("Hexagonal map wrongly specified!\n");
		}
			
		return tot_region;
	#else
		return TOT_REG;
	#endif
}

unsigned int get_tot_agents(void){
	return n_prc_tot - get_tot_regions();
}

bool is_agent(unsigned int me){

	if(me < get_tot_regions()){
		return false;
	}
	return true;
}


unsigned int random_region(void){
	return get_tot_regions()*Random();
}

unsigned char get_obstacles(void){
	return 0;
}

unsigned int get_region(unsigned int me, unsigned int obstacle,unsigned int agent){
	unsigned int edge,temp,tot_region;
	double random;
	tot_region = get_tot_regions();
	edge = sqrt(tot_region);

	random = Random();

	// CASE corner up-left
	if(me == 0){
		temp = 2 * random;
		switch(temp){
			case 0: // go rigth
				return me + 1;
			case 1:	// go down
				return me + edge;	
		}
	}
	// CASE corner up-rigth
	else if(me == edge - 1){
		temp = 2 * random;
                switch(temp){
                        case 0: // go left
                                return me - 1;
                        case 1: // go down
                                return me + edge;
                }
	}
	// CASE corner down-rigth
	else if(me == tot_region - 1){
		temp = 2 * random;
                switch(temp){
                        case 0: // go left
                                return me - 1;
                        case 1: // go up
                                return me - edge;
                }
        }
	// CASE corner down-left
	else if(me == tot_region - edge){
		temp = 2 * random;
                switch(temp){
                        case 0: // go rigth
                                return me + 1;
                        case 1: // go up
                                return me - edge;
                }
        }
	// CASE first row
	else if(me <  edge){
		temp = 3 * random;
                switch(temp){
                        case 0: // go rigth
                                return me + 1;
                        case 1: // go down
                                return me + edge;
			case 2:
				// go left
				return me - 1;
                }
        }
	// CASE last row
        else if(me > tot_region - edge && me <  tot_region - 1){
		temp = 3 * random;
                switch(temp){
                        case 0: // go rigth
                                return me + 1;
                        case 1: // go up
                                return me - edge;
                        case 2:
                                // go left
                                return me - 1;
                }
        }
	// CASE first column
        else if(me % edge == 0){
		temp = 3 * random;
                switch(temp){
                        case 0: // go rigth
                                return me + 1;
                        case 1: // go down
                                return me + edge;
                        case 2:
                                // go up
                                return me - edge;
                }
        }
	// CASE last column
	else if((me+1) % edge == 0){
		temp = 3 * random;
                switch(temp){
                        case 0: // go left
                                return me - 1;
                        case 1: // go down
                                return me + edge;
                        case 2:
                                // go up
                                return me - edge;
                }
        }
	// Normal case
	else{
		temp = 4 * random;
                switch(temp){
                        case 0: // go rigth
                                return me + 1;
                        case 1: // go down
                                return me + edge;
                        case 2:
                                // go up
                                return me - edge;
			case 3:
				// go left
				return me - 1;
                }
        }
	

	return me;
}

bool check_termination(lp_agent_t *agent){
	double regions = (double)agent->count;
	double tot_region = get_tot_regions();
	double result = regions/tot_region;
	
	if(result >= VISITED || agent->complete)
		return true;

	return false;
}

double percentage(lp_agent_t *agent){
	double regions = (double)agent->count;
	double tot_region = get_tot_regions();
	double result = (regions/tot_region)*100;

	return result;
}

void copy_map(unsigned char *pointer, int n, unsigned char (vector)[n]){
	unsigned int i;
	
	for(i=0; i<get_tot_regions(); i++){
		if(!BITMAP_CHECK_BIT(pointer,i) && BITMAP_CHECK_BIT(vector,i))
			BITMAP_SET_BIT(pointer,i);
		else if(BITMAP_CHECK_BIT(pointer,i) && !BITMAP_CHECK_BIT(vector,i))
			BITMAP_SET_BIT(vector,i);
	}
}
