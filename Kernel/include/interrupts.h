 
#ifndef INTERRUPS_H_
#define INTERRUPS_H_
#include <registers.h>
#include "idtLoader.h"
typedef struct register_struc_t register_struct_t;

void _irq00Handler(void);
void _irq01Handler(void);
void _irq02Handler(void);
void _irq03Handler(void);
void _irq04Handler(void);
void _irq05Handler(void);
void _irq60Handler(void);

void _exception0Handler(void);
void _exception6Handler(void);
void _exception13Handler(void);
void print_reg_asm(void);
void save_state(void);
void save_reg_state_asm( register_struct_t * proc_rec);

void _cli(void);

void _sti(void);

void _hlt(void);

void pic_master_mask(uint8_t mask);

void pic_slave_mask(uint8_t mask);

void haltcpu(void);

#endif 
