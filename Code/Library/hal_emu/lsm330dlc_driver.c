/*
 * itg3200_driver.c
 *
 * Created: 18/05/2012 17:57:46
 *  Author: sfx
 */ 


#include "lsm330dlc_driver.h"
#include "print_util.h"
//#include "twim.h"

static volatile lsm_gyro_data_t lsm_gyro_outputs;
static volatile lsm_acc_data_t  lsm_acc_outputs;



void init_lsm330_acc(void) {
	
	

}

void init_lsm330_gyro(void) {

}

lsm_get_acc_config() {
}
lsm_get_gyro_config() {
}


void init_lsm330(void) {

}





lsm_acc_data_t* lsm330_get_acc_data(void) {
	
	return &lsm_acc_outputs;
}


lsm_gyro_data_t* lsm330_get_gyro_data(void) {
	return &lsm_gyro_outputs;
}