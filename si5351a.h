#ifndef SI5351A_H
#define SI5351A_H

#include <inttypes.h>

#define SI_CRYSTAL_LOAD             183
#define SI_CRYSTAL_LOAD_MASK        (3<<6)
#define SI_CRYSTAL_LOAD_0PF         (0<<6)
#define SI_CRYSTAL_LOAD_6PF         (1<<6)
#define SI_CRYSTAL_LOAD_8PF         (2<<6)
#define SI_CRYSTAL_LOAD_10PF        (3<<6)

#define SI_CLK0_CONTROL				16			// Register definitions
#define SI_CLK1_CONTROL				17
#define SI_CLK2_CONTROL				18
#define SI_SYNTH_PLL_A				26
#define SI_SYNTH_PLL_B				34
#define SI_SYNTH_MS_0				42
#define SI_SYNTH_MS_1				50
#define SI_SYNTH_MS_2				58

#define SI_PLL_RESET				177
#define SI5351_PLL_RESET_B              (1<<7)
#define SI5351_PLL_RESET_A              (1<<5)

#define SI_SSC_PARAM0   		    149   
#define SI5351_CLK_PLL_SELECT      	(1<<5)   

#define SI5351_VXCO_PARAMETERS_LOW      162
#define SI5351_VXCO_PARAMETERS_MID      163
#define SI5351_VXCO_PARAMETERS_HIGH     164

#define SI_R_DIV_1		0b00000000			// R-division ratio definitions
#define SI_R_DIV_2		0b00010000
#define SI_R_DIV_4		0b00100000
#define SI_R_DIV_8		0b00110000
#define SI_R_DIV_16		0b01000000
#define SI_R_DIV_32		0b01010000
#define SI_R_DIV_64		0b01100000
#define SI_R_DIV_128		0b01110000

#define SI_CLK_SRC_PLL_A	0b00000000
#define SI_CLK_SRC_PLL_B	0b00100000

#define XTAL_FREQ	25000000			// Crystal frequency

uint8_t si5351alastpower = 76;
void si5351aOutputOff(uint8_t clk);
void si5351aSetFrequency(uint64_t frequency);
void si5351ainit(void);
void si5351areset(void);

#endif //SI5351A_H
