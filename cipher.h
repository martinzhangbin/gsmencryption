#include "random.h"


/* Masks for the three shift registers */
#define R1MASK	0x07FFFF /* 19 bits, numbered 0..18 */
#define R2MASK	0x3FFFFF /* 22 bits, numbered 0..21 */
#define R3MASK	0x7FFFFF /* 23 bits, numbered 0..22 */

/* Middle bit of each of the three shift registers, for clock control */
#define R1MID	0x000100 /* bit 8 */
#define R2MID	0x000400 /* bit 10 */
#define R3MID	0x000400 /* bit 10 */

/* Feedback taps, for clocking the shift registers.
* These correspond to the primitive polynomials
* x^19 + x^5 + x^2 + x + 1, x^22 + x + 1,
* and x^23 + x^15 + x^2 + x + 1. */
#define R1TAPS	0x072000 /* bits 18,17,16,13 */
#define R2TAPS	0x300000 /* bits 21,20 */
#define R3TAPS	0x700080 /* bits 22,21,20,7 */

/* Output taps, for output generation */
#define R1OUT	0x040000 /* bit 18 (the high bit) */
#define R2OUT	0x200000 /* bit 21 (the high bit) */
#define R3OUT	0x400000 /* bit 22 (the high bit) */

u8 parity(u32 x) 
{
	x ^= x >> 16;
	x ^= x >> 8;
	x ^= x >> 4;
	x ^= x >> 2;
	x ^= x >> 1;
	return (x & 1);
}

/* Clock one shift register */
u32 clockone_register(u32 reg, u32 mask, u32 taps) {
	u32 t = reg & taps;
	reg = (reg << 1) & mask;
	reg |= (u32)parity(t);
	return reg;
}

u8 majority(u32 R1, u32 R2, u32 R3) 
{
	int sum;
	sum = parity(R1 & R1MID) + parity(R2 & R2MID) + parity(R3 & R3MID);
	if (sum >= 2)
		return 1;
	else
		return 0;
}

/* Clock two or three of R1,R2,R3, with clock control according to their middle bits.
* Specifically, we clock Ri whenever Ri's middle bit agrees with the majority value of the three middle bits.*/
//void clock_stopgo(u32 *R1, u32 *R2, u32 *R3)
//{
//	u8 maj = majority(*R1, *R2, *R3);
//	if (((*R1 & R1MID) != 0) == maj)
//		*R1 = clockone_register(*R1, R1MASK, R1TAPS);
//	if (((*R2 & R2MID) != 0) == maj)
//		*R2 = clockone_register(*R2, R2MASK, R2TAPS);
//	if (((*R3 & R3MID) != 0) == maj)
//		*R3 = clockone_register(*R3, R3MASK, R3TAPS);
//}

void clock_stopgo(u32 *R1, u32 *R2, u32 *R3)
{
	u8 maj = majority(*R1, *R2, *R3);
	if (((*R1 & R1MID) != 0) == maj)
		*R1 = clockone_register(*R1, R1MASK, R1TAPS);
	if (((*R2 & R2MID) != 0) == maj)
		*R2 = clockone_register(*R2, R2MASK, R2TAPS);
	if (((*R3 & R3MID) != 0) == maj)
		*R3 = clockone_register(*R3, R3MASK, R3TAPS);
}


/* Clock regularly all three of R1,R2,R3, ignoring their middle bits. which is only used for key setup. */
void clockallthree(u32 *R1, u32 *R2, u32 *R3) 
{
	*R1 = clockone_register(*R1, R1MASK, R1TAPS);
	*R2 = clockone_register(*R2, R2MASK, R2TAPS);
	*R3 = clockone_register(*R3, R3MASK, R3TAPS);
}

/* Generate an output bit from the current state */
u8 Keystream_getbit(u32 R1, u32 R2, u32 R3) 
{
	return (parity(R1 & R1OUT) ^ parity(R2 & R2OUT) ^ parity(R3 & R3OUT));
}

/* Do the A5/1 key setup.  This routine accepts a 64-bit key and a 22-bit frame number. */
void keysetup(u8 key[8], u32 frame, u32 *R1, u32 *R2, u32 *R3) 
{
	int i;
	u8 keybit=0, framebit=0;

	/* Zero out the shift registers. */
	*R1 = 0;
	*R2 = 0;
	*R3 = 0;

	for (i = 0; i<8; i++)printf("%02X", key[i]);
	printf("\n");
	printf("%0x", frame);
	printf("\n");

	/* Load the key into the shift registers, LSB of first byte of key array first, clocking each register once for every key bit loaded. (The usual clock
	control rule is temporarily disabled.) */
	for (i = 0; i<64; i++) {
		clockallthree(R1,R2,R3); /* always clock */
		keybit = (key[i / 8] >> (i & 7)) & 1; /* The i-th bit of the key */
		*R1 ^= (u32)keybit; 
		*R2 ^= (u32)keybit;
		*R3 ^= (u32)keybit;
	}
//	printf("R1=%0x,R2=%0x,R3=%0x,\n", *R1,*R2,*R3);
//	printf("\n");
	/* Load the frame number into the shift registers, LSB first, clocking each register once for every key bit loaded.  (The usual clock 
	control rule is still disabled.) */
	for (i = 0; i<22; i++) {
		clockallthree(R1, R2, R3); /* always clock */
		framebit = (frame >> i) & 1; /* The i-th bit of the frame #
									 */
		*R1 ^= framebit; 
		*R2 ^= framebit; 
		*R3 ^= framebit;
	}

	/* Run the shift registers for 100 clocks to mix the keying material and frame number together with output generation disabled, so that there is sufficient avalanche.
	We re-enable the majority-based clock control rule from now on. */
	for (i = 0; i<100; i++) {
		clock_stopgo(R1, R2, R3);
	}
	printf("R1=%0x,R2=%0x,R3=%0x,\n", *R1, *R2, *R3);
	/* Now the key is properly set up. */
}


void keysetup2(u8 key[8], u32 frame, u32 *R1, u32 *R2, u32 *R3, u32 *R1a, u32 *R2a, u32 *R3a)
{
	int i;
	u8 keybit = 0, framebit = 0;

	/* Zero out the shift registers. */
	*R1 = 0;
	*R2 = 0;
	*R3 = 0;

	for (i = 0; i<8; i++)printf("%02X", key[i]);
	printf("\n");
	printf("%0x", frame);
	printf("\n");

	/* Load the key into the shift registers, LSB of first byte of key array first, clocking each register once for every key bit loaded. (The usual clock
	control rule is temporarily disabled.) */
	for (i = 0; i<64; i++) {
		clockallthree(R1, R2, R3); /* always clock */
		keybit = (key[i / 8] >> (i & 7)) & 1; /* The i-th bit of the key */
		*R1 ^= (u32)keybit;
		*R2 ^= (u32)keybit;
		*R3 ^= (u32)keybit;
	}
	//	printf("R1=%0x,R2=%0x,R3=%0x,\n", *R1,*R2,*R3);
	//	printf("\n");
	/* Load the frame number into the shift registers, LSB first, clocking each register once for every key bit loaded.  (The usual clock
	control rule is still disabled.) */
	for (i = 0; i<22; i++) {
		clockallthree(R1, R2, R3); /* always clock */
		framebit = (frame >> i) & 1; /* The i-th bit of the frame #
									 */
		*R1 ^= framebit;
		*R2 ^= framebit;
		*R3 ^= framebit;
	}

	/* Run the shift registers for 100 clocks to mix the keying material and frame number together with output generation disabled, so that there is sufficient avalanche.
	We re-enable the majority-based clock control rule from now on. */
	for (i = 0; i<100; i++) {
		clock_stopgo(R1, R2, R3);
	}
	printf("R1=%0x,R2=%0x,R3=%0x,\n", *R1, *R2, *R3);
	/* Now the key is properly set up. */
	*R1a = *R1;
	*R2a = *R2;
	*R3a = *R3;
}


/* Generate output. We generate 228 bits of keystream output.  The first 114 bits is for the A->B frame; the next 114 bits is for the B->A frame.  
You allocate a 15-byte buffer for each direction, and this function fills it in. */
void GSM_onesession_run(u8 AtoBkeystream[15], u8 BtoAkeystream[15], u32 *R1, u32 *R2, u32 *R3)
{
	int i;

	/* Zero out the output buffers. */
	for (i = 0; i <= 113 / 8; i++)AtoBkeystream[i] = BtoAkeystream[i] = 0;

	/* Generate 114 bits of keystream for the
	* A->B direction.  Store it, MSB first. */
	for (i = 0; i<114; i++) {
		clock_stopgo(R1, R2, R3);
		AtoBkeystream[i / 8] |= Keystream_getbit(*R1, *R2, *R3) << (7 - (i & 7));
	}

	/* Generate 114 bits of keystream for the
	* B->A direction.  Store it, MSB first. */
	for (i = 0; i<114; i++) {
		clock_stopgo(R1, R2, R3);
		BtoAkeystream[i / 8] |= Keystream_getbit(*R1, *R2, *R3) << (7 - (i & 7));
	}
}

u8 Eqiv_f(u8 x[9])
{
	u8 f = 0;

	f = x[3] ^ x[4] ^ x[5] ^ (x[0] & x[6]) ^ (x[3] & x[6]) ^ (x[1] & x[7]) ^ (x[4] & x[7]) ^ (x[2] & x[8]) ^ (x[5] & x[8]) ^ (x[0] & x[6] & x[7])
		^ (x[1] & x[6] & x[7]) ^ (x[2] & x[6] & x[7]) ^ (x[3] & x[6] & x[7]) ^ (x[4] & x[6] & x[7]) ^ (x[5] & x[6] & x[7]) ^ (x[0] & x[6] & x[8])
		^ (x[1] & x[6] & x[8]) ^ (x[2] & x[6] & x[8]) ^ (x[3] & x[6] & x[8]) ^ (x[4] & x[6] & x[8]) ^ (x[5] & x[6] & x[8]) ^ (x[0] & x[7] & x[8])
		^ (x[1] & x[7] & x[8]) ^ (x[2] & x[7] & x[8]) ^ (x[3] & x[7] & x[8]) ^ (x[4] & x[7] & x[8]) ^ (x[5] & x[7] & x[8]);
	return(f);
}

void Extract_state_one(u32 R1, u32 R2, u32 R3, u8 x[9])
{
	x[0] = (u8)((R1 & R1OUT) >> 18) & 0x1;
	x[1] = (u8)((R2 & R2OUT) >> 21) & 0x1;
	x[2] = (u8)((R3 & R3OUT) >> 22) & 0x1;

	x[3] = (u8)((R1 & 0x020000) >> 17) & 0x1;
	x[4] = (u8)((R2 & 0x100000) >> 20) & 0x1;
	x[5] = (u8)((R3 & 0x200000) >> 21) & 0x1;

	x[6] = (u8)((R1 & R1MID) >> 8) & 0x1;
	x[7] = (u8)((R2 & R2MID) >> 10) & 0x1;
	x[8] = (u8)((R3 & R3MID) >> 10) & 0x1;
}


void Extract_state_two(u32 R1, u32 R2, u32 R3, u8 x[15])
{
	x[0] = (u8)((R1 & R1OUT) >> 18) & 0x1;
	x[1] = (u8)((R2 & R2OUT) >> 21) & 0x1;
	x[2] = (u8)((R3 & R3OUT) >> 22) & 0x1;

	x[3] = (u8)((R1 & 0x020000) >> 17) & 0x1;
	x[4] = (u8)((R2 & 0x100000) >> 20) & 0x1;
	x[5] = (u8)((R3 & 0x200000) >> 21) & 0x1;

	x[6] = (u8)((R1 & R1MID) >> 8) & 0x1;
	x[7] = (u8)((R2 & R2MID) >> 10) & 0x1;
	x[8] = (u8)((R3 & R3MID) >> 10) & 0x1;

	x[9] = (u8)((R1 & 0x010000) >> 16) & 0x1;
	x[10] = (u8)((R2 & 0x080000) >> 19) & 0x1;
	x[11] = (u8)((R3 & 0x100000) >> 20) & 0x1;

	x[12] = (u8)((R1 & 0x000080) >> 7) & 0x1;
	x[13] = (u8)((R2 & 0x000200) >> 9) & 0x1;
	x[14] = (u8)((R3 & 0x000200) >> 9) & 0x1;
}

void Extract_state_three(u32 R1, u32 R2, u32 R3, u8 x[21])
{
	x[0] = (u8)((R1 & R1OUT) >> 18) & 0x1;
	x[1] = (u8)((R2 & R2OUT) >> 21) & 0x1;
	x[2] = (u8)((R3 & R3OUT) >> 22) & 0x1;

	x[3] = (u8)((R1 & 0x020000) >> 17) & 0x1;
	x[4] = (u8)((R2 & 0x100000) >> 20) & 0x1;
	x[5] = (u8)((R3 & 0x200000) >> 21) & 0x1;

	x[6] = (u8)((R1 & R1MID) >> 8) & 0x1;
	x[7] = (u8)((R2 & R2MID) >> 10) & 0x1;
	x[8] = (u8)((R3 & R3MID) >> 10) & 0x1;

	x[9] = (u8)((R1 & 0x010000) >> 16) & 0x1;
	x[10] = (u8)((R2 & 0x080000) >> 19) & 0x1;
	x[11] = (u8)((R3 & 0x100000) >> 20) & 0x1;

	x[12] = (u8)((R1 & 0x000080) >> 7) & 0x1;
	x[13] = (u8)((R2 & 0x000200) >> 9) & 0x1;
	x[14] = (u8)((R3 & 0x000200) >> 9) & 0x1;

	x[15] = (u8)((R1 & 0x008000) >> 15) & 0x1;
	x[16] = (u8)((R2 & 0x040000) >> 18) & 0x1;
	x[17] = (u8)((R3 & 0x080000) >> 19) & 0x1;

	x[18] = (u8)((R1 & 0x000040) >> 6) & 0x1;
	x[19] = (u8)((R2 & 0x000100) >> 8) & 0x1;
	x[20] = (u8)((R3 & 0x000100) >> 8) & 0x1;
}

void Extract_state_four(u32 R1, u32 R2, u32 R3, u8 x[27])
{
	x[0] = (u8)((R1 & R1OUT) >> 18) & 0x1;
	x[1] = (u8)((R2 & R2OUT) >> 21) & 0x1;
	x[2] = (u8)((R3 & R3OUT) >> 22) & 0x1;

	x[3] = (u8)((R1 & 0x020000) >> 17) & 0x1;
	x[4] = (u8)((R2 & 0x100000) >> 20) & 0x1;
	x[5] = (u8)((R3 & 0x200000) >> 21) & 0x1;

	x[6] = (u8)((R1 & R1MID) >> 8) & 0x1;
	x[7] = (u8)((R2 & R2MID) >> 10) & 0x1;
	x[8] = (u8)((R3 & R3MID) >> 10) & 0x1;

	x[9] = (u8)((R1 & 0x010000) >> 16) & 0x1;
	x[10] = (u8)((R2 & 0x080000) >> 19) & 0x1;
	x[11] = (u8)((R3 & 0x100000) >> 20) & 0x1;

	x[12] = (u8)((R1 & 0x000080) >> 7) & 0x1;
	x[13] = (u8)((R2 & 0x000200) >> 9) & 0x1;
	x[14] = (u8)((R3 & 0x000200) >> 9) & 0x1;

	x[15] = (u8)((R1 & 0x008000) >> 15) & 0x1;
	x[16] = (u8)((R2 & 0x040000) >> 18) & 0x1;
	x[17] = (u8)((R3 & 0x080000) >> 19) & 0x1;

	x[18] = (u8)((R1 & 0x000040) >> 6) & 0x1;
	x[19] = (u8)((R2 & 0x000100) >> 8) & 0x1;
	x[20] = (u8)((R3 & 0x000100) >> 8) & 0x1;

	x[21] = (u8)((R1 & 0x004000) >> 14) & 0x1;
	x[22] = (u8)((R2 & 0x020000) >> 17) & 0x1;
	x[23] = (u8)((R3 & 0x040000) >> 18) & 0x1;

	x[24] = (u8)((R1 & 0x000020) >> 5) & 0x1;
	x[25] = (u8)((R2 & 0x00080) >> 7) & 0x1;
	x[26] = (u8)((R3 & 0x00080) >> 7) & 0x1;
}


void Extract_state_five(u32 R1, u32 R2, u32 R3, u8 x[33])
{
	x[0] = (u8)((R1 & R1OUT) >> 18) & 0x1;
	x[1] = (u8)((R2 & R2OUT) >> 21) & 0x1;
	x[2] = (u8)((R3 & R3OUT) >> 22) & 0x1;

	x[3] = (u8)((R1 & 0x020000) >> 17) & 0x1;
	x[4] = (u8)((R2 & 0x100000) >> 20) & 0x1;
	x[5] = (u8)((R3 & 0x200000) >> 21) & 0x1;

	x[6] = (u8)((R1 & R1MID) >> 8) & 0x1;
	x[7] = (u8)((R2 & R2MID) >> 10) & 0x1;
	x[8] = (u8)((R3 & R3MID) >> 10) & 0x1;

	x[9] = (u8)((R1 & 0x010000) >> 16) & 0x1;
	x[10] = (u8)((R2 & 0x080000) >> 19) & 0x1;
	x[11] = (u8)((R3 & 0x100000) >> 20) & 0x1;

	x[12] = (u8)((R1 & 0x000080) >> 7) & 0x1;
	x[13] = (u8)((R2 & 0x000200) >> 9) & 0x1;
	x[14] = (u8)((R3 & 0x000200) >> 9) & 0x1;

	x[15] = (u8)((R1 & 0x008000) >> 15) & 0x1;
	x[16] = (u8)((R2 & 0x040000) >> 18) & 0x1;
	x[17] = (u8)((R3 & 0x080000) >> 19) & 0x1;

	x[18] = (u8)((R1 & 0x000040) >> 6) & 0x1;
	x[19] = (u8)((R2 & 0x000100) >> 8) & 0x1;
	x[20] = (u8)((R3 & 0x000100) >> 8) & 0x1;

	x[21] = (u8)((R1 & 0x004000) >> 14) & 0x1;
	x[22] = (u8)((R2 & 0x020000) >> 17) & 0x1;
	x[23] = (u8)((R3 & 0x040000) >> 18) & 0x1;

	x[24] = (u8)((R1 & 0x000020) >> 5) & 0x1;
	x[25] = (u8)((R2 & 0x00080) >> 7) & 0x1;
	x[26] = (u8)((R3 & 0x00080) >> 7) & 0x1;

	x[27] = (u8)((R1 & 0x002000) >> 13) & 0x1;
	x[28] = (u8)((R2 & 0x010000) >> 16) & 0x1;
	x[29] = (u8)((R3 & 0x020000) >> 17) & 0x1;

	x[30] = (u8)((R1 & 0x000010) >> 4) & 0x1;
	x[31] = (u8)((R2 & 0x00040) >> 6) & 0x1;
	x[32] = (u8)((R3 & 0x00040) >> 6) & 0x1;
}

u8 majority_equv(u8 r1, u8 r2, u8 r3)
{
	int sum;
	sum = r1 + r2 + r3;
	if (sum >= 2)
		return 1;
	else
		return 0;
}

u8 clock_stopgo_equv1(u32 *R1, u32 *R2, u32 *R3)
{
	u8 x[9],fd=0,y[3];
	u8 maj = 0;

	memset(x, 0, 9);
	memset(y, 0, 3);

	Extract_state_one(*R1, *R2, *R3, x);
	
	y[0] = x[0];
	y[1] = x[1];
	y[2] = x[2];

	maj = majority_equv(x[6], x[7], x[8]);
	if (x[6] == maj)y[0] = x[3];
	if (x[7] == maj)y[1] = x[4];
	if (x[8] == maj)y[2] = x[5];

	fd = (y[0] ^ y[1] ^ y[2]);
		

	return(fd);

}

u8 clock_stopgo_equv1a(u32 *R1, u32 *R2, u32 *R3)
{
	u8 x[9], fd = 0, y[3],ct[3];
	u8 maj = 0;

	memset(x, 0, 9);
	memset(y, 0, 3);
	memset(ct, 0, 3);

	Extract_state_one(*R1, *R2, *R3, x);

	y[0] = x[0];
	y[1] = x[1];
	y[2] = x[2];

	ct[0] = x[6];
	ct[1] = x[7];
	ct[2] = x[8];

	maj = majority_equv(ct[0], ct[1], ct[2]);
	if (x[6] == maj){
		y[0] = x[3];
	}
	if (x[7] == maj){
		y[1] = x[4];
	}
	if (x[8] == maj){
		y[2] = x[5];
	}

	fd = (y[0] ^ y[1] ^ y[2]);

	return(fd);

}

void clock_stopgo_2bits(u8 x[15],u8 keyst[2])
{
	u8 y[3],ct[3],flag[3];
	u8 maj = 0;

	memset(y, 0, 3);
	memset(ct, 0, 3);
	memset(flag, 0, 3);

	y[0] = x[0];
	y[1] = x[1];
	y[2] = x[2];

	ct[0] = x[6];
	ct[1] = x[7];
	ct[2] = x[8];

	maj = majority_equv(ct[0], ct[1], ct[2]);
	if (x[6] == maj){
		y[0] = x[3];
		ct[0] = x[12];
		flag[0] += 1;
	}
	if (x[7] == maj){
		y[1] = x[4];
		ct[1] = x[13];
		flag[1] += 1;
	}
	if (x[8] == maj){
		y[2] = x[5];
		ct[2] = x[14];
		flag[2] += 1;
	}

	keyst[0] = (y[0] ^ y[1] ^ y[2]);

	maj = majority_equv(ct[0], ct[1], ct[2]);
	if (ct[0] == maj){
		if (flag[0] == 0)y[0] = x[3];
		else y[0] = x[9];
	}
	
	if (ct[1] == maj){
		if (flag[1] == 0)y[1] = x[4];
		else y[1] = x[10];
	}

	if (ct[2] == maj){
		if (flag[2] == 0)y[2] = x[5];
		else y[2] = x[11];
	}
	
	keyst[1] = (y[0] ^ y[1] ^ y[2]);

}

u8 clock_stopgo_equv2(u32 *R1, u32 *R2, u32 *R3)
{
	u8 x[9], fd = 0;
	
	memset(x, 0, 9);
	
	Extract_state_one(*R1, *R2, *R3, x);

	fd = Eqiv_f(x);

	return(fd);

}

int Compare_clock(u32 *R1, u32 *R2, u32 *R3)
{
	u64 i;
	u8 fd1=0, fd2 = 0;
	int counter = 0;

	for (counter = 0,i = 0; i < 100000; i++){
		*R1 = ((rc4() << 24) + (rc4() << 16) + (rc4() << 8) + (rc4())) & R1MASK;
		*R2 = ((rc4() << 24) + (rc4() << 16) + (rc4() << 8) + (rc4())) & R2MASK;
		*R3 = ((rc4() << 24) + (rc4() << 16) + (rc4() << 8) + (rc4())) & R3MASK;
//		fd1 = clock_stopgo_equv1(R1, R2, R3);
		fd1 = clock_stopgo_equv1a(R1, R2, R3);
		fd2 = clock_stopgo_equv2(R1, R2, R3);
		if (fd1 == fd2)counter++;
	}

	return(counter);
}

int Compare_clock2bits(u32 *R1, u32 *R2, u32 *R3)
{
	u64 i;
	u8 fd1[2], fd2[2],x[15];
	int counter = 0;

	memset(fd1,0,2);
	memset(fd2, 0, 2);
	memset(x, 0, 15);

	for (counter = 0, i = 0; i < 10000000; i++){
		*R1 = ((rc4() << 24) + (rc4() << 16) + (rc4() << 8) + (rc4())) & R1MASK;
		*R2 = ((rc4() << 24) + (rc4() << 16) + (rc4() << 8) + (rc4())) & R2MASK;
		*R3 = ((rc4() << 24) + (rc4() << 16) + (rc4() << 8) + (rc4())) & R3MASK;
		
		Extract_state_two(*R1, *R2, *R3, x);
		clock_stopgo_2bits(x, fd1);

		clock_stopgo(R1, R2, R3);
		fd2[0] = Keystream_getbit(*R1, *R2, *R3);
		clock_stopgo(R1, R2, R3);
		fd2[1] = Keystream_getbit(*R1, *R2, *R3);

		if (memcmp(fd1, fd2, 2) == 0)counter++;
	}

	return(counter);
}

//void GSM_onesession_run_equv(u8 AtoBkeystream[15], u8 BtoAkeystream[15], u32 *R1, u32 *R2, u32 *R3)
//{
//	int i;
//
//	/* Zero out the output buffers. */
//	for (i = 0; i <= 113 / 8; i++)AtoBkeystream[i] = BtoAkeystream[i] = 0;
//
//	/* Generate 114 bits of keystream for the
//	* A->B direction.  Store it, MSB first. */
//	for (i = 0; i<114; i++) {
////		clock_stopgo(R1, R2, R3);
//		AtoBkeystream[i / 8] |= clock_stopgo_equv(R1, R2, R3) << (7 - (i & 7));
//	}
//
//	/* Generate 114 bits of keystream for the
//	* B->A direction.  Store it, MSB first. */
//	for (i = 0; i<114; i++) {
////		clock_stopgo(R1, R2, R3);
//		BtoAkeystream[i / 8] |= clock_stopgo_equv(R1, R2, R3) << (7 - (i & 7));
//	}
//}









