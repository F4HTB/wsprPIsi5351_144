#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include "si5351a.h"
#include "I2CWrapper.c"

#define I2C_WRITE 0x60
#define I2C_READ  0x61
// #define I2C_BUS 1
int I2C_BUS = 1;
//
// Set up specified PLL with mult, num and denom
// mult is 15..90
// num is 0..1,048,575 (0xFFFFF)
// denom is 0..1,048,575 (0xFFFFF)
//
void setupPLL(int i2c, uint8_t pll, uint8_t mult, uint32_t num, uint32_t denom)
{
	
    uint32_t P1;					// PLL config register P1
    uint32_t P2;					// PLL config register P2
    uint32_t P3;					// PLL config register P3

    P1 = (uint32_t)(128 * ((float)num / (float)denom));
    P1 = (uint32_t)(128 * (uint32_t)(mult) + P1 - 512);
    P2 = (uint32_t)(128 * ((float)num / (float)denom));
    P2 = (uint32_t)(128 * num - denom * P2);
    P3 = denom;

    I2CWrapperWriteByte(i2c, pll + 0, (P3 & 0x0000FF00) >> 8);
    I2CWrapperWriteByte(i2c, pll + 1, (P3 & 0x000000FF));
    I2CWrapperWriteByte(i2c, pll + 2, (P1 & 0x00030000) >> 16);
    I2CWrapperWriteByte(i2c, pll + 3, (P1 & 0x0000FF00) >> 8);
    I2CWrapperWriteByte(i2c, pll + 4, (P1 & 0x000000FF));
    I2CWrapperWriteByte(i2c, pll + 5, ((P3 & 0x000F0000) >> 12) | ((P2 & 0x000F0000) >> 16));
    I2CWrapperWriteByte(i2c, pll + 6, (P2 & 0x0000FF00) >> 8);
    I2CWrapperWriteByte(i2c, pll + 7, (P2 & 0x000000FF));
}

//
// Set up MultiSynth with integer divider and R divider
// R divider is the bit value which is OR'ed onto the appropriate register, it
// is a #define in si5351a.h
//
void setupMultisynth(int i2c, uint8_t synth, uint32_t divider, uint8_t rDiv)
{
	
    uint32_t P1;					// Synth config register P1
    uint32_t P2;					// Synth config register P2
    uint32_t P3;					// Synth config register P3

    P1 = 128 * divider - 512;
    P2 = 0;							// P2 = 0, P3 = 1 forces an integer value for the divider
    P3 = 1;

    I2CWrapperWriteByte(i2c, synth + 0,   (P3 & 0x0000FF00) >> 8);
    I2CWrapperWriteByte(i2c, synth + 1,   (P3 & 0x000000FF));
    I2CWrapperWriteByte(i2c, synth + 2,   ((P1 & 0x00030000) >> 16) | rDiv);
    I2CWrapperWriteByte(i2c, synth + 3,   (P1 & 0x0000FF00) >> 8);
    I2CWrapperWriteByte(i2c, synth + 4,   (P1 & 0x000000FF));
    I2CWrapperWriteByte(i2c, synth + 5,   ((P3 & 0x000F0000) >> 12) | ((P2 & 0x000F0000) >> 16));
    I2CWrapperWriteByte(i2c, synth + 6,   (P2 & 0x0000FF00) >> 8);
    I2CWrapperWriteByte(i2c, synth + 7,   (P2 & 0x000000FF));
}

//
// Switches off Si5351a output
// Example: si5351aOutputOff(SI_CLK0_CONTROL);
// will switch off output CLK0
//
void si5351aOutputOff(uint8_t clk)
{

	int i2c = I2CWrapperOpen(I2C_BUS,I2C_WRITE);
	I2CWrapperWriteByte(i2c, clk,0x80 );
	I2CWrapperClose(i2c);

}

void si5351aOutputOffAll()
{
	int i2c = I2CWrapperOpen(I2C_BUS,I2C_WRITE);
    si5351aOutputOff(SI_CLK0_CONTROL);
	si5351aOutputOff(SI_CLK1_CONTROL);
	si5351aOutputOff(SI_CLK2_CONTROL);
	I2CWrapperClose(i2c);

}



void si5351aOutputOn(uint8_t clk)
{
	int i2c = I2CWrapperOpen(I2C_BUS,I2C_WRITE);
	I2CWrapperWriteByte(i2c, clk, si5351alastpower );
	I2CWrapperClose(i2c);
	
}

void si5351ainit() //Frequency is in centiHz
{
	int i2c = I2CWrapperOpen(I2C_BUS,I2C_WRITE);
	
	// Initialize the CLK outputs according to flowchart in datasheet
	// First, turn them off
	I2CWrapperWriteByte(i2c, SI_CLK0_CONTROL, 0x80);
	I2CWrapperWriteByte(i2c, SI_CLK1_CONTROL, 0x80);
	I2CWrapperWriteByte(i2c, SI_CLK2_CONTROL, 0x80);

	// Turn the clocks back on...
	I2CWrapperWriteByte(i2c, SI_CLK0_CONTROL, 0x0c);
	I2CWrapperWriteByte(i2c, SI_CLK1_CONTROL, 0x0c);
	I2CWrapperWriteByte(i2c, SI_CLK2_CONTROL, 0x0c);



	uint8_t reg_val = 0;
	reg_val = I2CWrapperReadByte(i2c,SI_CLK0_CONTROL);
	reg_val &= ~(SI5351_CLK_PLL_SELECT);
	I2CWrapperWriteByte(i2c, SI_CLK0_CONTROL, reg_val);
	reg_val = I2CWrapperReadByte(i2c,SI_CLK1_CONTROL);
	reg_val &= ~(SI5351_CLK_PLL_SELECT);
	I2CWrapperWriteByte(i2c, SI_CLK1_CONTROL, reg_val);
	reg_val = I2CWrapperReadByte(i2c,SI_CLK2_CONTROL);
	reg_val &= ~(SI5351_CLK_PLL_SELECT);
	I2CWrapperWriteByte(i2c, SI_CLK2_CONTROL, reg_val);
	
	// Reset the VCXO param
	I2CWrapperWriteByte(i2c, SI5351_VXCO_PARAMETERS_LOW, 0);
	I2CWrapperWriteByte(i2c, SI5351_VXCO_PARAMETERS_MID, 0);
	I2CWrapperWriteByte(i2c, SI5351_VXCO_PARAMETERS_HIGH, 0);

	// Then reset the PLLs
	I2CWrapperWriteByte(i2c, SI_PLL_RESET, 0xA0);	
	
	// Set crystal load capacitance
	I2CWrapperWriteByte(i2c, SI_CRYSTAL_LOAD,(SI_CRYSTAL_LOAD_8PF & SI_CRYSTAL_LOAD_MASK) | 0b00010010 );
	
	// shut off the spread spectrum by default, DWaite contibuted code  
    uint8_t regval;
    regval = I2CWrapperReadByte(i2c,SI_SSC_PARAM0);
    regval &= ~0x80;  // set bit 7 LOW to turn OFF spread spectrum mode
	I2CWrapperWriteByte(i2c, SI_SSC_PARAM0,regval );
    si5351aOutputOffAll();  // power off all the outputs
	I2CWrapperClose(i2c);
}

void SetPower (uint8_t power, uint8_t clk){
	if (power == 0 || power > 4){power = 1;}
		switch (power){
		case 1:
		si5351alastpower = 76;// CLK0  Contrôle du courant de sortie -> 2mA
		break;
		case 2:
		si5351alastpower = 77;// CLK0  Contrôle du courant de sortie -> 4mA
		break;
		case 3:
		si5351alastpower = 78;// CLK0  Contrôle du courant de sortie -> 6mA
		break;
		case 4:
		si5351alastpower = 79;// CLK0  Contrôle du courant de sortie -> 8mA
		break;
	}
	si5351aOutputOn(SI_CLK0_CONTROL);
}

void si5351aSetFrequency(uint64_t frequency) //Frequency is in centiHz
{
    uint64_t pllFreq;
    uint32_t xtalFreq = XTAL_FREQ;
    uint32_t l;
    float f;
    uint8_t mult;
    uint32_t num;
    uint32_t denom;
    uint32_t divider;
	uint8_t rDiv;

	int i2c = I2CWrapperOpen(I2C_BUS,I2C_WRITE);

	if (frequency > 100000000ULL) {
		rDiv = SI_R_DIV_1;
		divider = 90000000000 / frequency;// Calculate the division ratio. 900,000,000 is the maximum internal 
		// PLL frequency: 900MHz
		if (divider % 2) divider--;		// Ensure an even integer division ratio

		pllFreq = divider * frequency;	// Calculate the pllFrequency: the divider * desired output frequency

	}else{
		rDiv = SI_R_DIV_128;
		divider = 90000000000 / (frequency * 128);// Calculate the division ratio. 900,000,000 is the maximum internal 
		// PLL frequency: 900MHz
		if (divider % 2) divider--;		// Ensure an even integer division ratio

		pllFreq = divider * frequency * 128;	// Calculate the pllFrequency: the divider * desired output frequency
	}
		mult = pllFreq / (xtalFreq * 100);		// Determine the multiplier to get to the required pllFrequency
		l = pllFreq % (xtalFreq * 100);			// It has three parts:
		f = l;							// mult is an integer that must be in the range 15..90
		f *= 1048575;					// num and denom are the fractional parts, the numerator and denominator
		f /= xtalFreq;					// each is 20 bits (range 0..1048575)
		num = f;						// the actual multiplier is  mult + num / denom
		denom = 1048575;				// For simplicity we set the denominator to the maximum 1048575
		num = num / 100;		
	

    // Set up PLL A with the calculated multiplication ratio
    setupPLL(i2c, SI_SYNTH_PLL_A, mult, num, denom);
    // Set up MultiSynth divider 0, with the calculated divider. 
    // The final R division stage can divide by a power of two, from 1..128. 
    // reprented by constants SI_R_DIV1 to SI_R_DIV128 (see si5351a.h header file)
    // If you want to output frequencies below 1MHz, you have to use the 
    // final R division stage
    setupMultisynth(i2c, SI_SYNTH_MS_0, divider, rDiv);
    // Reset the PLL. This causes a glitch in the output. For small changes to 
    // the parameters, you don't need to reset the PLL, and there is no glitch
    
	// I2CWrapperWriteByte(i2c, SI_PLL_RESET, 0xA0);	
    
	// Finally switch on the CLK0 output (0x4F)
    // and set the MultiSynth0 input to be PLL A
    //I2CWrapperWriteByte(i2c, SI_CLK0_CONTROL, 0x4F | SI_CLK_SRC_PLL_A);
	SetPower(1,SI_CLK0_CONTROL);
	
    I2CWrapperClose(i2c);
}

void si5351aSetsetupPLLandsetupMultisynth(uint8_t mult, uint32_t num, uint32_t denom, uint32_t divider, uint8_t rDiv)
{
	int i2c = I2CWrapperOpen(I2C_BUS,I2C_WRITE);
	setupPLL(i2c, SI_SYNTH_PLL_A, mult, num, denom);
	setupMultisynth(i2c, SI_SYNTH_MS_0, divider, rDiv);
	SetPower(1,SI_CLK0_CONTROL);
	I2CWrapperClose(i2c);
}



