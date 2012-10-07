/*
 * 2012/10/07
 * written by tharada
 *
 * steady-state Genetic Algorithm (steady-state GA)$B$rMQ$$$?(BOne-Max$BLdBj$N2rK!(B
 * One-Max$BLdBj(B(One-Max problem)
 * $B0dEA;R$NMWAG$G(B1$B$N8D?t$,B?$$$[$II>2ACM$,9b$/$J$kLdBj(B
 * $B:GNI2r$O0dEA;R$NA4MWAG$,(B1$B$G$"$k8DBN(B
 */

#include<stdio.h>
#include<stdlib.h>
#include<time.h>

#define POP_SIZE 100					// $B8DBN?t(B
#define CHROM_LEN 20					// $B0dEA;RD9(B
#define MAX_GENERATION 500				// $B:GBg@$Be?t(B
#define CROSSOVER_RATE 0.5				// $B8r:5N((B
#define MUTATION_RATE (1.0 / CHROM_LEN)	// $BFMA3JQ0[N((B

/*
 * GA$B$N8DBNMQ$N9=B$BN(B
 * Chromsome:	$B0dEA;R!"(B0/1$B$N%S%C%HNs(B
 * Fitness:		$BE,9gEY(B
 */
struct _Individual
{
	int Chromsome[CHROM_LEN];	// $B0dEA;R(B
	int Fitness;				// $BE,9gEY(B
};
typedef struct _Individual Individual;

Individual Population[POP_SIZE];	// $BJl=8CD(B
Individual Children[2];				// $B;R8DBN@8@.MQ(B

/**************************************************/
/*                                                */
/* $BI=<(4X?t(B                                       */
/*                                                */
/**************************************************/

/*
 * Population$BG[NsFb$N(Bpop_i$BHVL\$N8DBN$r2<5-$N7A<0$GI=<((B
 * xxxxxxxxxx: fitness=yyy
 *
 * <argument(s)>
 * pop_i: $BI=<($9$k8DBN$N%$%s%G%C%/%9(B
 */
void print_chrom(int pop_i)
{
	int i;

	// $B0dEA;R$NI=<((B
	for(i = 0; i < CHROM_LEN; i++){
		printf("%d", Population[pop_i].Chromsome[i]);
	}

	// $BE,9gEY$NI=<((B
	printf(": fitness=%d\n", Population[pop_i].Fitness);
}

/*
 * $BJl=8CDFb$NA48DBN$rI=<((B
 * $BI=<(7A<0$O(Bprint_chrom$B4X?t$K0MB8(B
 */
void print_all(void)
{
	int i;

	for(i = 0; i < POP_SIZE; i++){
		print_chrom(i);
	}
}

/**************************************************/
/*                                                */
/* $BI=<(4X?t$*$o$j(B                                 */
/*                                                */
/**************************************************/

/**************************************************/
/*                                                */
/* GA$BK\BN(B                                         */
/*                                                */
/**************************************************/

/*
 * Population$BG[NsFb$N(Bpop_i$BHVL\$N8DBN$NE,9gEY$r7W;;(B
 * $B:#2s$NLdBj$O0dEA;RFb$N(B1$B$N8D?t$,B?$$$[$INI$$2r$H$9$k$N$G(B
 * 1$B$N8D?t$r$=$N$^$^E,9gEY$K$9$k(B
 *
 * <argument(s)>
 * pop_i: $BE,9gEY$r7W;;$9$k8DBN$N%$%s%G%C%/%9(B
 */
void calc_fitness(int pop_i)
{
	int sum = 0;
	int i;

	// $B0dEA;RFb$N(B1$B$N8D?t$r;;=P(B
	for(i = 0; i < CHROM_LEN; i++){
		if(Population[pop_i].Chromsome[i] == 1){
			sum++;
		}
	}

	// 1$B$N8D?t$,$=$N$^$^8DBN$NE,9gEY(B
	Population[pop_i].Fitness = sum;
}

/*
 * $BJl=8CDFb$NA48DBN$NE,9gEY$r7W;;(B
 */
void calc_fit_all(void)
{
	int i;

	for(i = 0; i < POP_SIZE; i++){
		calc_fitness(i);
	}
}

/*
 * $BJl=8CD$rE,9gEY$G%=!<%F%#%s%0(B
 */
void sort_population(void)
{
	int i, j;
	Individual tmp1, tmp2;

	for(i = 0; i < POP_SIZE - 1; i++){
		for(j = i; j < POP_SIZE; j++){
			tmp1 = Population[i];
			tmp2 = Population[j];
			if(tmp1.Fitness < tmp2.Fitness){
				Population[i] = tmp2;
				Population[j] = tmp1;
			}
		}
	}
}

/*
 * $BJl=8CD$r%i%s%@%`$K=i4|2=(B
 */
void init_chrom(void)
{
	int i, j;

	for(i = 0; i < POP_SIZE; i++){
		for(j = 0; j < CHROM_LEN; j++){
			// $B0dEA;R$N3FMWAG$r(B0/1$B$G%i%s%@%`$K=i4|2=(B
			Population[i].Chromsome[j] = rand() % 2;
		}
		calc_fitness(i);
	}
}

/*
 * $BJl=8CD$NCf$+$i?F$H$J$k8DBN$rA*Br(B
 * $B:#2s$O%H!<%J%a%s%HA*Br$r;HMQ(B
 *
 * $B%H!<%J%a%s%HA*Br(B(tournament selection)
 * $BJl=8CD$NCf$+$i%i%s%@%`$K(Bn$B8DBNA*Br$7!"$=$NCf$G:G$bI>2A$N9b$$8DBN$rA*Br(B
 * n$B$r%H!<%J%a%s%H%5%$%:$H8F$V(B
 * $B:#2s$O%H!<%J%a%s%H%5%$%:$O(Bn=2
 *
 * <return>
 * $BA*Br$7$?8DBN$N%$%s%G%C%/%9(B
 */
int select_chrom(void)
{
	int i1, i2;
	int ret;
	
	// $B%i%s%@%`$K(B1$B8DBNA*Br(B
	i1 = rand() % POP_SIZE;
	// $B$b$&(B1$B8DBN$rHo$i$J$$$h$&$KA*Br(B
	do{
		i2 = rand() % POP_SIZE;
	} while(i1 == i2);

	// $BE,9gEY$rHf3S$7!"I>2A$N9b$$8DBN$rLa$jCM$H$7$FA*Br(B
	if(Population[i1].Fitness >= Population[i2].Fitness){
		ret = i1;
	}
	else{
		ret = i2;
	}

	return ret;
}

/*
 * $BJl=8CD$NCf$+$i(B2$B8DBN$rA*Br$7(BChildren$BG[Ns$KJ]B8(B
 */
void generate_children(void)
{
	int c1, c2;

	// $B%H!<%J%a%s%HA*Br(B
	c1 = select_chrom();
	c2 = select_chrom();
	
	// $BA*Br$7$?8DBN$r(BChildren$BG[Ns$KJ]B8(B
	Children[0] = Population[c1];
	Children[1] = Population[c2];
}

/*
 * 2$B$D$N;R8DBN$KBP$7$F8r:5N((B(CROSSOVER_RATE)$B$K$b$H$E$$$F8r:5$r<B9T(B
 * $B:#2s$O(B2$B$D$N8DBN$r(B1$B$D$N%i%s%@%`$J4p=`E@(B($B8r:5E@!'(Bcrossover_point)$B$GF~$lBX$($k0lE@8r:5$r;HMQ(B
 *
 * $B0lE@8r:5$NNc!J8r:5E@$O(B7$BHVL\!K(B
 * =======|=== -> =======|###
 * #######|### -> #######|===
 */
void crossover(void)
{
	int i;
	int crossover_point;
	int tmp;
	double r;

	r = (double)rand() / ((double)RAND_MAX + 1); // [0,1)$B$NMp?t(B

	// $B8r:5N($K4p$E$$$F8r:5$r<B9T(B
	if(r < CROSSOVER_RATE){
		// $B8r:5E@$r%i%s%@%`$K7hDj(B
		crossover_point = rand() % CHROM_LEN;

		// $B8r:5E@$h$j8e$m$rF~$lBX$($k(B
		for(i = crossover_point; i < CHROM_LEN; i++){
			tmp = Children[0].Chromsome[i];
			Children[0].Chromsome[i] = Children[1].Chromsome[i];
			Children[1].Chromsome[i] = tmp;
		}
	}
}

/*
 * 2$B$D$N;R8DBN$KBP$7$FFMA3JQ0[N(!J:#2s$O(B1/$B0dEA;RD9!K$K4p$E$$$FFMA3JQ0[$r<B9T(B
 * $BFMA3JQ0[$O0dEA;R$N%S%C%HCM$rH?E>(B(0->1, 1->0)
 */
void mutation(void)
{
	int i;
	double r;

	for(i = 0; i < CHROM_LEN; i++){
		// Children[0]$B$KBP$9$kFMA3JQ0[(B
		r = (double)rand() / ((double)RAND_MAX + 1); // [0,1)$B$NMp?t(B
		// $BFMA3JQ0[N($K4p$E$$$FFMA3JQ0[$r<B9T(B
		if(r < MUTATION_RATE){
			// $B%S%C%HCM$rH?E>(B
			if(Children[0].Chromsome[i] == 0){
				Children[0].Chromsome[i] = 1;
			}
			else{
				Children[0].Chromsome[i] = 0;
			}
		}

		// Children[0]$B$KBP$9$kFMA3JQ0[(B
		r = (double)rand() / ((double)RAND_MAX + 1); // [0,1)$B$NMp?t(B
		// $BFMA3JQ0[N($K4p$E$$$FFMA3JQ0[$r<B9T(B
		if(r < MUTATION_RATE){
			// $B%S%C%HCM$rH?E>(B
			if(Children[1].Chromsome[i] == 0){
				Children[1].Chromsome[i] = 1;
			}
			else{
				Children[1].Chromsome[i] = 0;
			}
		}
	}
}

/*
 * $B@8@.$7$?(B2$B8DBN$rJl=8CD$NCf$GE,9gEY$N0-$$(B2$B8DBN$rF~$lBX$((B
 * $B$3$N;~!"(BPopulation$BG[Ns$OE,9gEY$G%=!<%H$5$l$F$$$k$b$N$H$9$k(B
 */
void swap_population(void)
{
	Population[POP_SIZE - 1] = Children[0];
	calc_fitness(POP_SIZE - 1);
	Population[POP_SIZE - 2] = Children[1];
	calc_fitness(POP_SIZE - 2);
}

/*
 * GA$B$N<B9T%5%$%/%k(B
 *
 * $B<B9T=g$O(B
 * 1. $B=i4|8DBN@8@.(B(init_chrom)
 * 2. $BE,9gEY$G%=!<%F%#%s%0(B(sort_population)
 * 3. $B;R8DBN$r(B2$B$DA*Br(B(generate_children)
 * 4. $B8r:5(B(crossover)
 * 5. $BFMA3JQ0[(B(mutation)
 * 6. $B;R8DBN$rE,9gEY$N0-$$8DBN$HF~$lBX$((B(swap_population)
 * 7. 2~6$B$r7+$jJV$9(B
 */
void GA(void)
{
	int gen = 0;

	// 1. $B=i4|8DBN@8@.(B(init_chrom)
	init_chrom();

	while(gen < MAX_GENERATION)
	{
		// 2. $BE,9gEY$G%=!<%F%#%s%0(B(sort_population)
		sort_population();
		printf("%d gen\r\n", gen);
		print_chrom(0); // $B:GNI8DBN$rI=<((B
		// 3. $B;R8DBN$r(B2$B$DA*Br(B(generate_children)
		generate_children();
		// 4. $B8r:5(B(crossover)
		crossover();
		// 5. $BFMA3JQ0[(B(mutation)
		mutation();
		// 6. $B;R8DBN$rE,9gEY$N0-$$8DBN$HF~$lBX$((B(swap_population)
		swap_population();
		gen++;
	}
	sort_population();
	printf("%d gen\r\n", gen);
	print_chrom(0); // $B:G=*@$Be$N:GNI8DBN$rI=<((B
}

/**************************************************/
/*                                                */
/* GA$BK\BN$*$o$j(B                                   */
/*                                                */
/**************************************************/
 
/**************************************************/
/*                                                */
/* main                                           */
/*                                                */
/**************************************************/
int main(int argc, char **argv)
{
	srand((unsigned int)time(NULL)); // $BMp?t=i4|2=(B
	GA();

	return 0;
}
/**************************************************/
/*                                                */
/* main$B$*$o$j(B                                     */
/*                                                */
/**************************************************/

