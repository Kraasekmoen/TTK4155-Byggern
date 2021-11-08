/*
 * globals.h
 *
 * Created: 12.10.2021 16:34:04
 *  Author: sindrbov
 */ 


#ifndef GLOBALS_H_
#define GLOBALS_H_

#define is_bit_set(sfr, bit) (sfr & (1 << bit))
#define loop_until_bit_is_set(sfr, bit) while (!(sfr & (1 << bit)))



#endif /* GLOBALS_H_ */