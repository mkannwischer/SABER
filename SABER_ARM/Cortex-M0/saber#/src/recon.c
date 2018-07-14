#include <stdio.h>
#include "recon.h"

static uint16_t floor_special(uint16_t a)
{

	int sign;
	uint16_t b;
	int16_t abs;
	int16_t sign2, temp;


	sign = (a>>15) & 0x01;
	
        abs = sign*(0-a) + (1-sign)*a;      // absolute value of a in constant time;

		
	//b = (abs>>9); 			   
	b = abs; 			    

	
	// when a is -ve, we need to do more computation (but in constant time)
	temp = abs & 0x01ff;	// least 9 bits of a;

	temp = -temp;		// if temp was 0, then result is 0; otherwise -ve. so the sign bit tells it.

	

	sign2 = (temp>>15) & 0x01;
	
	
	abs = abs + sign*sign2;

	b = sign*(0-abs) + (1-sign)*b;

	b = (b>>9) & 0x01;

	
	return(b);
}





// generates binary K2 from a_avx and recon_data
void recon(uint16_t a, uint16_t recon_data, uint16_t *K2)
{
    uint16_t temp;

    //temp = a - (recon_data<<6) - 2^5 + 2^8;
    temp = (recon_data << 6);   // temp = recon_data<<6
    temp = (a - temp);        	// temp = a - (recon_data<<7)
    temp = (temp - 32);        	// temp = a - (recon_data<<7) - 2^5
    temp = (temp + 256);        // temp = a - (recon_data<<7) - 2^5 + 2^8

    //K2 = floor(temp/2^10);
    *K2 = floor_special(temp);

}

void ReconDataGen(uint16_t *vprime, unsigned char *rec_c)
{

	uint16_t ciphertext_temp0, ciphertext_temp1;
	int16_t j;

	for(j=0;j<SABER_N/2;j++)
	{
		ciphertext_temp0 = (vprime[2*j] >> 6);
		ciphertext_temp1 = (vprime[2*j+1] >> 6);
		rec_c[j]= (ciphertext_temp0 & 0x0f) | ( (ciphertext_temp1 & 0x0f)<<4 );
	}

}


void Recon(uint16_t *recon_data, const unsigned char *recon_ar, unsigned char *message_byte)
{
	int16_t j;
	uint16_t coeff, shift_amount, message_dec_bit;
 
	for(j=0; j<32; j++)
	message_byte[j] = 0;

	for(j=0;j<SABER_N/2;j++)
	{
		shift_amount = (2*j) %8;

		coeff = recon_ar[j] & 0x0f;
		recon(recon_data[2*j], coeff, &message_dec_bit);
		message_byte[j/4] = message_byte[j/4] | (message_dec_bit<<shift_amount);
		
		shift_amount++;
		coeff = (recon_ar[j]>>4) & 0x0f;
		recon(recon_data[2*j+1], coeff, &message_dec_bit);
		message_byte[j/4] = message_byte[j/4] | (message_dec_bit<<shift_amount);
	}

}

