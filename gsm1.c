#include "merge.h"

int main()
{
	int i, j;
	u8 key[8];
	u32 frame, R1, R2, R3, R1a, R2a, R3a;
	u8 AtoBkeystream[15], BtoAkeystream[15];
	u8 check = 0,ns[15],z[2];
	struct isd2 *isd2_table[4][4];
	double Pr[4][4];
	struct datalist2 *list2[5];
	struct datalist3 *list3[4];
	struct datalist4 *list4[3];
	struct datalist5 *list5[2];
	struct initialdata2 *initiallist;
	u16  *try1,*result[5];
	struct merge_state1a *merge_re[4];
	u32 in = 0;
	struct merge_state2a *merge_re2[3];
	u8 check_merge_result;

	
	srand((unsigned)time(NULL));
	rc4_setup();

	R1 = 0;
	R2 = 0;
	R3 = 0;
	R1a = 0;
	R2a = 0;
	R3a = 0;
	frame = 0;
	memset(ns,0,15);
	for (i = 0; i < 4; i++)
	for (j = 0; j < 4; j++)isd2_table[i][j] = NULL;
	for (j = 0; j < 6;j++)list2[j] = NULL;
	for (j = 0; j < 4; j++)list3[j] = NULL;
	for (j = 0; j < 3; j++)list4[j] = NULL;
	for (j = 0; j < 2; j++)list5[j] = NULL;
	initiallist = NULL;
	try1 = NULL;
	for (i = 0; i < 4;i++)merge_re[i] = NULL;

	for (j = 0; j < 5; j++){
		list2[j] = (struct datalist2 *)calloc(1, sizeof(struct datalist2));
		if (list2[j] == NULL){
			printf("error in calloc in main function\n");
		}
	}

	for (j = 0; j < 4; j++){
		list3[j] = (struct datalist3 *)calloc(1, sizeof(struct datalist3));
		if (list3[j] == NULL){
			printf("error in calloc in main function\n");
		}
	}

	for (j = 0; j < 3; j++){
		list4[j] = (struct datalist4 *)calloc(1, sizeof(struct datalist4));
		if (list4[j] == NULL){
			printf("error in calloc in main function\n");
		}
	}

	for (j = 0; j < 2; j++){
		list5[j] = (struct datalist5 *)calloc(1, sizeof(struct datalist5));
		if (list5[j] == NULL){
			printf("error in calloc in main function\n");
		}
	}
	initiallist = (struct initialdata2 *)calloc(1, sizeof(struct initialdata2));
	if (initiallist == NULL){
		printf("error in calloc in main function\n");
	}
	try1 = (u16 *)calloc(32768, sizeof(u16));
	if (try1 == NULL){
		printf("error in calloc in try1 in main function\n");
	}

	for (i = 0; i < 5; i++){
		result[i] = (u16 *)calloc(32768, sizeof(u16));
		if (result[i] == NULL){
			printf("error in calloc in result[i] in main function\n");
		}
	}

	for (i = 0; i < 4; i++){
		merge_re[i] = (struct merge_state1a *)calloc(2097152, sizeof(struct merge_state1a));    // 2097152=2^21
		if (merge_re[i] == NULL){
			printf("error in calloc in merge_re in main function\n");
		}
	}

	for (i = 0; i < 4; i++)
	for (j = 0; j < 4; j++)Pr[i][j] = 0.0;

	for (i = 0; i <= 113 / 8; i++)AtoBkeystream[i] = BtoAkeystream[i] = 0;
	for (i = 0; i < 8; i++)key[i] = 0;

	// key[8] = { 0x12, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF }

	key[0] = 0x12;
	key[1] = 0x23;
	key[2] = 0x45;
	key[3] = 0x67;
	key[4] = 0x89;
	key[5] = 0xab;
	key[6] = 0xcd;
	key[7] = 0xef;

	frame = 0x134;

	for (i = 0; i < 8; i++)key[i] = rc4();
	frame = ( (rc4() << 16) | (rc4() << 8) | rc4() ) & 0x3fffff;

	/*for (i = 0; i<8; i++)printf("%02X", key[i]);
	printf("\n");
	printf("%08x", frame);
	printf("\n");*/

//	keysetup(key, frame, &R1, &R2, &R3);
	keysetup2(key, frame, &R1, &R2, &R3, &R1a, &R2a, &R3a);
	GSM_onesession_run(AtoBkeystream, BtoAkeystream, &R1, &R2, &R3);
//	GSM_onesession_run_equv(AtoBkeystream, BtoAkeystream, &R1, &R2, &R3);
	/*check = clock_stopgo_equv(&R1, &R2, &R3);
	printf("%0X\n", check);*/

	printf("A->B: 0x");
	for (i = 0; i<15; i++)printf("%02X", AtoBkeystream[i]);
	printf("\n");
	printf("B->A: 0x");
	for (i = 0; i<15; i++)printf("%02X", BtoAkeystream[i]);
	printf("\n");

//	j = Compare_clock(&R1, &R2, &R3);
//	j = Compare_clock2bits(&R1, &R2, &R3);
//	printf("j=%d\n",j);

	Precomputation(ns, isd2_table);
	Isd2Pr_diversity(isd2_table, Pr);
	output_Pr(Pr);
	printf("-------------------------------------------------------------------\n");

//	Attacktarget_preparation2(initiallist, &R1, &R2, &R3, list2, 0);

//	initiallist = Targetkeystream_generation(&R1, &R2, &R3, initiallist);
	initiallist = Targetkeystream_generation2(&R1a, &R2a, &R3a, initiallist);
	for (j = 0; j < 5; j++)Prepare_ncadata(initiallist, list2[j], j);
	for (j = 0; j < 4; j++)Prepare_ncadata3(initiallist, list3[j], j);
	for (j = 0; j < 3; j++)Prepare_ncadata4(initiallist, list4[j], j);
	for (j = 0; j < 2; j++)Prepare_ncadata5(initiallist, list5[j], j);
	
//
////	try1 = FirstSet_preparation(try1, list2, isd2_table, &R1, &R2, &R3);
//	try1 = FirstSet_preparation2(try1, list2, isd2_table, &R1, &R2, &R3);

//	Test_constant2(list2[0], isd2_table, &R1, &R2, &R3);
//	Test_average2(list2[0], isd2_table, &R1, &R2, &R3);
//	Test_const_average2(list2[0], isd2_table, &R1, &R2, &R3);

//	Merge_normal(initiallist, list2, isd2_table, &R1, &R2, &R3);

//	Merge_normal(initiallist, list2[0], isd2_table, &R1, &R2, &R3);

	/*for (j = 0; j < 5;j++)Merge_normal_new(list2[j], isd2_table, &R1, &R2, &R3, j);
	printf("\n");*/

	for (j = 0; j < 5; j++)result[j] = Merge_normal_new2(list2[j], isd2_table, &R1, &R2, &R3, j,result[j]);
	Counting_result(result);
	Checking_result(result, list2);
	printf("\n");

	Merge_restricited_new2(result, 0, 1, merge_re[0]);

//	for (j = 0; j < 4;j++)Merge_restricited_new3(result, j, j+1, merge_re[j]);
	Merge_restricited_new3(result, 0, 1, merge_re[0]);
	Merge_restricited_new3(result, 1, 2, merge_re[1]);
	Merge_restricited_new3(result, 2, 3, merge_re[2]);
	Merge_restricited_new3(result, 3, 4, merge_re[3]);

	printf("---------------\n");
	for (j = 0; j < 5; j++){
		for (i = 0; i < 2; i++)printf("%x",list2[j]->prefix[i]);
		printf("\n");
		for (i = 0; i < 15;i++)printf("%x",list2[j]->ISnca[i]);
		printf("\n");
	}
	printf("---------------\n");

//	printf("---------------\n");
	for (j = 0; j < 4; j++){
		for (i = 0; i < 3; i++)printf("%x", list3[j]->prefix[i]);
		printf("\n");
		for (i = 0; i < 21; i++)printf("%x", list3[j]->ISnca[i]);
		printf("\n");
	}
	printf("---------------\n");

	for (j = 0; j < 3; j++){
		for (i = 0; i < 4; i++)printf("%x", list4[j]->prefix[i]);
		printf("\n");
		for (i = 0; i < 27; i++)printf("%x", list4[j]->ISnca[i]);
		printf("\n");
	}
	printf("---------------\n");

	for (j = 0; j < 2; j++){
		for (i = 0; i < 5; i++)printf("%x", list5[j]->prefix[i]);
		printf("\n");
		for (i = 0; i < 33; i++)printf("%x", list5[j]->ISnca[i]);
		printf("\n");
	}
	printf("---------------\n");
	
	for (j = 0; j < 4; j++){
		in = Merge_correct_restricted1(list2, j, j + 1);
		printf("The %dth merged state is %u: ",j,in);
		check_merge_result = check_merge_candidate1(merge_re[j], in);
		printf("%u\n", check_merge_result);
	}
	printf("\n");

	//for (i = 0; i < 2; i++){
	//	merge_re2[i] = (struct merge_state2a *)calloc(134217728, sizeof(struct merge_state2a));    // 134217728=2^27
	//	if (merge_re2[i] == NULL){
	//		printf("error in calloc in merge_re2 in main function\n");
	//	}
	//}

	for (i = 0; i < 2; i++){
		merge_re2[i] = (struct merge_state2a *)calloc(1, sizeof(struct merge_state2a));    // 134217728=2^27
		if (merge_re2[i] == NULL){
			printf("error in calloc in merge_re2 in main function\n");
		}
	}

	/*int sum;
	for (sum=0,i = 0; i < 2097152;i++)sum +=Dec2binary(i);
	printf("sum=%d\n",sum);*/

//	Merge_restricited_new4(merge_re, 0, 1, merge_re2[0]);



	/*for (j = 0; j < 15; j++)printf("%u", list2[0]->ISnca[j]);
	printf("\n");
	for (j = 0; j < 15; j++)printf("%u", list2[1]->ISnca[j]);

	Merge_restricited_new(list2, 0, 1);
	printf("\n");
	memset(z,0,2);*/

	/*randomIV(ns, 15);
	for (i = 0; i < 15; i++)printf("%x,",ns[i]);
	printf("\n");
	clock_stopgo_2bits(ns, z);
	for (i = 0; i < 2; i++)printf("%x,",z[i]);
	printf("\n");
	Extractnca_arbitrary(ns, z);
	printf("The restored state is:\n");
	for (i = 0; i < 15; i++)printf("%x,", ns[i]);
	printf("\n");
	clock_stopgo_2bits(ns, z);
	for (i = 0; i < 2; i++)printf("%x,", z[i]);
	printf("\n");*/

	for (j = 0; j < 5; j++)free(list2[j]);
	for (j = 0; j < 4; j++)free(list3[j]);
	for (j = 0; j < 3; j++)free(list4[j]);
	for (j = 0; j < 2; j++)free(list5[j]);
	free(try1);
	free(initiallist);
	for (i = 0; i < 5; i++)free(result[i]);
	for (i = 0; i < 4; i++)free(merge_re[i]);

	return(0);
}