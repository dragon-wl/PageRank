#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include"pagerank.h"
int ell_column_indices[MAX][ELL_COLUMN];
float ell_values[MAX][ELL_COLUMN];

int coo_row[COO_LENGTH]={0};
int coo_column[COO_LENGTH] ={0};
float coo_values[COO_LENGTH] = {0};
float pagerank[MAX] ={1};

char *URLs[300000];

int LinkOutNum[MAX]= {0};
int LinkInNum[MAX] ={0};

int COONUM = 0;

int top10ID[TOPNUM] = {0};

int main(int argc, char* argv[])
{

    FILE *url, *top10;
    if((url = fopen(argv[1], "r")) == NULL)
    {
    	printf("file url.txt read failed!\n");
    	return -1;
	}
	if((top10 = fopen(argv[2], "w")) == NULL)
    {
    	printf("file top10.txt write failed!\n");
    	return -1;
	}
	char row[100], strSourceID[10], strDestID[10];
	int space, end, sourceID, destID;
	int flag = 0, n = 0;
/*	while(!feof(url))
    {
        fgets(row, 100, url);
        if(*row != 'n' && *row != '\n')
        {
            memset(strSourceID, 0, 10);
            memset(strDestID, 0, 10);
            for (space = 0; row[space] != ' ' ; ++space);
                strncpy(strSourceID, row, space);
            sourceID = atoi(strSourceID);   //1 to MAX
            for (end = space; row[end] != '\n' ; ++end);
                strncpy(strDestID, row+space+1, end-space-1);
            destID = atoi(strDestID);  //1 to MAX
            LinkOutNum[sourceID-1]++;
            LinkInNum[destID-1]++;
            getNumforCoo(sourceID, destID);
        }
    }
*/
	while(!feof(url))
    {
        fgets(row, 100, url);
        if(*row == 'n')
        {
            n++;
            for (space = 0; row[space] != ' ' ; ++space);
            if ((URLs[n] = malloc(space)) == NULL)
                    return -1;
            memset(URLs[n], 0, space);
            strncpy(URLs[n], row, space);
            continue;
        }
        if(*row != 'n' && row != '\n')
        {
            memset(strSourceID, 0, 10);
            memset(strDestID, 0, 10);
            for (space = 0; row[space] != ' ' ; ++space);
            strncpy(strSourceID, row, space);
            sourceID = atoi(strSourceID);   //1 to MAX
            for (end = space; row[end] != '\n' ; ++end);
            strncpy(strDestID, row+space+1, end-space-1);
            destID = atoi(strDestID);  //1 to MAX
            LinkOutNum[sourceID-1]++;
            LinkInNum[destID-1]++;
            getNumforCoo(sourceID, destID);
        }

    }

    getValueforCoo();
    printf("Now start cal Pagerank\n");
    CalPageRank();
    printf("Now start print TOP URL\n");
    findTop10();
    for(int i =0;i<300000;i++)
    {
        free(URLs[i]);
    }
    fclose(url);
    fclose(top10);

}

void getNumforCoo(int source, int dest)
{
    static int coo_num = 0;
    if(coo_num == COO_LENGTH)
    {
        printf("ERROR : the coo has been full \n");
    }
    else
    {
        coo_column[coo_num] = source;
        coo_row[coo_num] = dest;
        coo_num++;
        COONUM++;
       // printf("coo length: %d \n",coo_num);
        //printf("linknum = %d  |  value: %f \n",linkNum,value);
    }
}

void getValueforCoo()
{
    for(int i=0; i<COONUM;i++)
    {
        int linkNum = LinkOutNum[coo_column[i]-1];
        float value = 0.85 * 1.0 / linkNum;  //other value = 0.15/MAx
        coo_values[i] = value;
        printf("source: %d | dest: %d | linkOutNum: %d | value:%.16f\n", coo_column[i],coo_row[i],LinkOutNum[coo_column[i]-1],value);

    }
}

void generate_Gm(int source, int dest, int linkNum)
{
    static int ell_state[MAX] ={0}; // if ell_state[i] >=ell_column  the row i is full and cant put anything in
    static int coo_state = 0;
    float value = 0;
    value = 0.85 * 1.0 / linkNum;  //other value = 0.15/MAx
    if(ell_state[dest]<ELL_COLUMN)
    {
        ell_column_indices[dest][ell_state[dest]] = source;
        ell_values[dest][ell_state[dest]] = value;
        ell_state[dest] ++;
        //printf("linknum = %d  |  value: %f \n",linkNum,value);
    }
    else
    {
        if(coo_state == COO_LENGTH)
        {
              printf("ERROR : the coo has been full \n");
        }
        else
        {
            coo_column[coo_state] = source;
            coo_row[coo_state] = dest;
            coo_values[coo_state] = value;
            coo_state++;
           // printf("coo length: %d \n",coo_state);
            //printf("linknum = %d  |  value: %f \n",linkNum,value);
        }
    }
}

// Sequential Cal when only using coo
void CalPageRank()
{
    printf("  **  ---  **   \n");
    if(ELL_COLUMN != 0)
    {
        printf("ERROR: When ELL_COLUMN !=0, Can not use this function!");
        return;
    }

    float lastrank[MAX] ={0};

    float limitation = 0.00001;
    double distance = 1;

    float comf = 0.15/MAX;
    float comp = 0;

    while(distance>limitation * limitation)
    {
        comp = 0;
        for(int i =0; i<MAX; i++)
        {
            lastrank[i] = pagerank[i];
            pagerank[i] = 0;
            comp = comp + lastrank[i];
        }
        for(int i =0; i<MAX; i++)
        {
            pagerank[i] = comp * comf;
        }
        for(int i =0; i<COO_LENGTH;i++)
        {
            pagerank[ coo_row[i] ] = coo_values[i] * lastrank[ coo_column[i] ] + pagerank[ coo_row[i] ];
            pagerank[ coo_row[i] ] = pagerank[ coo_row[i] ] - comf*lastrank[ coo_column[i] ];
        }
        distance = 0;
        for(int i =0; i<MAX; i++)
        {
            distance = (lastrank[i] - pagerank[i]) * (lastrank[i] - pagerank[i]) +distance;
            //printf("distance = %.16lf\n",distance);
        }
    }
    if(distance < limitation * limitation)
    {
        printf("Deal Finished!\n");
    }

}




//ergodic by row when ell + coo
void ErgodicCalPageRank()
{
    float lastrank[MAX] ={0};

    float limitation = 0.00001;
    float distance = 0;
    int ellCalp =0;
    int cooCalp =0;
    float metrix_j =0;  //A[i][j]

    while(distance>limitation)
    {
        for(int i=0; i<MAX; i++)
        {
            ellCalp =0;
            cooCalp =0;
            lastrank[i] = pagerank[i];
            for(int j =0; j<MAX; j++)
            {
                if(j<ell_column_indices[i][ELL_COLUMN-1])  //in ell
                {
                    if(ell_column_indices[i][ellCalp] == j)
                    {
                        metrix_j = ell_values[i][ellCalp];
                        ellCalp++;
                    }
                    else
                        metrix_j = 0.15/MAX;
                }
                else //in coo
                {
                    // ergodic by row
                }
                pagerank[i] = pagerank[j]*metrix_j + pagerank[i];
            }

        }
        for(int i= 0; i<MAX; i++ )
            distance = (lastrank[i] - pagerank[i]) * (lastrank[i] - pagerank[i]) +distance;

    }
}



void findTop10()
{
    printf("\n**TOP 10**\n");
    float top10Pagerank[TOPNUM] ={0};
    int i =0;
    int j =0;
    for(i =0; i<MAX; i++)
    {
        for(j=0;j<TOPNUM;j++)
        {
            if(pagerank[i]>top10Pagerank[j])
            {
                for(int q=TOPNUM-1;q>j;q--)
                {
                    top10Pagerank[q]= top10Pagerank[q-1];
                    top10ID[q] = top10ID[q-1];
                }
                top10Pagerank[j] = pagerank[i];
                top10ID[j]=i;
                break;
            }
        }
    }
    printf("The top 10 URL is:\n");
    for(int i =0; i<TOPNUM;i++)
    {
        printf("**%d: %d | %.16lf\n",i,top10ID[i],top10Pagerank[i]);
        printf("  URL:%s\n",URLs[ top10ID[i] ]);

    }
}
