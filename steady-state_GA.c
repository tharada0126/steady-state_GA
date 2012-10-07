/*
 * 2012/10/07
 * written by tharada
 *
 * steady-state Genetic Algorithm (steady-state GA)を用いたOne-Max問題の解法
 * One-Max問題(One-Max problem)
 * 遺伝子の要素で1の個数が多いほど評価値が高くなる問題
 * 最良解は遺伝子の全要素が1である個体
 */

#include<stdio.h>
#include<stdlib.h>
#include<time.h>

#define POP_SIZE 100					// 個体数
#define CHROM_LEN 20					// 遺伝子長
#define MAX_GENERATION 500				// 最大世代数
#define CROSSOVER_RATE 0.5				// 交叉率
#define MUTATION_RATE (1.0 / CHROM_LEN)	// 突然変異率

/*
 * GAの個体用の構造体
 * Chromsome:	遺伝子、0/1のビット列
 * Fitness:		適合度
 */
struct _Individual
{
	int Chromsome[CHROM_LEN];	// 遺伝子
	int Fitness;				// 適合度
};
typedef struct _Individual Individual;

Individual Population[POP_SIZE];	// 母集団
Individual Children[2];				// 子個体生成用

/**************************************************/
/*                                                */
/* 表示関数                                       */
/*                                                */
/**************************************************/

/*
 * Population配列内のpop_i番目の個体を下記の形式で表示
 * xxxxxxxxxx: fitness=yyy
 *
 * <argument(s)>
 * pop_i: 表示する個体のインデックス
 */
void print_chrom(int pop_i)
{
	int i;

	// 遺伝子の表示
	for(i = 0; i < CHROM_LEN; i++){
		printf("%d", Population[pop_i].Chromsome[i]);
	}

	// 適合度の表示
	printf(": fitness=%d\n", Population[pop_i].Fitness);
}

/*
 * 母集団内の全個体を表示
 * 表示形式はprint_chrom関数に依存
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
/* 表示関数おわり                                 */
/*                                                */
/**************************************************/

/**************************************************/
/*                                                */
/* GA本体                                         */
/*                                                */
/**************************************************/

/*
 * Population配列内のpop_i番目の個体の適合度を計算
 * 今回の問題は遺伝子内の1の個数が多いほど良い解とするので
 * 1の個数をそのまま適合度にする
 *
 * <argument(s)>
 * pop_i: 適合度を計算する個体のインデックス
 */
void calc_fitness(int pop_i)
{
	int sum = 0;
	int i;

	// 遺伝子内の1の個数を算出
	for(i = 0; i < CHROM_LEN; i++){
		if(Population[pop_i].Chromsome[i] == 1){
			sum++;
		}
	}

	// 1の個数がそのまま個体の適合度
	Population[pop_i].Fitness = sum;
}

/*
 * 母集団内の全個体の適合度を計算
 */
void calc_fit_all(void)
{
	int i;

	for(i = 0; i < POP_SIZE; i++){
		calc_fitness(i);
	}
}

/*
 * 母集団を適合度でソーティング
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
 * 母集団をランダムに初期化
 */
void init_chrom(void)
{
	int i, j;

	for(i = 0; i < POP_SIZE; i++){
		for(j = 0; j < CHROM_LEN; j++){
			// 遺伝子の各要素を0/1でランダムに初期化
			Population[i].Chromsome[j] = rand() % 2;
		}
		calc_fitness(i);
	}
}

/*
 * 母集団の中から親となる個体を選択
 * 今回はトーナメント選択を使用
 *
 * トーナメント選択(tournament selection)
 * 母集団の中からランダムにn個体選択し、その中で最も評価の高い個体を選択
 * nをトーナメントサイズと呼ぶ
 * 今回はトーナメントサイズはn=2
 *
 * <return>
 * 選択した個体のインデックス
 */
int select_chrom(void)
{
	int i1, i2;
	int ret;
	
	// ランダムに1個体選択
	i1 = rand() % POP_SIZE;
	// もう1個体を被らないように選択
	do{
		i2 = rand() % POP_SIZE;
	} while(i1 == i2);

	// 適合度を比較し、評価の高い個体を戻り値として選択
	if(Population[i1].Fitness >= Population[i2].Fitness){
		ret = i1;
	}
	else{
		ret = i2;
	}

	return ret;
}

/*
 * 母集団の中から2個体を選択しChildren配列に保存
 */
void generate_children(void)
{
	int c1, c2;

	// トーナメント選択
	c1 = select_chrom();
	c2 = select_chrom();
	
	// 選択した個体をChildren配列に保存
	Children[0] = Population[c1];
	Children[1] = Population[c2];
}

/*
 * 2つの子個体に対して交叉率(CROSSOVER_RATE)にもとづいて交叉を実行
 * 今回は2つの個体を1つのランダムな基準点(交叉点：crossover_point)で入れ替える一点交叉を使用
 *
 * 一点交叉の例（交叉点は7番目）
 * =======|=== -> =======|###
 * #######|### -> #######|===
 */
void crossover(void)
{
	int i;
	int crossover_point;
	int tmp;
	double r;

	r = (double)rand() / ((double)RAND_MAX + 1); // [0,1)の乱数

	// 交叉率に基づいて交叉を実行
	if(r < CROSSOVER_RATE){
		// 交叉点をランダムに決定
		crossover_point = rand() % CHROM_LEN;

		// 交叉点より後ろを入れ替える
		for(i = crossover_point; i < CHROM_LEN; i++){
			tmp = Children[0].Chromsome[i];
			Children[0].Chromsome[i] = Children[1].Chromsome[i];
			Children[1].Chromsome[i] = tmp;
		}
	}
}

/*
 * 2つの子個体に対して突然変異率（今回は1/遺伝子長）に基づいて突然変異を実行
 * 突然変異は遺伝子のビット値を反転(0->1, 1->0)
 */
void mutation(void)
{
	int i;
	double r;

	for(i = 0; i < CHROM_LEN; i++){
		// Children[0]に対する突然変異
		r = (double)rand() / ((double)RAND_MAX + 1); // [0,1)の乱数
		// 突然変異率に基づいて突然変異を実行
		if(r < MUTATION_RATE){
			// ビット値を反転
			if(Children[0].Chromsome[i] == 0){
				Children[0].Chromsome[i] = 1;
			}
			else{
				Children[0].Chromsome[i] = 0;
			}
		}

		// Children[0]に対する突然変異
		r = (double)rand() / ((double)RAND_MAX + 1); // [0,1)の乱数
		// 突然変異率に基づいて突然変異を実行
		if(r < MUTATION_RATE){
			// ビット値を反転
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
 * 生成した2個体を母集団の中で適合度の悪い2個体を入れ替え
 * この時、Population配列は適合度でソートされているものとする
 */
void swap_population(void)
{
	Population[POP_SIZE - 1] = Children[0];
	calc_fitness(POP_SIZE - 1);
	Population[POP_SIZE - 2] = Children[1];
	calc_fitness(POP_SIZE - 2);
}

/*
 * GAの実行サイクル
 *
 * 実行順は
 * 1. 初期個体生成(init_chrom)
 * 2. 適合度でソーティング(sort_population)
 * 3. 子個体を2つ選択(generate_children)
 * 4. 交叉(crossover)
 * 5. 突然変異(mutation)
 * 6. 子個体を適合度の悪い個体と入れ替え(swap_population)
 * 7. 2~6を繰り返す
 */
void GA(void)
{
	int gen = 0;

	// 1. 初期個体生成(init_chrom)
	init_chrom();

	while(gen < MAX_GENERATION)
	{
		// 2. 適合度でソーティング(sort_population)
		sort_population();
		printf("%d gen\r\n", gen);
		print_chrom(0); // 最良個体を表示
		// 3. 子個体を2つ選択(generate_children)
		generate_children();
		// 4. 交叉(crossover)
		crossover();
		// 5. 突然変異(mutation)
		mutation();
		// 6. 子個体を適合度の悪い個体と入れ替え(swap_population)
		swap_population();
		gen++;
	}
	sort_population();
	printf("%d gen\r\n", gen);
	print_chrom(0); // 最終世代の最良個体を表示
}

/**************************************************/
/*                                                */
/* GA本体おわり                                   */
/*                                                */
/**************************************************/
 
/**************************************************/
/*                                                */
/* main                                           */
/*                                                */
/**************************************************/
int main(int argc, char **argv)
{
	srand((unsigned int)time(NULL)); // 乱数初期化
	GA();

	return 0;
}
/**************************************************/
/*                                                */
/* mainおわり                                     */
/*                                                */
/**************************************************/

