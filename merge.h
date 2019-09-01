#include "attack.h"

struct merge_state1                                 // to construct the link of mergestate 
{
	u8 clock_control;
	u8 overlapping[11];
	double overlap_value;
	u8 ncastate[15];
	double nca_value;
	struct merge_state1 *next;
};

struct merge_state1a                                 // to construct the link of mergestate 
{
	u8 hit_value;
	double ahead_value;                                 // corresponding to the restricted state of z_0z_1
	double behind_value;                                // corresponding to the restricted state of z_1z_2
	struct merge_state1a *next;
};

struct merge_state2a                                 // to construct the link of mergestate 
{
	u8 hit_value;
	double ahead_value;                                 // corresponding to the restricted state of z_0z_1z_2
	double behind_value;                                // corresponding to the restricted state of z_1z_2z_3
	struct merge_state2a *next;
};

struct merge_state2                                 // to construct the second merging link of mergestate 
{
	u8 overlapping[15];
	double overlap_value;
	struct merge_state2 *next;
};

void Merge_restricited_new2(u16 *result[5], u8 begin_point, u8 end_point, struct merge_state1a *merge_re)  // merge result[begin_point] and result[end_point]
{
	int i, j, k;
	double sum, aa[3];
	u16 ct[8];

	for (i = 0; i < 8; i++)ct[i] = 0;
	for (i = 0; i < 3; i++)aa[i] = 0;

	for (j = 0; j < 5; j++){

		for (k = 0; k < 8; k++)ct[k] = 0;

		for (i = 0; i < 32768; i++){
			if ((*(result[j] + i)) != 0){

				aa[0] = (i >> 6) & 0x1;
				aa[1] = (i >> 7) & 0x1;
				aa[2] = (i >> 8) & 0x1;
				for (sum = 0.0, k = 0; k < 3; k++)sum += aa[k] * pow(2, k);
				ct[(u16)sum]++;

			}
		}
		/*for (k = 0; k < 8; k++)printf("%u,", ct[k]);
		printf("\n");*/
	}

	for (i = 0; i < 8; i++)printf("%u,",ct[i]);
	printf("\n");

}

void Output_mergestate1(struct merge_state1 *ph)                                          // output the link of merge_state1[i]
{
	struct merge_state1 *p;
	int i;

	p = ph;

	while(p != NULL)
	{
		printf("%d,\n", p->clock_control);
		for (i = 0; i < 11; i++)printf("%x", p->overlapping[i]);
		printf("\n");
		printf("%f,\n", p->overlap_value);
		for (i = 0; i < 15; i++)printf("%x", p->ncastate[i]);
		printf("\n---------------\n");
		p = p->next;
	};
}

u32 length_mergestate1(struct merge_state1 *ph)                             // find the length of the link merge_state1[i]
{
	struct merge_state1 *p;
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

	//	return(n - 1);                           // return the index of the last element  
	return(n);                                   // return the length of the link  
}

void Swap_mergestate1(struct merge_state1 *ph, u32 left, u32 right)          // swap the two structures in the link, indexed by left and right position pointers
{
	struct merge_state1 *p1, *head, *p2, *p3;
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

	p3 = (struct merge_state1 *)calloc(1, sizeof(struct merge_state1));
	if (NULL == p3)
	{
		printf("Error in calloc in Swap_mergestate1.\n");
	}

	p3->clock_control = p1->clock_control;
	for (i = 0; i < 11;i++)p3->overlapping[i] = p1->overlapping[i];
	p3->overlap_value = p1->overlap_value;
	for (i = 0; i < 15; i++)p3->ncastate[i] = p1->ncastate[i];

	p1->clock_control = p2->clock_control;
	for (i = 0; i < 11; i++)p1->overlapping[i] = p2->overlapping[i];
	p1->overlap_value = p2->overlap_value;
	for (i = 0; i < 15; i++)p1->ncastate[i] = p2->ncastate[i];

	p2->clock_control = p3->clock_control;
	for (i = 0; i < 11; i++)p2->overlapping[i] = p3->overlapping[i];
	p2->overlap_value = p3->overlap_value;
	for (i = 0; i < 15; i++)p2->ncastate[i] = p3->ncastate[i];

	free(p3);                         // new added instruction
}

u32 partition_merge(struct merge_state1 *ph, u32 left, u32 right)
{
	u32 i, pivotIndex, length, storeIndex, r;
	double pivotValue;
	struct merge_state1 *p1, *p2, *p3;

	p1 = NULL;
	p2 = NULL;
	p3 = NULL;
	length = length_mergestate1(ph)-1;


	pivotIndex = (u32)(floor((left + right) / 2));                 // choose the pivot element
	p1 = ph;
	for (i = 0; i < pivotIndex; i++)p1 = p1->next;
	pivotValue = p1->overlap_value;

	Swap_mergestate1(ph, pivotIndex, right);
	storeIndex = left;
	p2 = ph;

	for (i = left; i <= right - 1; i++){
		if (i == 0)p2 = ph;
		else for (p2 = ph, r = 0; r < i; r++)p2 = p2->next;

		//		if ((p2->fc) < pivotValue){
		if ((p2->overlap_value) > pivotValue){
			Swap_mergestate1(ph, i, storeIndex);
			storeIndex++;
		}
	}
	Swap_mergestate1(ph, storeIndex, right);

	return(storeIndex);

}

void Sort_mergestate1(struct merge_state1 *ph, u32 left, u32 right)          // sort the link of isd2_table[i] according to the field of fc
{
	u32 p;


	if (left < right){
		p = partition_merge(ph, left, right);
		Sort_mergestate1(ph, left, p);
		Sort_mergestate1(ph, p + 1, right);
	}

}

void Output_mergestate2(struct merge_state2 *ph)                                          // output the link of merge_state1[i]
{
	struct merge_state2 *p;
	int i;

	p = ph;

	while (p != NULL)
	{
		printf("%f,\n", p->overlap_value);
		for (i = 0; i < 15; i++)printf("%x", p->overlapping[i]);
		printf("\n");
		printf("---------------\n");
		p = p->next;
	};
}

u32 length_mergestate2(struct merge_state2 *ph)                             // find the length of the link merge_state1[i]
{
	struct merge_state2 *p;
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

	//	return(n - 1);                           // return the index of the last element  
	return(n);                                   // return the length of the link  
}




void Merge_restricited_new3(u16 *result[5], u8 begin_point, u8 end_point, struct merge_state1a *merge_re)  // merge result[begin_point] and result[end_point]
{
	int i, j, k;
	double sum, sum1;
	struct merge_state1 *p[8], *head[8], *p2[8], *p3[8];
	struct merge_state1 *q[8], *headq[8], *q2[8], *q3[8];
	struct merge_state1 *ph,*ph1;
	u32 r[8];
	u8 merge_array[21], aa[3];


	for (i = 0; i < 3; i++)aa[i] = 0;
	for (i = 0; i < 8; i++)r[i] = 0;
	for (i = 0; i < 8; i++)p[i] = NULL;
	for (i = 0; i < 8; i++)p2[i] = NULL;
	for (i = 0; i < 8; i++)p3[i] = NULL;
	for (i = 0; i < 8; i++)head[i] = NULL;
	for (i = 0; i < 21; i++)merge_array[i] = 0;
	ph = NULL;
	ph1 = NULL;

	for (i = 0; i < 8; i++){
		p[i] = (struct merge_state1 *)calloc(1, sizeof(struct merge_state1));      // 8192/8=1024
		if (p[i] == NULL){
			printf("error in calloc in p[i] in Merge_restricited_new3\n");
		}
	}

// prepare the begin_point sublists
	for (i = 0; i < 32768; i++){
//	for (i = 26370; i < 26371; i++){

		if ((*(result[begin_point] + i)) != 0){

			aa[0] = (i >> 6) & 0x1;
			aa[1] = (i >> 7) & 0x1;
			aa[2] = (i >> 8) & 0x1;
			for (sum = 0.0, k = 0; k < 3; k++)sum += aa[k] * pow(2, k);
//			printf("sum=%f\n",sum);

			if (r[(u8)sum] == 0){

				r[(u8)sum]++;
				head[(u8)sum] = p[(u8)sum];
				p[(u8)sum]->clock_control = (u8)sum;
				for (k = 0; k < 15; k++)p[(u8)sum]->ncastate[k] = (i >> k) & 0x1;
				p[(u8)sum]->nca_value = i;


				if ((sum == 0) || (sum == 7)){
					for (k = 3; k < 6; k++)p[(u8)sum]->overlapping[k - 3] = (i >> k) & 0x1;
					for (k = 9; k < 15; k++)p[(u8)sum]->overlapping[k - 6] = (i >> k) & 0x1;
					p[(u8)sum]->overlapping[9] = 20;
					p[(u8)sum]->overlapping[10] = 20;

					for (sum1 = 0.0, k = 0; k < 9; k++)sum1 += p[(u8)sum]->overlapping[k] * pow(2, k);
					p[(u8)sum]->overlap_value = sum1;
				}
				else if (sum == 1){
					p[(u8)sum]->overlapping[0] = i & 0x1;
					for (k = 3; k < 7; k++)p[(u8)sum]->overlapping[k - 2] = (i >> k) & 0x1;
					for (k = 9; k < 15; k++)p[(u8)sum]->overlapping[k - 4] = (i >> k) & 0x1;

					for (sum1 = 0.0, k = 0; k < 11; k++)sum1 += p[(u8)sum]->overlapping[k] * pow(2, k);
					p[(u8)sum]->overlap_value = sum1;

				}
				else if (sum == 2){
					p[(u8)sum]->overlapping[0] = (i >> 1) & 0x1;
					for (k = 3; k < 6; k++)p[(u8)sum]->overlapping[k - 2] = (i >> k) & 0x1;
					p[(u8)sum]->overlapping[4] = (i >> 7) & 0x1;
					for (k = 9; k < 15; k++)p[(u8)sum]->overlapping[k - 4] = (i >> k) & 0x1;

					for (sum1 = 0.0, k = 0; k < 11; k++)sum1 += p[(u8)sum]->overlapping[k] * pow(2, k);
					p[(u8)sum]->overlap_value = sum1;
				}
				else if (sum == 3){
					p[(u8)sum]->overlapping[0] = (i >> 2) & 0x1;
					for (k = 3; k < 6; k++)p[(u8)sum]->overlapping[k - 2] = (i >> k) & 0x1;
					p[(u8)sum]->overlapping[4] = (i >> 8) & 0x1;
					for (k = 9; k < 15; k++)p[(u8)sum]->overlapping[k - 4] = (i >> k) & 0x1;

					for (sum1 = 0.0, k = 0; k < 11; k++)sum1 += p[(u8)sum]->overlapping[k] * pow(2, k);
					p[(u8)sum]->overlap_value = sum1;
				}
				else if (sum == 4){
					p[(u8)sum]->overlapping[0] = (i >> 2) & 0x1;
					for (k = 3; k < 6; k++)p[(u8)sum]->overlapping[k - 2] = (i >> k) & 0x1;
					p[(u8)sum]->overlapping[4] = (i >> 8) & 0x1;
					for (k = 9; k < 15; k++)p[(u8)sum]->overlapping[k - 4] = (i >> k) & 0x1;

					for (sum1 = 0.0, k = 0; k < 11; k++)sum1 += p[(u8)sum]->overlapping[k] * pow(2, k);
					p[(u8)sum]->overlap_value = sum1;
				}
				else if (sum == 5){
					p[(u8)sum]->overlapping[0] = (i >> 1) & 0x1;
					for (k = 3; k < 6; k++)p[(u8)sum]->overlapping[k - 2] = (i >> k) & 0x1;
					p[(u8)sum]->overlapping[4] = (i >> 7) & 0x1;
					for (k = 9; k < 15; k++)p[(u8)sum]->overlapping[k - 4] = (i >> k) & 0x1;

					for (sum1 = 0.0, k = 0; k < 11; k++)sum1 += p[(u8)sum]->overlapping[k] * pow(2, k);
					p[(u8)sum]->overlap_value = sum1;
				}
				else{
					p[(u8)sum]->overlapping[0] = i & 0x1;
					for (k = 3; k < 7; k++)p[(u8)sum]->overlapping[k - 2] = (i >> k) & 0x1;
					for (k = 9; k < 15; k++)p[(u8)sum]->overlapping[k - 4] = (i >> k) & 0x1;

					for (sum1 = 0.0, k = 0; k < 11; k++)sum1 += p[(u8)sum]->overlapping[k] * pow(2, k);
					p[(u8)sum]->overlap_value = sum1;

				}
				p[(u8)sum]->next = NULL;
				p2[(u8)sum] = p[(u8)sum];

			}// corresponding to if (r[(u8)sum] == 0)
			else {
				r[(u8)sum]++;

				p3[(u8)sum] = (struct merge_state1 *)calloc(1, sizeof(struct merge_state1));
				if (NULL == p3[(u8)sum])
				{
					printf("Error in calloc in Merge_restricited_new3.\n");

				}

				p3[(u8)sum]->clock_control = (u8)sum;
				for (k = 0; k < 15; k++)p3[(u8)sum]->ncastate[k] = (i >> k) & 0x1;
				p3[(u8)sum]->nca_value = i;


				if ((sum == 0) || (sum == 7)){
					for (k = 3; k < 6; k++)p3[(u8)sum]->overlapping[k - 3] = (i >> k) & 0x1;
					for (k = 9; k < 15; k++)p3[(u8)sum]->overlapping[k - 6] = (i >> k) & 0x1;
					p3[(u8)sum]->overlapping[9] = 20;
					p3[(u8)sum]->overlapping[10] = 20;

					for (sum1 = 0.0, k = 0; k < 9; k++)sum1 += p3[(u8)sum]->overlapping[k] * pow(2, k);
					p3[(u8)sum]->overlap_value = sum1;
				}
				else if (sum == 1){
					p3[(u8)sum]->overlapping[0] = i & 0x1;
					for (k = 3; k < 7; k++)p3[(u8)sum]->overlapping[k - 2] = (i >> k) & 0x1;
					for (k = 9; k < 15; k++)p3[(u8)sum]->overlapping[k - 4] = (i >> k) & 0x1;

					for (sum1 = 0.0, k = 0; k < 11; k++)sum1 += p3[(u8)sum]->overlapping[k] * pow(2, k);
					p3[(u8)sum]->overlap_value = sum1;

				}
				else if (sum == 2){
					p3[(u8)sum]->overlapping[0] = (i >> 1) & 0x1;
					for (k = 3; k < 6; k++)p3[(u8)sum]->overlapping[k - 2] = (i >> k) & 0x1;
					p3[(u8)sum]->overlapping[4] = (i >> 7) & 0x1;
					for (k = 9; k < 15; k++)p3[(u8)sum]->overlapping[k - 4] = (i >> k) & 0x1;

					for (sum1 = 0.0, k = 0; k < 11; k++)sum1 += p3[(u8)sum]->overlapping[k] * pow(2, k);
					p3[(u8)sum]->overlap_value = sum1;
				}
				else if (sum == 3){
					p3[(u8)sum]->overlapping[0] = (i >> 2) & 0x1;
					for (k = 3; k < 6; k++)p3[(u8)sum]->overlapping[k - 2] = (i >> k) & 0x1;
					p3[(u8)sum]->overlapping[4] = (i >> 8) & 0x1;
					for (k = 9; k < 15; k++)p3[(u8)sum]->overlapping[k - 4] = (i >> k) & 0x1;

					for (sum1 = 0.0, k = 0; k < 11; k++)sum1 += p3[(u8)sum]->overlapping[k] * pow(2, k);
					p3[(u8)sum]->overlap_value = sum1;
				}
				else if (sum == 4){
					p3[(u8)sum]->overlapping[0] = (i >> 2) & 0x1;
					for (k = 3; k < 6; k++)p3[(u8)sum]->overlapping[k - 2] = (i >> k) & 0x1;
					p3[(u8)sum]->overlapping[4] = (i >> 8) & 0x1;
					for (k = 9; k < 15; k++)p3[(u8)sum]->overlapping[k - 4] = (i >> k) & 0x1;

					for (sum1 = 0.0, k = 0; k < 11; k++)sum1 += p3[(u8)sum]->overlapping[k] * pow(2, k);
					p3[(u8)sum]->overlap_value = sum1;
				}
				else if (sum == 5){
					p3[(u8)sum]->overlapping[0] = (i >> 1) & 0x1;
					for (k = 3; k < 6; k++)p3[(u8)sum]->overlapping[k - 2] = (i >> k) & 0x1;
					p3[(u8)sum]->overlapping[4] = (i >> 7) & 0x1;
					for (k = 9; k < 15; k++)p3[(u8)sum]->overlapping[k - 4] = (i >> k) & 0x1;

					for (sum1 = 0.0, k = 0; k < 11; k++)sum1 += p3[(u8)sum]->overlapping[k] * pow(2, k);
					p3[(u8)sum]->overlap_value = sum1;
				}
				else{
					p3[(u8)sum]->overlapping[0] = i & 0x1;
					for (k = 3; k < 7; k++)p3[(u8)sum]->overlapping[k - 2] = (i >> k) & 0x1;
					for (k = 9; k < 15; k++)p3[(u8)sum]->overlapping[k - 4] = (i >> k) & 0x1;

					for (sum1 = 0.0, k = 0; k < 11; k++)sum1 += p3[(u8)sum]->overlapping[k] * pow(2, k);
					p3[(u8)sum]->overlap_value = sum1;

				}
				p3[(u8)sum]->next = NULL;
				p2[(u8)sum]->next = p3[(u8)sum];
				p2[(u8)sum] = p3[(u8)sum];

			}

		}
	}

	
	for (i = 0; i < 8;i++)Sort_mergestate1(p[i], 0, length_mergestate1(p[i]) - 1);
//	Sort_mergestate1(p[4], 0, length_mergestate1(p[4]) - 1);
//	Output_mergestate1(p[4]);
	/*for (i = 0; i < 8; i++){
	printf("%u,", length_mergestate1(p[i]));
	}
	printf("\n");*/

// prepare the end_point
	printf("Now the end_point\n");

	for (i = 0; i < 3; i++)aa[i] = 0;
	for (i = 0; i < 8; i++)r[i] = 0;
	for (i = 0; i < 8; i++)q[i] = NULL;
	for (i = 0; i < 8; i++)q2[i] = NULL;
	for (i = 0; i < 8; i++)q3[i] = NULL;
	for (i = 0; i < 8; i++)headq[i] = NULL;

	for (i = 0; i < 8; i++){
		q[i] = (struct merge_state1 *)calloc(1, sizeof(struct merge_state1));      // 8192/8=1024
		if (q[i] == NULL){
			printf("error in calloc in q[i] in Merge_restricited_new3\n");
		}
	}

	for (j = 0; j < 8; j++){
//	for (j = 4; j < 5; j++){
		sum = (double)j;
//		printf("j=%d\n",j);

	    for (i = 0; i < 32768; i++){
//		for (i = 22424; i < 22425; i++){
		
		   if ((*(result[end_point] + i)) != 0){

			 if (r[(u8)sum] == 0){

					r[(u8)sum]++;
					headq[(u8)sum] = q[(u8)sum];
					q[(u8)sum]->clock_control = (u8)sum;
//					printf("ct=%u,index=%u\n", q[(u8)sum]->clock_control,(u8)sum);
					for (k = 0; k < 15; k++)q[(u8)sum]->ncastate[k] = (i >> k) & 0x1;
					q[(u8)sum]->nca_value = i;


					if (((u8)sum == 0) || ((u8)sum == 7)){
//						printf("sum=%f\n",sum);
						for (k = 0; k < 9; k++)q[(u8)sum]->overlapping[k] = (i >> k) & 0x1;

						q[(u8)sum]->overlapping[9] = 20;
						q[(u8)sum]->overlapping[10] = 20;

						for (sum1 = 0.0, k = 0; k < 9; k++)sum1 += q[(u8)sum]->overlapping[k] * pow(2, k);
						q[(u8)sum]->overlap_value = sum1;
					}
					else if ((u8)sum == 1){
//						printf("sum=%f\n", sum);
						q[(u8)sum]->overlapping[0] = i & 0x1;
						q[(u8)sum]->overlapping[1] = ( i >> 3 ) & 0x1;
						q[(u8)sum]->overlapping[2] = (i >> 1) & 0x1;
						q[(u8)sum]->overlapping[3] = (i >> 2) & 0x1;
						q[(u8)sum]->overlapping[4] = (i >> 6) & 0x1;
						q[(u8)sum]->overlapping[5] = (i >> 9) & 0x1;
						q[(u8)sum]->overlapping[6] = (i >> 4) & 0x1;
						q[(u8)sum]->overlapping[7] = (i >> 5) & 0x1;
						q[(u8)sum]->overlapping[8] = (i >> 12) & 0x1;
						q[(u8)sum]->overlapping[9] = (i >> 7) & 0x1;
						q[(u8)sum]->overlapping[10] = (i >> 8) & 0x1;
						
						for (sum1 = 0.0, k = 0; k < 11; k++)sum1 += q[(u8)sum]->overlapping[k] * pow(2, k);
						q[(u8)sum]->overlap_value = sum1;

					}
					else if ((u8)sum == 2){
//						printf("sum=%f\n", sum);
						q[(u8)sum]->overlapping[0] = (i >> 1) & 0x1;
						q[(u8)sum]->overlapping[1] = i & 0x1;
						q[(u8)sum]->overlapping[2] = (i >> 4) & 0x1;
						q[(u8)sum]->overlapping[3] = (i >> 2) & 0x1;
						q[(u8)sum]->overlapping[4] = (i >> 7) & 0x1;
						q[(u8)sum]->overlapping[5] = (i >> 3) & 0x1;
						q[(u8)sum]->overlapping[6] = (i >> 10) & 0x1;
						q[(u8)sum]->overlapping[7] = (i >> 5) & 0x1;
						q[(u8)sum]->overlapping[8] = (i >> 6) & 0x1;
						q[(u8)sum]->overlapping[9] = (i >> 13) & 0x1;
						q[(u8)sum]->overlapping[10] = (i >> 8) & 0x1;
						
						for (sum1 = 0.0, k = 0; k < 11; k++)sum1 += q[(u8)sum]->overlapping[k] * pow(2, k);
						q[(u8)sum]->overlap_value = sum1;
					}
					else if ((u8)sum == 3){
//						printf("sum=%f\n", sum);
						q[(u8)sum]->overlapping[0] = (i >> 2) & 0x1;
						q[(u8)sum]->overlapping[1] = i & 0x1;
						q[(u8)sum]->overlapping[2] = (i >> 1) & 0x1;
						q[(u8)sum]->overlapping[3] = (i >> 5) & 0x1;
						q[(u8)sum]->overlapping[4] = (i >> 8) & 0x1;
						q[(u8)sum]->overlapping[5] = (i >> 3) & 0x1;
						q[(u8)sum]->overlapping[6] = (i >> 4) & 0x1;
						q[(u8)sum]->overlapping[7] = (i >> 11) & 0x1;
						q[(u8)sum]->overlapping[8] = (i >> 6) & 0x1;
						q[(u8)sum]->overlapping[9] = (i >> 7) & 0x1;
						q[(u8)sum]->overlapping[10] = (i >> 14) & 0x1;

						for (sum1 = 0.0, k = 0; k < 11; k++)sum1 += q[(u8)sum]->overlapping[k] * pow(2, k);
						q[(u8)sum]->overlap_value = sum1;
					}
					else if ((u8)sum == 4){
//						printf("sum=%f\n", sum);
						q[(u8)sum]->overlapping[0] = (i >> 2) & 0x1;
						q[(u8)sum]->overlapping[1] = i & 0x1;
						q[(u8)sum]->overlapping[2] = (i >> 1) & 0x1;
						q[(u8)sum]->overlapping[3] = (i >> 5) & 0x1;
						q[(u8)sum]->overlapping[4] = (i >> 8) & 0x1;
						q[(u8)sum]->overlapping[5] = (i >> 3) & 0x1;
						q[(u8)sum]->overlapping[6] = (i >> 4) & 0x1;
						q[(u8)sum]->overlapping[7] = (i >> 11) & 0x1;
						q[(u8)sum]->overlapping[8] = (i >> 6) & 0x1;
						q[(u8)sum]->overlapping[9] = (i >> 7) & 0x1;
						q[(u8)sum]->overlapping[10] = (i >> 14) & 0x1;

						for (sum1 = 0.0, k = 0; k < 11; k++)sum1 += q[(u8)sum]->overlapping[k] * pow(2, k);
						q[(u8)sum]->overlap_value = sum1;
					}
					else if ((u8)sum == 5){
//						printf("sum=%f\n", sum);
						q[(u8)sum]->overlapping[0] = (i >> 1) & 0x1;
						q[(u8)sum]->overlapping[1] = i & 0x1;
						q[(u8)sum]->overlapping[2] = (i >> 4) & 0x1;
						q[(u8)sum]->overlapping[3] = (i >> 2) & 0x1;
						q[(u8)sum]->overlapping[4] = (i >> 7) & 0x1;
						q[(u8)sum]->overlapping[5] = (i >> 3) & 0x1;
						q[(u8)sum]->overlapping[6] = (i >> 10) & 0x1;
						q[(u8)sum]->overlapping[7] = (i >> 5) & 0x1;
						q[(u8)sum]->overlapping[8] = (i >> 6) & 0x1;
						q[(u8)sum]->overlapping[9] = (i >> 13) & 0x1;
						q[(u8)sum]->overlapping[10] = (i >> 8) & 0x1;

						for (sum1 = 0.0, k = 0; k < 11; k++)sum1 += q[(u8)sum]->overlapping[k] * pow(2, k);
						q[(u8)sum]->overlap_value = sum1;
					}
					else{
//						printf("sum=%f\n", sum);
						q[(u8)sum]->overlapping[0] = i & 0x1;
						q[(u8)sum]->overlapping[1] = (i >> 3) & 0x1;
						q[(u8)sum]->overlapping[2] = (i >> 1) & 0x1;
						q[(u8)sum]->overlapping[3] = (i >> 2) & 0x1;
						q[(u8)sum]->overlapping[4] = (i >> 6) & 0x1;
						q[(u8)sum]->overlapping[5] = (i >> 9) & 0x1;
						q[(u8)sum]->overlapping[6] = (i >> 4) & 0x1;
						q[(u8)sum]->overlapping[7] = (i >> 5) & 0x1;
						q[(u8)sum]->overlapping[8] = (i >> 12) & 0x1;
						q[(u8)sum]->overlapping[9] = (i >> 7) & 0x1;
						q[(u8)sum]->overlapping[10] = (i >> 8) & 0x1;

						for (sum1 = 0.0, k = 0; k < 11; k++)sum1 += q[(u8)sum]->overlapping[k] * pow(2, k);
						q[(u8)sum]->overlap_value = sum1;

					}
					q[(u8)sum]->next = NULL;
					q2[(u8)sum] = q[(u8)sum];

				}// corresponding to if (r[(u8)sum] == 0)
				else {
					r[(u8)sum]++;

					q3[(u8)sum] = (struct merge_state1 *)calloc(1, sizeof(struct merge_state1));
					if (NULL == q3[(u8)sum])
					{
						printf("Error in calloc in Merge_restricited_new3.\n");

					}
					
					q3[(u8)sum]->clock_control = (u8)sum;
//					(q3[(u8)sum]->clock_control) = j;
//					(*q3[(u8)sum]).clock_control = j;
//					printf("j=%d,ct=%u,index=%u\n", j,q3[(u8)sum]->clock_control, (u8)sum);
					for (k = 0; k < 15; k++)q3[(u8)sum]->ncastate[k] = (i >> k) & 0x1;
					q3[(u8)sum]->nca_value = i;


					if (((u8)sum == 0) || ((u8)sum == 7)){
//						printf("sum=%f\n", sum);
						for (k = 0; k < 9; k++)q3[(u8)sum]->overlapping[k] = (i >> k) & 0x1;

						q3[(u8)sum]->overlapping[9] = 20;
						q3[(u8)sum]->overlapping[10] = 20;

						for (sum1 = 0.0, k = 0; k < 9; k++)sum1 += q3[(u8)sum]->overlapping[k] * pow(2, k);
						q3[(u8)sum]->overlap_value = sum1;

					}
					else if ((u8)sum == 1){
//						printf("sum=%f\n", sum);
						q3[(u8)sum]->overlapping[0] = i & 0x1;
						q3[(u8)sum]->overlapping[1] = (i >> 3) & 0x1;
						q3[(u8)sum]->overlapping[2] = (i >> 1) & 0x1;
						q3[(u8)sum]->overlapping[3] = (i >> 2) & 0x1;
						q3[(u8)sum]->overlapping[4] = (i >> 6) & 0x1;
						q3[(u8)sum]->overlapping[5] = (i >> 9) & 0x1;
						q3[(u8)sum]->overlapping[6] = (i >> 4) & 0x1;
						q3[(u8)sum]->overlapping[7] = (i >> 5) & 0x1;
						q3[(u8)sum]->overlapping[8] = (i >> 12) & 0x1;
						q3[(u8)sum]->overlapping[9] = (i >> 7) & 0x1;
						q3[(u8)sum]->overlapping[10] = (i >> 8) & 0x1;

						for (sum1 = 0.0, k = 0; k < 11; k++)sum1 += q3[(u8)sum]->overlapping[k] * pow(2, k);
						q3[(u8)sum]->overlap_value = sum1;

					}
					else if ((u8)sum == 2){
//						printf("sum=%f\n", sum);
						q3[(u8)sum]->overlapping[0] = (i >> 1) & 0x1;
						q3[(u8)sum]->overlapping[1] = i & 0x1;
						q3[(u8)sum]->overlapping[2] = (i >> 4) & 0x1;
						q3[(u8)sum]->overlapping[3] = (i >> 2) & 0x1;
						q3[(u8)sum]->overlapping[4] = (i >> 7) & 0x1;
						q3[(u8)sum]->overlapping[5] = (i >> 3) & 0x1;
						q3[(u8)sum]->overlapping[6] = (i >> 10) & 0x1;
						q3[(u8)sum]->overlapping[7] = (i >> 5) & 0x1;
						q3[(u8)sum]->overlapping[8] = (i >> 6) & 0x1;
						q3[(u8)sum]->overlapping[9] = (i >> 13) & 0x1;
						q3[(u8)sum]->overlapping[10] = (i >> 8) & 0x1;

						for (sum1 = 0.0, k = 0; k < 11; k++)sum1 += q3[(u8)sum]->overlapping[k] * pow(2, k);
						q3[(u8)sum]->overlap_value = sum1;
					}
					else if ((u8)sum == 3){
//						printf("sum=%f\n", sum);
						q3[(u8)sum]->overlapping[0] = (i >> 2) & 0x1;
						q3[(u8)sum]->overlapping[1] = i & 0x1;
						q3[(u8)sum]->overlapping[2] = (i >> 1) & 0x1;
						q3[(u8)sum]->overlapping[3] = (i >> 5) & 0x1;
						q3[(u8)sum]->overlapping[4] = (i >> 8) & 0x1;
						q3[(u8)sum]->overlapping[5] = (i >> 3) & 0x1;
						q3[(u8)sum]->overlapping[6] = (i >> 4) & 0x1;
						q3[(u8)sum]->overlapping[7] = (i >> 11) & 0x1;
						q3[(u8)sum]->overlapping[8] = (i >> 6) & 0x1;
						q3[(u8)sum]->overlapping[9] = (i >> 7) & 0x1;
						q3[(u8)sum]->overlapping[10] = (i >> 14) & 0x1;;

						for (sum1 = 0.0, k = 0; k < 11; k++)sum1 += q3[(u8)sum]->overlapping[k] * pow(2, k);
						q3[(u8)sum]->overlap_value = sum1;
					}
					else if ((u8)sum == 4){
//						printf("sum=%f\n", sum);
						q3[(u8)sum]->overlapping[0] = (i >> 2) & 0x1;
						q3[(u8)sum]->overlapping[1] = i & 0x1;
						q3[(u8)sum]->overlapping[2] = (i >> 1) & 0x1;
						q3[(u8)sum]->overlapping[3] = (i >> 5) & 0x1;
						q3[(u8)sum]->overlapping[4] = (i >> 8) & 0x1;
						q3[(u8)sum]->overlapping[5] = (i >> 3) & 0x1;
						q3[(u8)sum]->overlapping[6] = (i >> 4) & 0x1;
						q3[(u8)sum]->overlapping[7] = (i >> 11) & 0x1;
						q3[(u8)sum]->overlapping[8] = (i >> 6) & 0x1;
						q3[(u8)sum]->overlapping[9] = (i >> 7) & 0x1;
						q3[(u8)sum]->overlapping[10] = (i >> 14) & 0x1;

						for (sum1 = 0.0, k = 0; k < 11; k++)sum1 += q3[(u8)sum]->overlapping[k] * pow(2, k);
						q3[(u8)sum]->overlap_value = sum1;
					}
					else if ((u8)sum == 5){
//						printf("sum=%f\n", sum);
						q3[(u8)sum]->overlapping[0] = (i >> 1) & 0x1;
						q3[(u8)sum]->overlapping[1] = i & 0x1;
						q3[(u8)sum]->overlapping[2] = (i >> 4) & 0x1;
						q3[(u8)sum]->overlapping[3] = (i >> 2) & 0x1;
						q3[(u8)sum]->overlapping[4] = (i >> 7) & 0x1;
						q3[(u8)sum]->overlapping[5] = (i >> 3) & 0x1;
						q3[(u8)sum]->overlapping[6] = (i >> 10) & 0x1;
						q3[(u8)sum]->overlapping[7] = (i >> 5) & 0x1;
						q3[(u8)sum]->overlapping[8] = (i >> 6) & 0x1;
						q3[(u8)sum]->overlapping[9] = (i >> 13) & 0x1;
						q3[(u8)sum]->overlapping[10] = (i >> 8) & 0x1;

						for (sum1 = 0.0, k = 0; k < 11; k++)sum1 += q3[(u8)sum]->overlapping[k] * pow(2, k);
						q3[(u8)sum]->overlap_value = sum1;
					}
					else{
//						printf("sum=%f\n", sum);
						q3[(u8)sum]->overlapping[0] = i & 0x1;
						q3[(u8)sum]->overlapping[1] = (i >> 3) & 0x1;
						q3[(u8)sum]->overlapping[2] = (i >> 1) & 0x1;
						q3[(u8)sum]->overlapping[3] = (i >> 2) & 0x1;
						q3[(u8)sum]->overlapping[4] = (i >> 6) & 0x1;
						q3[(u8)sum]->overlapping[5] = (i >> 9) & 0x1;
						q3[(u8)sum]->overlapping[6] = (i >> 4) & 0x1;
						q3[(u8)sum]->overlapping[7] = (i >> 5) & 0x1;
						q3[(u8)sum]->overlapping[8] = (i >> 12) & 0x1;
						q3[(u8)sum]->overlapping[9] = (i >> 7) & 0x1;
						q3[(u8)sum]->overlapping[10] = (i >> 8) & 0x1;

						for (sum1 = 0.0, k = 0; k < 11; k++)sum1 += q3[(u8)sum]->overlapping[k] * pow(2, k);
						q3[(u8)sum]->overlap_value = sum1;

					}
					q3[(u8)sum]->next = NULL;
					q2[(u8)sum]->next = q3[(u8)sum];
					q2[(u8)sum] = q3[(u8)sum];

				}
			

		}
		else continue;
	  }
//	  printf("sum=%f\n", sum);
	}//correspond to for(j)

	
	for (j = 4; j < 5; j++){
//		ph = NULL;
		ph = headq[j];

		do
		{
			ph->clock_control = j;
			ph = ph->next;
		} while (ph != NULL);
	}

	printf("sort begin\n");
	for (i = 0; i < 8; i++)Sort_mergestate1(headq[i], 0, length_mergestate1(headq[i]) - 1);
//	Sort_mergestate1(headq[4], 0, length_mergestate1(headq[4]) - 1);
//	Output_mergestate1(headq[4]);
	/*for (i = 0; i < 8; i++){
		printf("%u,", length_mergestate1(headq[i]));
	}
	printf("\n");*/

// merge the begin_point and the end_point

	for (j = 0; j < 8; j++){           // j is the clock control value between the two lists
//	for (j = 4; j < 5; j++){
		ph = NULL;
		ph = p[j];

		do {
//			ph1 = NULL;
			ph1 = headq[j];
//			printf("headq[%d]=%x\n", j, headq[j]);

			do{
				if ((ph1->overlap_value) == (ph->overlap_value)){
					if ((j == 0) || (j == 7)){
						for (i = 0; i < 15; i++)merge_array[i] = ph->ncastate[i];
						for (i = 15; i < 21;i++)merge_array[i] = ph1->ncastate[i-6];
						for (sum = 0.0, i = 0; i < 21; i++)sum += merge_array[i] * pow(2, i);
						merge_re[(u32)sum].hit_value++;
						merge_re[(u32)sum].ahead_value = ph->nca_value;
						merge_re[(u32)sum].behind_value = ph1->nca_value;
					}
					else if (j == 1){
						for (i = 0; i < 15; i++)merge_array[i] = ph->ncastate[i];
						for (i = 15; i < 17; i++)merge_array[i] = ph1->ncastate[i - 5];
						for (i = 17; i < 19; i++)merge_array[i] = ph1->ncastate[i - 4];
						for (sum = 0.0, i = 0; i < 19; i++)sum += merge_array[i] * pow(2, i);
						merge_re[(u32)sum].hit_value++;
						merge_re[(u32)sum].ahead_value = ph->nca_value;
						merge_re[(u32)sum].behind_value = ph1->nca_value;
					}
					else if (j == 2){
						for (i = 0; i < 15; i++)merge_array[i] = ph->ncastate[i];
						merge_array[15] = ph1->ncastate[9];
						merge_array[16] = ph1->ncastate[11];
						merge_array[17] = ph1->ncastate[12];
						merge_array[18] = ph1->ncastate[14];
						for (sum = 0.0, i = 0; i < 19; i++)sum += merge_array[i] * pow(2, i);
						merge_re[(u32)sum].hit_value++;
						merge_re[(u32)sum].ahead_value = ph->nca_value;
						merge_re[(u32)sum].behind_value = ph1->nca_value;
					}
					else if (j == 3){
						for (i = 0; i < 15; i++)merge_array[i] = ph->ncastate[i];
						merge_array[15] = ph1->ncastate[9];
						merge_array[16] = ph1->ncastate[10];
						merge_array[17] = ph1->ncastate[12];
						merge_array[18] = ph1->ncastate[13];
						for (sum = 0.0, i = 0; i < 19; i++)sum += merge_array[i] * pow(2, i);
						merge_re[(u32)sum].hit_value++;
						merge_re[(u32)sum].ahead_value = ph->nca_value;
						merge_re[(u32)sum].behind_value = ph1->nca_value;
					}
					else if (j == 4){
						for (i = 0; i < 15; i++)merge_array[i] = ph->ncastate[i];
//						printf("come here 1\n");
//						printf("ph1 is %x\n", ph1);
						merge_array[15] = ph1->ncastate[9];
						merge_array[16] = ph1->ncastate[10];
						merge_array[17] = ph1->ncastate[12];
						merge_array[18] = ph1->ncastate[13];
						for (sum = 0.0, i = 0; i < 19; i++)sum += merge_array[i] * pow(2, i);
						merge_re[(u32)sum].hit_value++;
						merge_re[(u32)sum].ahead_value = ph->nca_value;
						merge_re[(u32)sum].behind_value = ph1->nca_value;
					}
					else if (j == 5){
						for (i = 0; i < 15; i++)merge_array[i] = ph->ncastate[i];
						merge_array[15] = ph1->ncastate[9];
						merge_array[16] = ph1->ncastate[11];
						merge_array[17] = ph1->ncastate[12];
						merge_array[18] = ph1->ncastate[14];
						for (sum = 0.0, i = 0; i < 19; i++)sum += merge_array[i] * pow(2, i);
						merge_re[(u32)sum].hit_value++;
						merge_re[(u32)sum].ahead_value = ph->nca_value;
						merge_re[(u32)sum].behind_value = ph1->nca_value;
					}
					else {
						for (i = 0; i < 15; i++)merge_array[i] = ph->ncastate[i];
						for (i = 15; i < 17; i++)merge_array[i] = ph1->ncastate[i - 5];
						for (i = 17; i < 19; i++)merge_array[i] = ph1->ncastate[i - 4];
						for (sum = 0.0, i = 0; i < 19; i++)sum += merge_array[i] * pow(2, i);
						merge_re[(u32)sum].hit_value++;
						merge_re[(u32)sum].ahead_value = ph->nca_value;
						merge_re[(u32)sum].behind_value = ph1->nca_value;
					}
					ph1 = ph1->next;
				}
				else ph1 = ph1->next;
			} while (ph1 != NULL);

			ph = ph->next;
		} while (ph != NULL);

	}

	for (sum = 0.0, i = 0; i < 2097152; i++)
	if (merge_re[i].hit_value != 0)
	{
			sum++;
	}
	
	printf("sum=%f\n",sum);


}

u32 Merge_correct_restricted1(struct datalist2 *list[5], u8 begin_point, u8 end_point)  // merge list[begin_point] and list[end_point]
{
	u32 merge_re;
	int i, j;
	double sum;
	u8 merge_array[21];

	j = (int)(list[begin_point]->ISnca[6] * pow(2, 0) + list[begin_point]->ISnca[7] * pow(2, 1) + list[begin_point]->ISnca[8] * pow(2, 2));

	if ((j == 0) || (j == 7)){
		for (i = 0; i < 15; i++)merge_array[i] = list[begin_point]->ISnca[i];
		for (i = 15; i < 21; i++)merge_array[i] = list[end_point]->ISnca[i - 6];
		for (sum = 0.0, i = 0; i < 21; i++)sum += merge_array[i] * pow(2, i);
		merge_re =(u32)sum;
	}
	else if (j == 1){
		for (i = 0; i < 15; i++)merge_array[i] = list[begin_point]->ISnca[i];
		for (i = 15; i < 17; i++)merge_array[i] = list[end_point]->ISnca[i - 5];
		for (i = 17; i < 19; i++)merge_array[i] = list[end_point]->ISnca[i - 4];
		for (sum = 0.0, i = 0; i < 19; i++)sum += merge_array[i] * pow(2, i);
		merge_re = (u32)sum;
	}
	else if (j == 2){
		for (i = 0; i < 15; i++)merge_array[i] = list[begin_point]->ISnca[i];
		merge_array[15] = list[end_point]->ISnca[9];
		merge_array[16] = list[end_point]->ISnca[11];
		merge_array[17] = list[end_point]->ISnca[12];
		merge_array[18] = list[end_point]->ISnca[14];
		for (sum = 0.0, i = 0; i < 19; i++)sum += merge_array[i] * pow(2, i);
		merge_re = (u32)sum;
	}
	else if (j == 3){
		for (i = 0; i < 15; i++)merge_array[i] = list[begin_point]->ISnca[i];
		merge_array[15] = list[end_point]->ISnca[9];
		merge_array[16] = list[end_point]->ISnca[10];
		merge_array[17] = list[end_point]->ISnca[12];
		merge_array[18] = list[end_point]->ISnca[13];
		for (sum = 0.0, i = 0; i < 19; i++)sum += merge_array[i] * pow(2, i);
		merge_re = (u32)sum;
	}
	else if (j == 4){
		for (i = 0; i < 15; i++)merge_array[i] = list[begin_point]->ISnca[i];
		merge_array[15] = list[end_point]->ISnca[9];
		merge_array[16] = list[end_point]->ISnca[10];
		merge_array[17] = list[end_point]->ISnca[12];
		merge_array[18] = list[end_point]->ISnca[13];
		for (sum = 0.0, i = 0; i < 19; i++)sum += merge_array[i] * pow(2, i);
		merge_re = (u32)sum;
	}
	else if (j == 5){
		for (i = 0; i < 15; i++)merge_array[i] = list[begin_point]->ISnca[i];
		merge_array[15] = list[end_point]->ISnca[9];
		merge_array[16] = list[end_point]->ISnca[11];
		merge_array[17] = list[end_point]->ISnca[12];
		merge_array[18] = list[end_point]->ISnca[14];
		for (sum = 0.0, i = 0; i < 19; i++)sum += merge_array[i] * pow(2, i);
		merge_re = (u32)sum;
	}
	else {
		for (i = 0; i < 15; i++)merge_array[i] = list[begin_point]->ISnca[i];
		for (i = 15; i < 17; i++)merge_array[i] = list[end_point]->ISnca[i - 5];
		for (i = 17; i < 19; i++)merge_array[i] = list[end_point]->ISnca[i - 4];
		for (sum = 0.0, i = 0; i < 19; i++)sum += merge_array[i] * pow(2, i);
		merge_re = (u32)sum;
	}

	return(merge_re);

}

u8 check_merge_candidate1(struct merge_state1a *merge_re, u32 in)
{
	if (merge_re[(u32)in].hit_value != 0)return(1);
	else return(0);
}

//u32 Merge_correct_restricted2(struct datalist2 *list[5], u8 begin_point, u8 mid_point, u8 end_point)  // merge list[begin_point], list[middle_point] and list[end_point]
//{
//	u32 merge_re;
//	int i, j, k;
//	double sum;
//	u8 merge_array[27];
//
//	memset(merge_array,0,27);
//	j = (int)(list[begin_point]->ISnca[6] * pow(2, 0) + list[begin_point]->ISnca[7] * pow(2, 1) + list[begin_point]->ISnca[8] * pow(2, 2));
//	k = (int)(list[mid_point]->ISnca[6] * pow(2, 0) + list[mid_point]->ISnca[7] * pow(2, 1) + list[mid_point]->ISnca[8] * pow(2, 2));
//
//	if ((j == 0) || (j == 7)){
//		for (i = 0; i < 15; i++)merge_array[i] = list[begin_point]->ISnca[i];
//		for (i = 15; i < 21; i++)merge_array[i] = list[mid_point]->ISnca[i - 6];
//
//		if ((k == 0) && (k == 7)){
//			for (i = 21; i < 27; i++)merge_array[i] = list[end_point]->ISnca[i - 12];
//
//			for (sum = 0.0, i = 0; i < 27; i++)sum += merge_array[i] * pow(2, i);
//			merge_re = (u32)sum;
//		}
//		else if (k == 1){
//			
//			for (sum = 0.0, i = 0; i < 21; i++)sum += merge_array[i] * pow(2, i);
//			merge_re = (u32)sum;
//		}
//		else if (k == 2){
//			
//			for (sum = 0.0, i = 0; i < 21; i++)sum += merge_array[i] * pow(2, i);
//			merge_re = (u32)sum;
//		}
//		else if (k == 3){
//			
//			for (sum = 0.0, i = 0; i < 21; i++)sum += merge_array[i] * pow(2, i);
//			merge_re = (u32)sum;
//		}
//		else if (k == 4){
//			
//			for (sum = 0.0, i = 0; i < 21; i++)sum += merge_array[i] * pow(2, i);
//			merge_re = (u32)sum;
//		}
//		else if (k == 5){
//			
//			for (sum = 0.0, i = 0; i < 21; i++)sum += merge_array[i] * pow(2, i);
//			merge_re = (u32)sum;
//		}
//		else{
//			
//			for (sum = 0.0, i = 0; i < 21; i++)sum += merge_array[i] * pow(2, i);
//			merge_re = (u32)sum;
//		}
//
//		
//	}
//	else if (j == 1){
//		for (i = 0; i < 15; i++)merge_array[i] = list[begin_point]->ISnca[i];
//		for (i = 15; i < 17; i++)merge_array[i] = list[end_point]->ISnca[i - 5];
//		for (i = 17; i < 19; i++)merge_array[i] = list[end_point]->ISnca[i - 4];
//		for (sum = 0.0, i = 0; i < 19; i++)sum += merge_array[i] * pow(2, i);
//		merge_re = (u32)sum;
//	}
//	else if (j == 2){
//		for (i = 0; i < 15; i++)merge_array[i] = list[begin_point]->ISnca[i];
//		merge_array[15] = list[end_point]->ISnca[9];
//		merge_array[16] = list[end_point]->ISnca[11];
//		merge_array[17] = list[end_point]->ISnca[12];
//		merge_array[18] = list[end_point]->ISnca[14];
//		for (sum = 0.0, i = 0; i < 19; i++)sum += merge_array[i] * pow(2, i);
//		merge_re = (u32)sum;
//	}
//	else if (j == 3){
//		for (i = 0; i < 15; i++)merge_array[i] = list[begin_point]->ISnca[i];
//		merge_array[15] = list[end_point]->ISnca[9];
//		merge_array[16] = list[end_point]->ISnca[10];
//		merge_array[17] = list[end_point]->ISnca[12];
//		merge_array[18] = list[end_point]->ISnca[13];
//		for (sum = 0.0, i = 0; i < 19; i++)sum += merge_array[i] * pow(2, i);
//		merge_re = (u32)sum;
//	}
//	else if (j == 4){
//		for (i = 0; i < 15; i++)merge_array[i] = list[begin_point]->ISnca[i];
//		merge_array[15] = list[end_point]->ISnca[9];
//		merge_array[16] = list[end_point]->ISnca[10];
//		merge_array[17] = list[end_point]->ISnca[12];
//		merge_array[18] = list[end_point]->ISnca[13];
//		for (sum = 0.0, i = 0; i < 19; i++)sum += merge_array[i] * pow(2, i);
//		merge_re = (u32)sum;
//	}
//	else if (j == 5){
//		for (i = 0; i < 15; i++)merge_array[i] = list[begin_point]->ISnca[i];
//		merge_array[15] = list[end_point]->ISnca[9];
//		merge_array[16] = list[end_point]->ISnca[11];
//		merge_array[17] = list[end_point]->ISnca[12];
//		merge_array[18] = list[end_point]->ISnca[14];
//		for (sum = 0.0, i = 0; i < 19; i++)sum += merge_array[i] * pow(2, i);
//		merge_re = (u32)sum;
//	}
//	else {
//		for (i = 0; i < 15; i++)merge_array[i] = list[begin_point]->ISnca[i];
//		for (i = 15; i < 17; i++)merge_array[i] = list[end_point]->ISnca[i - 5];
//		for (i = 17; i < 19; i++)merge_array[i] = list[end_point]->ISnca[i - 4];
//		for (sum = 0.0, i = 0; i < 19; i++)sum += merge_array[i] * pow(2, i);
//		merge_re = (u32)sum;
//	}
//
//	return(merge_re);
//
//}
//


u8 Dec2binary(u32 input_number)
{
	int i, j;
	u8 binary[21];
	double output_number;

	memset(binary,0,21);
	output_number = 0;

	for (i = 0; i < 21; i++){
		binary[i] = (input_number >> i) & 0x1;
	}
	for (output_number = 0.0, i = 0; i < 21; i++)output_number += binary[i] * pow(2, i);

	if ((u32)output_number == input_number)return(1);
	else return(0);
}

void Merge_restricited_new4(struct merge_state1a *merge_re[4], u8 begin_point, u8 end_point, struct merge_state2a *merge_re2)
{
	int i, j, k;
	double sum, sum1;
	struct merge_state2 *p[32768], *head[32768], *p2[32768], *p3[32768];
	struct merge_state2 *q[32768], *headq[32768], *q2[32768], *q3[32768];
	struct merge_state2 *ph, *ph1;
	u32 r[32768];
	u8 merge_array[21];

	for (i = 0; i < 21; i++)merge_array[i] = 0;
	for (i = 0; i < 32768; i++)r[i] = 0;
	for (i = 0; i < 32768; i++)p[i] = NULL;
	for (i = 0; i < 32768; i++)p2[i] = NULL;
	for (i = 0; i < 32768; i++)p3[i] = NULL;
	for (i = 0; i < 32768; i++)head[i] = NULL;

	for (i = 0; i < 32768; i++)q[i] = NULL;
	for (i = 0; i < 32768; i++)q2[i] = NULL;
	for (i = 0; i < 32768; i++)q3[i] = NULL;
	for (i = 0; i < 32768; i++)headq[i] = NULL;

	
	for (i = 0; i < 32768; i++){
		p[i] = (struct merge_state2 *)calloc(1, sizeof(struct merge_state2));      
		if (p[i] == NULL){
			printf("error in calloc in p[i] in Merge_restricited_new4\n");
		}
	}
	
	// prepare the begin_point sublists
	for (i = 0; i < 2097152; i++){
		if (((merge_re[begin_point] + i)->hit_value) != 0){
			if (r[(u32)((merge_re[begin_point] + i)->behind_value)] == 0){
				r[(u32)((merge_re[begin_point] + i)->behind_value)]++;
				head[(u32)((merge_re[begin_point] + i)->behind_value)] = p[(u32)((merge_re[begin_point] + i)->behind_value)];
				p[(u32)((merge_re[begin_point] + i)->behind_value)]->overlap_value = (merge_re[begin_point] + i)->behind_value;
				for (j = 0; j < 15; j++)p[(u32)((merge_re[begin_point] + i)->behind_value)]->overlapping[j] = ((u32)((merge_re[begin_point] + i)->behind_value) >> j) & 0x1;
				p[(u32)((merge_re[begin_point] + i)->behind_value)]->next = NULL;
				p2[(u32)((merge_re[begin_point] + i)->behind_value)] = p[(u32)((merge_re[begin_point] + i)->behind_value)];
			}
			else {
				r[(u32)((merge_re[begin_point] + i)->behind_value)]++;

				p3[(u32)((merge_re[begin_point] + i)->behind_value)] = (struct merge_state2 *)calloc(1, sizeof(struct merge_state2));
				if (NULL == p3[(u32)((merge_re[begin_point] + i)->behind_value)])
				{
					printf("Error in calloc in Merge_restricited_new4.\n");

				}
				p3[(u32)((merge_re[begin_point] + i)->behind_value)]->overlap_value = (merge_re[begin_point] + i)->behind_value;
				for (j = 0; j < 15; j++)p3[(u32)((merge_re[begin_point] + i)->behind_value)]->overlapping[j] = ((u32)((merge_re[begin_point] + i)->behind_value) >> j) & 0x1;

				p3[(u32)((merge_re[begin_point] + i)->behind_value)]->next = NULL;
				p2[(u32)((merge_re[begin_point] + i)->behind_value)]->next = p3[(u32)((merge_re[begin_point] + i)->behind_value)];
				p2[(u32)((merge_re[begin_point] + i)->behind_value)] = p3[(u32)((merge_re[begin_point] + i)->behind_value)];
			}
		}
	}

	Output_mergestate2(p[4]);
	for (i = 0; i < 32768; i++){
	printf("i=%d:%u\n", i,length_mergestate2(p[i]));
	}
	for (j=0,i = 0; i < 32768; i++){
		if (length_mergestate2(p[i]) == r[i])j++;
	}
	printf("j=%d\n",j);
	for (j = 0, i = 0; i < 32768; i++){
		if (length_mergestate2(p[i]) == 0)j++;
	}
	printf("j=%d\n", j);

	
	for (i = 0; i < 32768; i++)r[i] = 0;
	for (i = 0; i < 32768; i++)q[i] = NULL;
	for (i = 0; i < 32768; i++)q2[i] = NULL;
	for (i = 0; i < 32768; i++)q3[i] = NULL;
	for (i = 0; i < 32768; i++)headq[i] = NULL;

	for (i = 0; i < 32768; i++){
		q[i] = (struct merge_state2 *)calloc(1, sizeof(struct merge_state2));      
		if (q[i] == NULL){
			printf("error in calloc in q[i] in Merge_restricited_new4\n");
		}
	}
	
	
	// prepare the end_point sublists
	for (i = 0; i < 2097152; i++){
		if (((merge_re[end_point] + i)->hit_value) != 0){
			if (r[(u32)((merge_re[end_point] + i)->ahead_value)] == 0){
				r[(u32)((merge_re[end_point] + i)->ahead_value)]++;
				headq[(u32)((merge_re[end_point] + i)->ahead_value)] = q[(u32)((merge_re[end_point] + i)->ahead_value)];
				q[(u32)((merge_re[end_point] + i)->ahead_value)]->overlap_value = (merge_re[end_point] + i)->ahead_value;
				for (j = 0; j < 15; j++)q[(u32)((merge_re[end_point] + i)->ahead_value)]->overlapping[j] = ((u32)((merge_re[end_point] + i)->ahead_value) >> j) & 0x1;
				q[(u32)((merge_re[end_point] + i)->ahead_value)]->next = NULL;
				q2[(u32)((merge_re[end_point] + i)->ahead_value)] = q[(u32)((merge_re[end_point] + i)->ahead_value)];
			}
			else {
				r[(u32)((merge_re[end_point] + i)->ahead_value)]++;

				q3[(u32)((merge_re[end_point] + i)->ahead_value)] = (struct merge_state2 *)calloc(1, sizeof(struct merge_state2));
				if (NULL == q3[(u32)((merge_re[end_point] + i)->ahead_value)])
				{
					printf("Error in calloc in Merge_restricited_new4.\n");

				}
				q3[(u32)((merge_re[end_point] + i)->ahead_value)]->overlap_value = (merge_re[end_point] + i)->ahead_value;
				for (j = 0; j < 15; j++)q3[(u32)((merge_re[end_point] + i)->ahead_value)]->overlapping[j] = ((u32)((merge_re[end_point] + i)->ahead_value) >> j) & 0x1;

				q3[(u32)((merge_re[end_point] + i)->ahead_value)]->next = NULL;
				q2[(u32)((merge_re[end_point] + i)->ahead_value)]->next = q3[(u32)((merge_re[end_point] + i)->ahead_value)];
				q2[(u32)((merge_re[end_point] + i)->ahead_value)] = q3[(u32)((merge_re[end_point] + i)->ahead_value)];
			}
		}
	}

	Output_mergestate2(q[4]);
	for (i = 0; i < 32768; i++){
		printf("i=%d:%u\n", i, length_mergestate2(q[i]));
	}
	for (j = 0, i = 0; i < 32768; i++){
		if (length_mergestate2(q[i]) == r[i])j++;
	}
	printf("j=%d\n", j);
	for (j = 0, i = 0; i < 32768; i++){
		if (length_mergestate2(q[i]) == 0)j++;
	}
	printf("j=%d\n", j);

	for (i = 0; i < 32768; i++){

	}

}




