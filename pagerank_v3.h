

//file deal
#define MAX 400000
#define URLIDLEN 6
#define ELL_COLUMN 0
#define COO_LENGTH 2000000
#define TOPNUM 10

extern int ell_column_indices[MAX][ELL_COLUMN];
extern float ell_values[MAX][ELL_COLUMN];

extern int coo_row[COO_LENGTH];
extern int coo_column[COO_LENGTH];
extern float coo_values[COO_LENGTH];

extern float pagerank[MAX];

//page rank deal
void generate_Gm(int source, int dest, int linkNum);
void CalPageRank();
void findTop10();
void getNumforCoo(int source, int dest);
void getValueforCoo();
