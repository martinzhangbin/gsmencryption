#include "cipher.h"

struct isd2                                 // to construct the link of ksd2isd with the proportion of the isd
{
	u8 position[2];
	double fc;
	struct isd2 *next;
};

struct datalist2
{
	u8 prefix[2];
	u8 ISnca[15];
	double ISnca_sum;
	u32 position;
	struct datalist2 *next;
};

struct datalist3
{
	u8 prefix[3];
	u8 ISnca[21];
	double ISnca_sum;
	u32 position;
	struct datalist3 *next;
};

struct datalist4
{
	u8 prefix[4];
	u8 ISnca[27];
	double ISnca_sum;
	u32 position;
	struct datalist4 *next;
};

struct datalist5
{
	u8 prefix[5];
	u8 ISnca[33];
	double ISnca_sum;
	u32 position;
	struct datalist5 *next;
};

struct initialdata2
{
	u32 R1;
	u32 R2;
	u32 R3;
	u8 keystream[100];
};

u32 length_isd2(struct isd2 *ph)                             // find the length of the link isd2_table[i]
{
	struct isd2 *p;
	u32 n;

	n = 0;
	p = ph;

	if (ph != NULL)
	{
		do
		{
			n++;
			p = p->next;
		} while (p != NULL);
	}

	return(n - 1);                                   // return the index of the last element  
}

u32 Computing_difnca_arbitray_2(u32 ksd[4], u8 ns[15], u8 i1, u8 i2, u8 keystr[2])
{
	u8 z[2];
	u32 k, j, counter;
	double pattern;
	
	memset(z, 0, 2);
	pattern = 0;
	counter = 0;

	for (k = 0; k < 32768; k++){
		for (j = 0; j < 15; j++)ns[j] = (k >> j) & 0x1;
		clock_stopgo_2bits(ns, z);
		if (memcmp(z, keystr, 2) == 0){
			counter++;

			ns[i1] ^= 0x1;
			ns[i2] ^= 0x1;
			
			clock_stopgo_2bits(ns, z);

			for (j = 0; j < 2; j++)z[j] ^= keystr[j];
			for (pattern = 0.0, j = 0; j < 2; j++)pattern += z[j] * pow(2, j);
			ksd[(u32)pattern] += 1;
		}
	}
	return(counter);
}

u32 Computing_difnca_arbitray_1(u32 ksd[4], u8 ns[15], u8 i1, u8 keystr[2])
{
	u8 z[2];
	u32 k, j, counter;
	double pattern;

	memset(z, 0, 2);
	pattern = 0;
	counter = 0;

	for (k = 0; k < 32768; k++){
		for (j = 0; j < 15; j++)ns[j] = (k >> j) & 0x1;
		clock_stopgo_2bits(ns, z);
		if (memcmp(z, keystr, 2) == 0){
			counter++;

			ns[i1] ^= 0x1;
			
			clock_stopgo_2bits(ns, z);

			for (j = 0; j < 2; j++)z[j] ^= keystr[j];
			for (pattern = 0.0, j = 0; j < 2; j++)pattern += z[j] * pow(2, j);
			ksd[(u32)pattern] += 1;
		}
	}
	return(counter);
}


u32 Computing_difnca_arbitray_0(u32 ksd[4], u8 ns[15], u8 keystr[2])
{
	u8 z[2];
	u32 k, j, counter;
	double pattern;

	memset(z, 0, 2);
	pattern = 0;
	counter = 0;

	for (k = 0; k < 32768; k++){
		for (j = 0; j < 15; j++)ns[j] = (k >> j) & 0x1;
		clock_stopgo_2bits(ns, z);
		if (memcmp(z, keystr, 2) == 0){
			counter++;

			clock_stopgo_2bits(ns, z);

			for (j = 0; j < 2; j++)z[j] ^= keystr[j];
			for (pattern = 0.0, j = 0; j < 2; j++)pattern += z[j] * pow(2, j);
			ksd[(u32)pattern] += 1;
		}
	}
	return(counter);
}

struct isd2 *Ksd2isd2_nca_arbitrary(u8 ns[15], struct isd2 *ph, u8 index, u8 z[2])       //index is the KSD and z[2] is one of the 2-bit keystream prefix
{
	u8 i1, i2, i, flag = 0;
	u32 ksd[4], r, counter = 0;
	struct isd2 *p1, *p2, *head, *p3;

	for (i = 0; i < 4; i++)ksd[i] = 0;

	p1 = (struct isd2 *)calloc(1, sizeof(struct isd2));
	if (NULL == p1)
	{
		printf("Error in calloc in Ksd2isd2_nca_arbitrary.\n");
		return 0;
	}
	p2 = NULL;
	p3 = NULL;

	r = 0;
	head = NULL;

	for (i1 = 0; i1 < 14; i1++)                                     // index starts from 0 to include the less weight isd
	for (i2 = i1 + 1; i2 < 15; i2++)
	{
		flag = 0;
		for (i = 0; i < 4; i++)ksd[i] = 0;
		counter = Computing_difnca_arbitray_2(ksd, ns, i1, i2, z);

		if ((ksd[index] != 0) && (r == 0) && (flag == 0))
		{
			r++;

			head = p1;
			p1->position[0] = i1;
			p1->position[1] = i2;


			p1->fc = (double)(ksd[index]) / counter;
//			printf("%u,%u,%u: %f\n", p1->position[0], p1->position[1], p1->position[2],p1->fc);
			p1->next = NULL;
			p2 = p1;
			flag = 1;

		}
		else if ((ksd[index] != 0) && (r > 0) && (flag == 0))
		{
			p3 = (struct isd2 *)calloc(1, sizeof(struct isd2));
			if (NULL == p3)
			{
				printf("Error in calloc in Ksd2isd2_nca_arbitrary.\n");
				return 0;
			}

			p3->position[0] = i1;
			p3->position[1] = i2;

			p3->fc = (double)(ksd[index]) / counter;
//			printf("%u,%u,%u: %f\n", p3->position[0], p3->position[1], p3->position[2], p3->fc);
			p3->next = NULL;
			p2->next = p3;
			p2 = p3;
			flag = 1;
			r++;

		}
		else continue;
	}



	for (i1 = 0; i1 < 15; i1++){

		flag = 0;
		for (i = 0; i < 4; i++)ksd[i] = 0;
		counter = Computing_difnca_arbitray_1(ksd, ns, i1, z);

		if ((ksd[index] != 0) && (flag == 0)){
			p3 = (struct isd2 *)calloc(1, sizeof(struct isd2));
			if (NULL == p3)
			{
				printf("Error in calloc in Ksd2isd2_nca_arbitrary.\n");
				return 0;
			}

			p3->position[0] = i1;
			p3->position[1] = 20;


			p3->fc = (double)(ksd[index]) / counter;
//			printf("%u,%u,%u: %f\n", p3->position[0], p3->position[1], p3->position[2], p3->fc);
			p3->next = NULL;
			p2->next = p3;
			p2 = p3;
			flag = 1;
			r++;
		}
		else continue;
	}

	flag = 0;
	for (i = 0; i < 4; i++)ksd[i] = 0;
	counter = Computing_difnca_arbitray_0(ksd, ns, z);

	if ((ksd[index] != 0) && (flag == 0)){
		p3 = (struct isd2 *)calloc(1, sizeof(struct isd2));
		if (NULL == p3)
		{
			printf("Error in calloc in Ksd2isd2_nca_arbitrary.\n");
			return 0;
		}

		p3->position[0] = 20;
		p3->position[1] = 20;

		p3->fc = (double)(ksd[index]) / counter;
//		printf("%u,%u,%u: %f\n", p3->position[0], p3->position[1], p3->position[2], p3->fc);
		p3->next = NULL;
		p2->next = p3;
		p2 = p3;
		flag = 1;
		r++;
	}


	ph = head;
	return(head);
}

void Swap_isd2(struct isd2 *ph, u32 left, u32 right)          // swap the two structures in the link, indexed by left and right position pointers
{
	struct isd2 *p1, *head, *p2, *p3;
	u32 i;

	p1 = NULL;
	p2 = NULL;
	p3 = NULL;
	head = NULL;

	if (left == 0)p1 = ph;
	else{
		for (p1 = ph, i = 0; i < left; i++)p1 = p1->next;
	}

	for (p2 = ph, i = 0; i < right; i++)p2 = p2->next;

	p3 = (struct isd2 *)calloc(1, sizeof(struct isd2));
	if (NULL == p3)
	{
		printf("Error in calloc in Swap_isd2.\n");
		return 0;
	}

	p3->position[0] = p1->position[0];
	p3->position[1] = p1->position[1];
//	p3->position[2] = p1->position[2];
//	p3->position[3] = p1->position[3];
//	p3->position[4] = p1->position[4];
	p3->fc = p1->fc;

	p1->position[0] = p2->position[0];
	p1->position[1] = p2->position[1];
//	p1->position[2] = p2->position[2];
//	p1->position[3] = p2->position[3];
//	p1->position[4] = p2->position[4];
	p1->fc = p2->fc;

	p2->position[0] = p3->position[0];
	p2->position[1] = p3->position[1];
//	p2->position[2] = p3->position[2];
//	p2->position[3] = p3->position[3];
//	p2->position[4] = p3->position[4];
	p2->fc = p3->fc;

	free(p3);                         // new added instruction
}


u32 partition(struct isd2 *ph, u32 left, u32 right)
{
	u32 i, pivotIndex, length, storeIndex, r;
	double pivotValue;
	struct isd2 *p1, *p2, *p3;

	p1 = NULL;
	p2 = NULL;
	p3 = NULL;
	length = length_isd2(ph);


	pivotIndex = (u32)(floor((left + right) / 2));                 // choose the pivot element
	p1 = ph;
	for (i = 0; i < pivotIndex; i++)p1 = p1->next;
	pivotValue = p1->fc;

	Swap_isd2(ph, pivotIndex, right);
	storeIndex = left;
	p2 = ph;

	for (i = left; i <= right - 1; i++){
		if (i == 0)p2 = ph;
		else for (p2 = ph, r = 0; r < i; r++)p2 = p2->next;

		//		if ((p2->fc) < pivotValue){
		if ((p2->fc) > pivotValue){
			Swap_isd2(ph, i, storeIndex);
			storeIndex++;
		}
	}
	Swap_isd2(ph, storeIndex, right);

	return(storeIndex);

}

void Sort_isd2(struct isd2 *ph, u32 left, u32 right)          // sort the link of isd2_table[i] according to the field of fc
{
	u32 p;


	if (left < right){
		p = partition(ph, left, right);
		Sort_isd2(ph, left, p);
		Sort_isd2(ph, p + 1, right);
	}

}

void Output_isd2(struct isd2 *ph)                                          // output the link of isd2_table[i]
{
	struct isd2 *p;

	p = ph;

	if (ph != NULL)
	{
		do
		{
			printf("%u,%u: %.7f;\n", p->position[0], p->position[1], p->fc);
			p = p->next;
		} while (p != NULL);
	}
}

void Precomputation(u8 *ns, struct isd2 *isd2_table[4][4])
{
	u8 i, j, k;
	u8 keystr[2];

	memset(keystr, 0, 2);

	for (i = 0; i < 4; i++)
	for (k = 0; k < 4; k++)
	{
		for (j = 0; j < 2; j++)keystr[j] = (k >> j) & 0x1;
		isd2_table[i][k] = Ksd2isd2_nca_arbitrary(ns, isd2_table[i][k], i, keystr);
		Sort_isd2(isd2_table[i][k], 0, length_isd2(isd2_table[i][k]));
//		printf("The KSD is %u and one of the prefix is %u\n", i, k);
//		printf("The length is %u\n", length_isd2(isd2_table[i][k]));
//		Output_isd2(isd2_table[i][k]);
//		printf("***************************************\n");
//		TotalISD_BSW_arbitrary(LFSR, NFSR, isd2_table, i, keystr);
	}
	printf("Finished pre-computation\n");

	/*for (i = 0; i < 4; i++)
	for (k = 0; k < 4; k++){
	printf("The KSD is %u and one of the prefix is %u: ", i, k);
	printf("The length is %u\n", length_isd2(isd2_table[i][k]));
	printf("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");
	}
	printf("-------------------------------------------------------------------\n");*/
}


void Isd2Pr_diversity(struct isd2 *isd2_table[4][4], double Pr[4][4])
{
	u8 i, j;
	u32 r;
	double sum;
	struct isd2 *pp1, *pp2;

	pp1 = NULL;
	pp2 = NULL;
	for (i = 0; i < 4; i++)
	for (j = 0; j < 4; j++)
	{
		sum = 0.0;
		pp1 = isd2_table[i][j];
		pp2 = pp1;
		r = 0;

		do{
			sum = sum + pp2->fc;
			r++;
			pp2 = pp2->next;
		} while (pp2 != NULL);

		Pr[i][j] = (double)(sum) / (r);
	}
}

void output_Pr(double Pr[4][4])
{
	u8 i, j, r;

	r = 0;
	for (i = 0; i < 4; i++)
	for (j = 0; j < 4; j++){
		printf("i=%u,j=%u:", i, j);
		printf("%f ", Pr[i][j]);
		r++;
		if ((r % 4) == 0)printf("\n");
	}

}


struct initialdata2 *Targetkeystream_generation(u32 *R1, u32 *R2, u32 *R3, struct initialdata2 *list)        // generate the target keystream and its corresponding internal state
{
	u8 z[100];
	u32 i;
	
	memset(z, 0, 100);
	
	*R1 = ((rc4() << 24) + (rc4() << 16) + (rc4() << 8) + (rc4())) & R1MASK;
	*R2 = ((rc4() << 24) + (rc4() << 16) + (rc4() << 8) + (rc4())) & R2MASK;
	*R3 = ((rc4() << 24) + (rc4() << 16) + (rc4() << 8) + (rc4())) & R3MASK;

	/**R1 = (0x1da4b) & R1MASK;
	*R2 = (0x2cb2f9) & R2MASK;
	*R3 = (0x8be5f) & R3MASK;*/

	list->R1 = *R1;
	list->R2 = *R2;
	list->R3 = *R3;

	for (i = 0; i<100; i++) {
		clock_stopgo(R1, R2, R3);
		z[i] = Keystream_getbit(*R1, *R2, *R3);
	}

	for (i = 0; i < 100; i++)list->keystream[i] = z[i];

	return(list);
}

struct initialdata2 *Targetkeystream_generation2(u32 *R1, u32 *R2, u32 *R3, struct initialdata2 *list)        // generate the target keystream and its corresponding internal state
{
	u8 z[100];
	u32 i;

	memset(z, 0, 100);

	/**R1 = ((rc4() << 24) + (rc4() << 16) + (rc4() << 8) + (rc4())) & R1MASK;
	*R2 = ((rc4() << 24) + (rc4() << 16) + (rc4() << 8) + (rc4())) & R2MASK;
	*R3 = ((rc4() << 24) + (rc4() << 16) + (rc4() << 8) + (rc4())) & R3MASK;*/

	/**R1 = (0x1da4b) & R1MASK;
	*R2 = (0x2cb2f9) & R2MASK;
	*R3 = (0x8be5f) & R3MASK;*/

	printf("R1a=%0x,R2a=%0x,R3a=%0x,\n", *R1, *R2, *R3);

	list->R1 = *R1;
	list->R2 = *R2;
	list->R3 = *R3;

	for (i = 0; i<100; i++) {
		clock_stopgo(R1, R2, R3);
		z[i] = Keystream_getbit(*R1, *R2, *R3);
	}

	for (i = 0; i < 100; i++)list->keystream[i] = z[i];

	return(list);
}


void output_initialdata2(struct initialdata2 *list)
{
	u32 i;

	printf("%x,", list->R1);
	printf("%x,", list->R2);
	printf("%x,", list->R3);
	printf("\n");
	
	printf("The generated 100 keystream is: ");
	for (i = 0; i < 100; i++)printf("%x", list->keystream[i]);
	printf("\n");
}

void Prepare_ncadata(struct initialdata2 *initiallist, struct datalist2 *list, u8 n)           // transform into the data structure used in our programm
{
	u32 i, j, R1, R2, R3;
	u8 ns[15];
	u8 z[2], z1[100];
	double sum;

	memset(z, 0, 2);
	memset(z1, 0, 100);
	memset(ns, 0, 15);
	R1 = R2 = R3 = 0;
	
	for (i = 0; i < 2; i++)list->prefix[i] = initiallist->keystream[n + i];
	list->position = n;

	for (i = 0; i < 2; i++)z[i] = initiallist->keystream[n + i];
	
	R1 = initiallist->R1;
	R2 = initiallist->R2;
	R3 = initiallist->R3;

	if (n != 0){
		for (i = 0; i<n; i++) {
			clock_stopgo(&R1, &R2, &R3);
			z1[i] = Keystream_getbit(R1, R2, R3);
		}                                                                                      // run the beginning state up to the correct position
		Extract_state_two(R1, R2, R3, ns);
	}
	else{
		Extract_state_two(R1, R2, R3, ns);
	}
	for (j = 0; j < 15; j++)list->ISnca[j] = ns[j];

	for (sum = 0.0, j = 0; j < 15; j++)sum += (list->ISnca[j]) * pow(2, j);
	list->ISnca_sum = sum;
	list->next = NULL;
}


void Prepare_ncadata3(struct initialdata2 *initiallist, struct datalist3 *list, u8 n)           // transform into the data structure used in our programm
{
	u32 i, j, R1, R2, R3;
	u8 ns[21];
	u8 z[3], z1[100];
	double sum;

	memset(z, 0, 3);
	memset(z1, 0, 100);
	memset(ns, 0, 21);
	R1 = R2 = R3 = 0;

	for (i = 0; i < 3; i++)list->prefix[i] = initiallist->keystream[n + i];
	list->position = n;

	for (i = 0; i < 3; i++)z[i] = initiallist->keystream[n + i];

	R1 = initiallist->R1;
	R2 = initiallist->R2;
	R3 = initiallist->R3;

	if (n != 0){
		for (i = 0; i<n; i++) {
			clock_stopgo(&R1, &R2, &R3);
			z1[i] = Keystream_getbit(R1, R2, R3);
		}                                                                                      // run the beginning state up to the correct position
		Extract_state_three(R1, R2, R3, ns);
	}
	else{
		Extract_state_three(R1, R2, R3, ns);
	}
	for (j = 0; j < 21; j++)list->ISnca[j] = ns[j];

	for (sum = 0.0, j = 0; j < 21; j++)sum += (list->ISnca[j]) * pow(2, j);
	list->ISnca_sum = sum;
	list->next = NULL;
}


void Prepare_ncadata4(struct initialdata2 *initiallist, struct datalist4 *list, u8 n)           // transform into the data structure used in our programm
{
	u32 i, j, R1, R2, R3;
	u8 ns[27];
	u8 z[4], z1[100];
	double sum;

	memset(z, 0, 4);
	memset(z1, 0, 100);
	memset(ns, 0, 27);
	R1 = R2 = R3 = 0;

	for (i = 0; i < 4; i++)list->prefix[i] = initiallist->keystream[n + i];
	list->position = n;

	for (i = 0; i < 4; i++)z[i] = initiallist->keystream[n + i];

	R1 = initiallist->R1;
	R2 = initiallist->R2;
	R3 = initiallist->R3;

	if (n != 0){
		for (i = 0; i<n; i++) {
			clock_stopgo(&R1, &R2, &R3);
			z1[i] = Keystream_getbit(R1, R2, R3);
		}                                                                                      // run the beginning state up to the correct position
		Extract_state_four(R1, R2, R3, ns);
	}
	else{
		Extract_state_four(R1, R2, R3, ns);
	}
	for (j = 0; j < 27; j++)list->ISnca[j] = ns[j];

	for (sum = 0.0, j = 0; j < 27; j++)sum += (list->ISnca[j]) * pow(2, j);
	list->ISnca_sum = sum;
	list->next = NULL;
}

void Prepare_ncadata5(struct initialdata2 *initiallist, struct datalist5 *list, u8 n)           // transform into the data structure used in our programm
{
	u32 i, j, R1, R2, R3;
	u8 ns[33];
	u8 z[5], z1[100];
	double sum;

	memset(z, 0, 5);
	memset(z1, 0, 100);
	memset(ns, 0, 33);
	R1 = R2 = R3 = 0;

	for (i = 0; i < 5; i++)list->prefix[i] = initiallist->keystream[n + i];
	list->position = n;

	for (i = 0; i < 5; i++)z[i] = initiallist->keystream[n + i];

	R1 = initiallist->R1;
	R2 = initiallist->R2;
	R3 = initiallist->R3;

	if (n != 0){
		for (i = 0; i<n; i++) {
			clock_stopgo(&R1, &R2, &R3);
			z1[i] = Keystream_getbit(R1, R2, R3);
		}                                                                                      // run the beginning state up to the correct position
		Extract_state_five(R1, R2, R3, ns);
	}
	else{
		Extract_state_five(R1, R2, R3, ns);
	}
	for (j = 0; j < 33; j++)list->ISnca[j] = ns[j];

	for (sum = 0.0, j = 0; j < 33; j++)sum += (list->ISnca[j]) * pow(2, j);
	list->ISnca_sum = sum;
	list->next = NULL;
}

void Attacktarget_preparation2(struct initialdata2 *initiallist, u32 *R1, u32 *R2, u32 *R3, struct datalist2 *list, u8 begin_point)
{
	initiallist = Targetkeystream_generation(R1, R2, R3, initiallist);
	output_initialdata2(initiallist);

	Prepare_ncadata(initiallist, list, begin_point);
}




void Self_contained2_ncacounting(u16 *try1)
{
	u32 i, num;

	for (num = 0, i = 0; i < 32768; i++)
	if (try1[i] != 0)
	{
		num++;
	}
	//	printf("num=%u\n", num);
}

u32 Self_contained2_ncacounting3(u16 *try1)
{
	u32 i, num;

	for (num = 0, i = 0; i < 32768; i++)
	if (try1[i] != 0)
	{
		num++;
	}
	//	printf("num=%u\n", num);
	return(num);
}

u8 check_candidate1(u16 *try, double in)
{
	if (try[(u32)in] != 0)return(1);
	else return(0);
}

u8 verify_ncakeystr(struct datalist2 *list, u8 inter[15])
{
	u8 z[2];

	memset(z, 0, 2);
	clock_stopgo_2bits(inter, z);

	return(memcmp(list->prefix, z, 2));
}

void flip_ncainter(u8 inter[15], struct isd2 *pp2)
{
	u8 i1 = 0, i2 = 0;

	i1 = pp2->position[0];
	i2 = pp2->position[1];

	if(i1 < 16)inter[i1] ^= 0x1;
	if(i2 < 16)inter[i2] ^= 0x1;
	
}

void Extractnca_arbitrary(u8 ns[15], u8 z[2])
{
	u8 i,ct[3], maj=0;
	u8 keystr[2];
	u8 flag[3];
	
//	memset(ns, 0, 15);
	memset(keystr, 0, 2);
	memset(ct, 0, 3);
	memset(flag, 0, 3);
	
	clock_stopgo_2bits(ns, keystr);

	ct[0] = ns[6];
	ct[1] = ns[7];
	ct[2] = ns[8];

	if ((keystr[0] == z[0]) && (keystr[1] != z[1])){
		
		maj = majority_equv(ct[0], ct[1], ct[2]);
		if (ns[6] == maj){
			ct[0] = ns[12];
			flag[0] += 1;
		}
		if (ns[7] == maj){
			ct[1] = ns[13];
			flag[1] += 1;
		}
		if (ns[8] == maj){
			ct[2] = ns[14];
			flag[2] += 1;
		}

		maj = majority_equv(ct[0], ct[1], ct[2]);
		if (ct[0] == maj){
			if (flag[0] == 0)ns[3] ^= 0x1;
			else ns[9] ^= 0x1;
		}

	}
	else if ((keystr[0] != z[0]) && (keystr[1] == z[1])){
		
		maj = majority_equv(ct[0], ct[1], ct[2]);

		if (ns[6] == maj){
			ns[3] ^= 0x1;
			ct[0] = ns[12];
			flag[0] += 1;
		}
		else ns[0] ^= 0x1;

		if (ns[7] == maj){
			ct[1] = ns[13];
			flag[1] += 1;
		}
		if (ns[8] == maj){
			ct[2] = ns[14];
			flag[2] += 1;
		}
	}
	else if ((keystr[0] != z[0]) && (keystr[1] != z[1])){
		
		maj = majority_equv(ct[0], ct[1], ct[2]);
		if (ns[6] == maj){
			ns[3] ^= 0x1;
			ct[0] = ns[12];
			flag[0] += 1;
		}
		else ns[0] ^= 0x1;

		if (ns[7] == maj){
			ct[1] = ns[13];
			flag[1] += 1;
		}
		if (ns[8] == maj){
			ct[2] = ns[14];
			flag[2] += 1;
		}

		maj = majority_equv(ct[0], ct[1], ct[2]);
		if (ct[0] == maj){
			if (flag[0] == 0)ns[3] ^= 0x1;
			else ns[9] ^= 0x1;
		}
	}
	
}

void Extractnca_arbitrary3(u8 ns[15], u8 z[2])
{
	u8 keystr[2];

	memset(keystr, 0, 2);

	do{
		randomIV(ns, 15);
		clock_stopgo_2bits(ns, keystr);
	} while (memcmp(keystr, z, 2) != 0);

}

//#define NCAC 271                               // 2^15/Sum[Binomial[15,i],{i,0,2}]
#define NCAC 331 

u16 *ISrecovery_nca_Selfcontained(u32 *R1, u32 *R2, u32 *R3, struct datalist2 *list, struct isd2 *isd2_table[4][4], u16 *try)     // generate the internal state corresponding to the 2-bit keystream segment
{
	u8 z[2], j, k, z1[2], index = 0, inter[15], flag = 0, zero[2];
	u32 i, i1, r = 0, success = 0, c = 0;
	u8 ns[15];
	struct datalist2 *p1;
	struct isd2 *pp1, *pp2;
	double sum, counter1;

	memset(z, 0, 2);
	memset(z1, 0, 2);
	memset(zero, 0, 2);
	memset(ns, 0, 15);
	memset(inter, 0, 15);
	pp1 = NULL;
	pp2 = NULL;

	p1 = NULL;
	p1 = (struct datalist2 *)calloc(1, sizeof(struct datalist2));
	if (p1 == NULL){
		printf("error in calloc in ISrecovery_nca_Selfcontained\n");
	}

//	for (j = 0; j < 2; j++)zero[j] = 0x1 ^ list->prefix[j];
	for (j = 0; j < 2; j++)zero[j] = 0x1 ^ list->prefix[j];

	i1 = 0;
	counter1 = 0;
	r = 0;
//	c = 5;
//	c = 3;
	c = 4;

	while (success < c*(NCAC)){

		randomIV(ns, 15);                                            // 1st, the virtual part of the self-contained method: p1
		Extractnca_arbitrary3(ns, zero);

		for (j = 0; j < 15; j++)p1->ISnca[j] = ns[j];
//		for (j = 0; j < 2; j++)p1->prefix[j] = z[j];
		for (j = 0; j < 2; j++)p1->prefix[j] = zero[j];
		for (sum = 0.0, j = 0; j < 15; j++)sum += (p1->ISnca[j]) * pow(2, j);
		p1->ISnca_sum = sum;
		p1->position = 0;
			
		for (j = 0; j < 15; j++)inter[j] = p1->ISnca[j];
			
		for (i = 0; i < 1; i++){
			for (j = 0; j < 2; j++)z1[j] = (list + i)->prefix[j];
			for (index = 0, j = 0; j < 2; j++)index += (u8)((z1[j]) * pow(2, j));

//			pp1 = isd2_table[index][3];                             // isd2_table is used to store the precomputation information
			pp1 = isd2_table[3][index];
//			pp1 = isd2_table[0][index];
			pp2 = pp1;
			do
			{
				for (j = 0; j < 15; j++)inter[j] = p1->ISnca[j];
				flip_ncainter(inter, pp2);
				flag = verify_ncakeystr(list + i, inter);

				if (flag == 0){
					r++;
					for (sum = 0.0, j = 0; j < 15; j++)sum += inter[j] * pow(2, j);
					try[(u32)sum] ++;

					if (r == 1){
//						if ( memcmp((list + i)->ISnca + 2, inter + 2 , 10) == 0 ) {
						if (memcmp((list + i)->ISnca, inter, 15) == 0) {

							   /* printf("Fd!\n");
								printf("The flipped positions are: ");
								printf("%u,%u\n", pp2->position[0], pp2->position[1]);*/
								/*for (j = 0; j < 2; j++)printf("%x", (list + i)->prefix[j]);
								printf("\n");*/
								/*for (j = 0; j < 15; j++)printf("%x", inter[j]);
								printf("\n");*/
								/*for (j = 0; j < 15; j++)printf("%x", (list + i)->ISnca[j]);
								printf("\n");
								printf("The sum is %f\n", (list + i)->ISnca_sum);*/

								/*printf("The new position is %u and the original position is %u\n", r - 1, i);
								printf("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");*/
						}
							//						else break;						

					}
					else
					{
							//						if ( memcmp((list + i)->ISnca + 2, inter + 2, 10) == 0 ){
						if (memcmp((list + i)->ISnca, inter, 15) == 0) {

							   /*printf("Fd!\n");
								printf("The flipped positions are: ");
								printf("%u,%u\n", pp2->position[0], pp2->position[1]);*/
								/*for (j = 0; j < 2; j++)printf("%x", (list + i)->prefix[j]);
								printf("\n");*/
								/*for (j = 0; j < 15; j++)printf("%x", inter[j]);
								printf("\n");*/
								/*for (j = 0; j < 15; j++)printf("%x", (list + i)->ISnca[j]);
								printf("\n");
								printf("The sum is %f\n", (list + i)->ISnca_sum);*/

								/*printf("The new position is %u and the original position is %u\n", r - 1, i);
								printf("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");*/
						}
							//						else break;
					}
				}
				pp2 = pp2->next;
			} while (pp2 != NULL);
		}
				
		success++;
	}   // corresponding to while(successs<)

	return(try);
}

u16 *FirstSet_preparation(u16 *try1, struct datalist2 *list2, struct isd2 *isd2_table[4][4], u32 *R1, u32 *R2, u32 *R3)
{
	u8 in1;
	double counter;

	in1 = 0;
	counter = 0.0;
	do{
		try1 = (u16 *)calloc(32768, sizeof(u16));
		if (try1 == NULL){
			printf("error in calloc in try1 in FirstSet_preparation\n");
		}

		try1 = ISrecovery_nca_Selfcontained(R1, R2, R3, list2, isd2_table, try1);
		Self_contained2_ncacounting(try1);

		in1 = check_candidate1(try1, list2->ISnca_sum);
		if (in1 != 0){
			printf("Corret1\n");
			printf("++++++++++++++++++++++++++++\n");
			counter++;
		}
		else free(try1);
	} while (in1 == 0);

	return(try1);
}

u16 *FirstSet_preparation2(u16 *try1, struct datalist2 *list2, struct isd2 *isd2_table[4][4], u32 *R1, u32 *R2, u32 *R3)
{
	u8 in1;
	double counter;
	int i;

	in1 = 0;
	counter = 0.0;
	do{
		for (i = 0; i < 32768; i++)try1[i] = 0;

		try1 = ISrecovery_nca_Selfcontained(R1, R2, R3, list2, isd2_table, try1);
		Self_contained2_ncacounting(try1);

		in1 = check_candidate1(try1, list2->ISnca_sum);
		if (in1 != 0){
			printf("Corret1\n");
			printf("++++++++++++++++++++++++++++\n");
			counter++;
		}
		
	} while (in1 == 0);

	return(try1);
}

void Test_constant2(struct datalist2 *list2, struct isd2 *isd2_table[4][4], u32 *R1, u32 *R2, u32 *R3)
{
	u8 in1;
	u16 *try1;
	double counter;
	u32 j, k;

	in1 = 0;
	k = 0;
	try1 = NULL;
	counter = 0.0;

	try1 = (u16 *)calloc(32768, sizeof(u16));
	if (try1 == NULL){
		printf("error in calloc in try1 in Test_constant2\n");
	}
	do{
		for (j = 0; j < 32768; j++)try1[j] = 0;
		try1 = ISrecovery_nca_Selfcontained(R1, R2, R3, list2, isd2_table, try1);
//		Self_contained2_ncacounting(try1);
		in1 = check_candidate1(try1, list2->ISnca_sum);
		if (in1 != 0){
//			printf("Corret1\n");
			counter = counter + 1.0;
		}

		k++;
	} while (k < 10000);

	printf("The ratio that the correct key will be in the self_contained list is %f\n", counter / 10000);

	free(try1);
}

u32 Self_contained2_ncacounting2(u16 *try1)
{
	u32 i, num;

	for (num = 0, i = 0; i < 32768; i++)
	if (try1[i] != 0)
	{
		num++;
	}
	return(num);
	//	printf("num=%u\n", num);
}

void Test_average2(struct datalist2 *list2, struct isd2 *isd2_table[4][4], u32 *R1, u32 *R2, u32 *R3)
{
	u8 in1;
	u16 *try1;
	double counter;
	u32 j, k, xx = 0;

	in1 = 0;
	k = 0;
	try1 = NULL;
	counter = 0.0;

	try1 = (u16 *)calloc(32768, sizeof(u16));
	if (try1 == NULL){
		printf("error in calloc in try1 in Test_constant2\n");
	}
	do{
		for (j = 0; j < 32768; j++)try1[j] = 0;
		try1 = ISrecovery_nca_Selfcontained(R1, R2, R3, list2, isd2_table, try1);
		xx += Self_contained2_ncacounting2(try1);
		/*in1 = check_candidate1(try1, list2->ISnca_sum);
		if (in1 != 0){
		printf("Corret1\n");
		counter++;
		}*/

		k++;
	} while (k < 10000);

	printf("The average list size is %f\n", (double)xx / 10000);

	free(try1);
}

void Test_const_average2(struct datalist2 *list2, struct isd2 *isd2_table[4][4], u32 *R1, u32 *R2, u32 *R3)
{
	u8 in1;
	u16 *try1;
	double counter;
	u32 j, k;
	double xx = 0.0;

	in1 = 0;
	k = 0;
	try1 = NULL;
	counter = 0.0;

	try1 = (u16 *)calloc(32768, sizeof(u16));
	if (try1 == NULL){
		printf("error in calloc in try1 in Test_constant2\n");
	}
	do{
		for (j = 0; j < 32768; j++)try1[j] = 0;
		try1 = ISrecovery_nca_Selfcontained(R1, R2, R3, list2, isd2_table, try1);
		in1 = check_candidate1(try1, list2->ISnca_sum);
		if (in1 != 0){
//			printf("Corret1\n");
			counter = counter + 1.0;
		}
		xx += (double)Self_contained2_ncacounting2(try1);
		/*in1 = check_candidate1(try1, list2->ISnca_sum);
		if (in1 != 0){
		printf("Corret1\n");
		counter++;
		}*/

		k++;
	} while (k < 100000);

	printf("The ratio that the correct key will be in the self_contained list is %f\n", counter / 100000);
	printf("The average list size is %f\n", (double)xx / 100000);

	free(try1);
}


void Self_contained2_intersection_nca(u16 *try1, u16 *try2)      // generate the intersection set of the candidates 
{
	u32 i, num;

	for (i = 0; i < 32768; i++)
	if ((try1[i] != 0) && (try2[i] != 0))               // note that here the logical operation is &&, not ||
	{
		try1[i] = 1;
	}
	else
	{
		try1[i] = 0;
	}


	/*for (num = 0, i = 0; i < 32768; i++)
	if (try1[i] != 0)num++;*/

}

void Merge_nca(u16 *try, u16 *result)
{
	u32 i;

	for (i = 0; i < 32768; i++)
	{
		if (try[i] != 0){

			result[i] = 1;

		}
		else continue;
	}
}

u32 Counting_merge_nca(u16 *result)
{
	u32 i, num;

	for (num = 0, i = 0; i < 32768; i++)
	if (result[i] != 0)
	{
		num++;
	}
	//	printf("There are %u elements after merging nca\n", num);

	return(num);
}

void Merge_normal(struct initialdata2 *initiallist, struct datalist2 *list2, struct isd2 *isd2_table[4][4], u32 *R1, u32 *R2, u32 *R3)
{
	u16 *try1, *try2, *result;
	u8 in1, in11;
	u32 i, j, k, tt, xx = 0;
	double c1;
	

	try1 = NULL;
	try2 = NULL;
	result = NULL;
	
	result = (u16 *)calloc(32768, sizeof(u16));
	if (result == NULL){
			printf("error in calloc in result in Merge_normal\n");
	}
	

	try1 = (u16 *)calloc(32768, sizeof(u16));
	if (try1 == NULL){
		printf("error in calloc in try1 in Merge_normal\n");
	}
	try2 = (u16 *)calloc(32768, sizeof(u16));
	if (try2 == NULL){
		printf("error in calloc in try2 in Merge_normal\n");
	}

	c1 = 0.0;
	for (tt = 0; tt < 50000; tt++){

		for (j = 0; j < 32768; j++)result[j] = 0;
		in11 = 0;
		for (i = 0; i < 2; i++){


			for (j = 0; j < 32768; j++)try1[j] = 0;

			try1 = ISrecovery_nca_Selfcontained(R1, R2, R3, list2, isd2_table, try1);
//			Self_contained2_ncacounting(try1);


			for (j = 0; j < 32768; j++)try2[j] = 0;


			for (k = 0; k < 4; k++){
				for (j = 0; j < 32768; j++)try2[j] = 0;
				try2 = ISrecovery_nca_Selfcontained(R1, R2, R3, list2, isd2_table, try2);
//				printf("k=%u: ", k);
				Self_contained2_intersection_nca(try1, try2);
			}


//			Self_contained2_ncacounting(try1);
			in1 = 0;
			in1 = check_candidate1(try1, list2->ISnca_sum);
			if (in1 != 0){
//				printf("Success!\n");
				in11 = 1;
			}
//			printf("----------------------------------------------------------\n");
			
			Merge_nca(try1, result);
			
		}

		if (in11 != 0)c1 = c1 + 1.0;
//		printf("tt=%u: ", tt);
		xx += Counting_merge_nca(result);


	}

	printf("The ratio is %f\n", c1 / 50000.0);
	printf("The average is %f\n", (double)xx / 50000.0);
	
	free(result);
	free(try2);
	free(try1);
}

void Merge_normal_new(struct datalist2 *list2, struct isd2 *isd2_table[4][4], u32 *R1, u32 *R2, u32 *R3,u8 begin_point)
{
	u16 *try1, *try2;
	u8 in1, in11;
	u32 i, j, k;
	double c1;
	

	try1 = NULL;
	try2 = NULL;
	

	try1 = (u16 *)calloc(32768, sizeof(u16));
	if (try1 == NULL){
		printf("error in calloc in try1 in Merge_normal\n");
	}
	try2 = (u16 *)calloc(32768, sizeof(u16));
	if (try2 == NULL){
		printf("error in calloc in try2 in Merge_normal\n");
	}
	printf("\n^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");
	for (j = 0; j < 15; j++)printf("%x", list2->ISnca[j]);
	printf("\n^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");

	c1 = 0.0;
	in11 = 0;
	for (i = 0; i < 2; i++){


		for (j = 0; j < 32768; j++)try1[j] = 0;

		try1 = ISrecovery_nca_Selfcontained(R1, R2, R3, list2, isd2_table, try1);
//		Self_contained2_ncacounting(try1);


		for (j = 0; j < 32768; j++)try2[j] = 0;

		for (k = 0; k < 6; k++){
			for (j = 0; j < 32768; j++)try2[j] = 0;
			try2 = ISrecovery_nca_Selfcontained(R1, R2, R3, list2, isd2_table, try2);
//			printf("k=%u: ", k);
			Self_contained2_intersection_nca(try1, try2);
		}


//		Self_contained2_ncacounting(try1);
		in1 = 0;
		in1 = check_candidate1(try1, list2->ISnca_sum);
		if (in1 != 0){
				printf("%u, Success!\n",begin_point);
				in11 = 1;
		}
//		printf("----------------------------------------------------------\n");

	}

	free(try2);
	free(try1);
}

u16 *Merge_normal_new2(struct datalist2 *list2, struct isd2 *isd2_table[4][4], u32 *R1, u32 *R2, u32 *R3, u8 begin_point, u16 *result)
{
	u16 *try1, *try2;
	u8 in1, in11;
	u32 i, j, k;
	double c1;


	try1 = NULL;
	try2 = NULL;


	try1 = (u16 *)calloc(32768, sizeof(u16));
	if (try1 == NULL){
		printf("error in calloc in try1 in Merge_normal\n");
	}
	try2 = (u16 *)calloc(32768, sizeof(u16));
	if (try2 == NULL){
		printf("error in calloc in try2 in Merge_normal\n");
	}
//	printf("\n^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");
	for (j = 0; j < 15; j++)printf("%x", list2->ISnca[j]);
	printf("\n");
//	printf("\n^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");

	c1 = 0.0;
	in11 = 0;
	for (i = 0; i < 2; i++){


		for (j = 0; j < 32768; j++)try1[j] = 0;

		try1 = ISrecovery_nca_Selfcontained(R1, R2, R3, list2, isd2_table, try1);
		//		Self_contained2_ncacounting(try1);


		for (j = 0; j < 32768; j++)try2[j] = 0;

		for (k = 0; k < 6; k++){
			for (j = 0; j < 32768; j++)try2[j] = 0;
			try2 = ISrecovery_nca_Selfcontained(R1, R2, R3, list2, isd2_table, try2);
			//			printf("k=%u: ", k);
			Self_contained2_intersection_nca(try1, try2);
		}


		//		Self_contained2_ncacounting(try1);
		in1 = 0;
		in1 = check_candidate1(try1, list2->ISnca_sum);
		if (in1 != 0){
//			printf("%u, Success!\n", begin_point);
			in11 = 1;
		}
		//		printf("----------------------------------------------------------\n");

		Merge_nca(try1, result);

	}

	free(try2);
	free(try1);
	return(result);
}

void Counting_result(u16 *result[5])
{
	int i;
	u32 sc[5];

	for (i = 0; i < 5; i++)sc[i] = 0;

	for (i = 0; i < 5;i++)sc[i] = Self_contained2_ncacounting2(result[i]);

	for (i = 0; i < 5; i++)printf("%u,",sc[i]);
	printf("\n");
}

void Checking_result(u16 *result[5], struct datalist2 *list2[5])
{
	int i;
	u8 sc[5];

	for (i = 0; i < 5; i++)sc[i] = 0;

	for (i = 0; i < 5; i++)sc[i] = check_candidate1(result[i], list2[i]->ISnca_sum);

	for (i = 0; i < 5; i++)printf("%u,", sc[i]);
	printf("\n");
}

void Merge_restricited_new(struct datalist2 *list2[6], u8 begin_point, u8 end_point)  // merge list2->[begin_point] and list2->[end_point]
{
	struct datalist2 *pp1, *pp2;
	u8 in1, in11;
	u32 i, j, k;
	double sum;

	pp1 = NULL;
	pp2 = NULL;

	printf("\n&&^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^&&\n");
	for (j = 0; j < 15; j++)printf("%u", list2[begin_point]->ISnca[j]);
	printf("\n");
	for (j = 0; j < 15; j++)printf("%u", list2[end_point]->ISnca[j]);
	printf("\n&&^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^&&\n");
	
	
	pp1 = list2[begin_point];
	pp2 = list2[end_point];


	if (pp1 != NULL)
	{
		do
		{
			for (j = 0; j < 3; j++)printf("%u", pp1->ISnca[6+j]);
			printf("\n");
			for (sum = 0.0, j = 0; j < 3; j++)sum += (pp1->ISnca[6 + j]) * pow(2, j);
			pp1->position = (u32)sum;
//			printf("sum=%f\n",sum);
			printf("sum=%d\n", pp1->position);
			pp1 = pp1->next;
		} while (pp1 != NULL);
	}
	printf("---------------------------\n");
	if (pp2 != NULL)
	{
		do
		{
			for (j = 0; j < 3; j++)printf("%u", pp2->ISnca[6 + j]);
			printf("\n");
			for (sum = 0.0, j = 0; j < 3; j++)sum += (pp2->ISnca[6 + j]) * pow(2, j);
			pp2->position = (u32)sum;
//			printf("sum=%f\n", sum);
			printf("sum=%d\n", pp2->position);
			pp2 = pp2->next;
		} while (pp2 != NULL);
	}

}

