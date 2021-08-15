#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void lcs_length(char * X, char * Y, int m, int n, int len[m+1][n+1]);
char * lcs_string(char * X, char * Y, int m, int n);

int main(int argc, char ** argv)
{
    if(argc < 2){
        printf("Wrong usage: pass the path to the input file!\n");
        return 1;
    }

    char * path = argv[1];

    FILE * file = fopen(path, "rb");
    if(file == NULL){
        printf("Make sure the input file exists!\n");
        return 1;
    }
    fseek(file, 0, SEEK_END);
    long fsize = ftell(file);
    fseek(file, 0, SEEK_SET);

    char * strings = malloc(fsize + 1);
    fread(strings, 1, fsize, file);
    fclose(file);
    strings[fsize] = 0;

    char * newline = strchr(strings, '\n');
    if(newline == NULL){
        printf("Make sure the input file consists of two lines!\n");
        return 1;
    }

    newline[0] = '\0';
    char * X = strings;
    char * Y = newline + 1;

    int size1 = strlen(X);
    int size2 = strlen(Y);

    char * lcs = lcs_string(X, Y, size1, size2);
    printf("Sequential output: %s\n", lcs);


    //save to the specified file
    if(argc == 3){
        path = argv[2];
        FILE * ofile = fopen(path, "w");
        fputs(lcs, ofile);
        fclose(ofile);
    }

    free(lcs);
    free(strings);
    return 0;
}

/*
 * Computes the longest common sequence for all prefixes of X and Y
 * using a dynmic programming approach
 * @param X: a pointer to an array of char's
 * @param Y: a pointer to an array of char's
 * @param m: size of array X
 * @param n: size of array Y
 * @param lcs_prefix: 2d array holding the results of the computation
 *
*/
void lcs_length(char * X, char * Y, int m, int n, int lcs_prefix[m+1][n+1]){

    for(int i = 1; i <= m; i++)
        lcs_prefix[i][0] = 0;
    for(int j = 0; j <= n; j++)
        lcs_prefix[0][j] = 0;

    for(int i = 1; i <= m; i++){
        for(int j = 1; j <= n; j++){
            if(X[i-1] == Y[j-1])
                lcs_prefix[i][j] = lcs_prefix[i-1][j-1] + 1;
            else if(lcs_prefix[i-1][j] >= lcs_prefix[i][j-1])
                lcs_prefix[i][j] = lcs_prefix[i-1][j];
            else
                lcs_prefix[i][j] = lcs_prefix[i][j-1];
        }
    }
}

/*
 * Computes the longest common sequence for two sequences X and Y
 * @param X: a pointer to an array of char's
 * @param Y: a pointer to an array of char's
 * @param m: size of array X
 * @param n: size of array Y
 * @return: the LCS of X and Y
 *
*/
char * lcs_string(char * X, char * Y, int m, int n){

   int lcs_prefix[m+1][n+1];
   lcs_length(X, Y, m, n, lcs_prefix);
   int lcs_len = lcs_prefix[m][n];
   char * lcs = malloc(sizeof(char) * (lcs_len+1));
   lcs[lcs_len] = '\0';
   int i = m, j = n;
   while(i > 0 && j > 0){
       if(X[i-1] == Y[j-1]){
           lcs[--lcs_len] = X[i-1];
           i--;
           j--;
       }
       else if(lcs_prefix[i-1][j] >= lcs_prefix[i][j-1])
           i--;
       else
           j--;
   }
   return lcs;
}


