#include <stdio.h>
#include <stdlib.h>

#define N 40
#define L 5
#define C 8

int main()
{
    int i,j;
    short matrizCodificacao[L][C];
    short xorBits=0;
    short mensagemCodificada[N];

    FILE *pont_arq;
    pont_arq = fopen("mensagemBits.txt","r");
    if(pont_arq==NULL){
        printf("Erro! Arquivo não foi aberto!\n");
        return -1;
    }

    for(i=0;i<N;i++){ //Zera o vetor
        mensagemCodificada[i]=0;
    }

    for(i=0;i<L;i++){ // Zera a matriz de codificação
        for(j=0;j<C;j++){
            matrizCodificacao[i][j]=0;
        }
    }

    for( i=0;i<4;i++){ // Preenche a matriz com os bits do arquivo txt nas posições corretas para codificaçãp
        for(j=0;j<4;j++){
            matrizCodificacao[i][j]=getc(pont_arq)-48;
        }
    }

    pont_arq = fopen("mensagemCodificada.txt","wb");
    if(pont_arq==NULL){
        printf("Erro! Arquivo não foi aberto!\n");
        return -1;
    }

    for(i=0;i<4;i++){ // Calcula os checkbits da primeira coluna
        int j=0;
        xorBits = matrizCodificacao[i][1];
        for(j=2;j<4;j++){
            xorBits = xorBits^matrizCodificacao[i][j];
        }
        matrizCodificacao[i][j] = xorBits;
    }
    for(i=0;i<4;i++){ // Calcula os checkbits da segunda coluna
        j=0;
        xorBits = matrizCodificacao[i][0];
        for(j=2;j<4;j++){
            xorBits = xorBits^matrizCodificacao[i][j];
        }
        matrizCodificacao[i][j+1] = xorBits;
    }
    for(i=0;i<4;i++){ // Calcula os checkbits da terceira coluna
        j=0;
        xorBits = matrizCodificacao[i][3];
        for(j=0;j<2;j++){
            xorBits = xorBits^matrizCodificacao[i][j];
        }
        matrizCodificacao[i][j+4] = xorBits;
    }
    for(i=0;i<4;i++){ // Calcula os bits de paridade de palavra
        j=0;
        xorBits = matrizCodificacao[i][j];
        for(j=1;j<7;j++){
            xorBits = xorBits^matrizCodificacao[i][j];
        }
        matrizCodificacao[i][j] = xorBits;
    }
    for(j=0;j<8;j++){ // Calcula os bits de paridade
        i=0;
        xorBits = matrizCodificacao[i][j];
        for(i=1;i<4;i++){
            xorBits = xorBits^matrizCodificacao[i][j];
        }
        matrizCodificacao[i][j] = xorBits;
    }

    for(i=0;i<L;i++){
        for(j=0;j<C;j++){
            mensagemCodificada[8*i+j ]=matrizCodificacao[i][j];
        }
    }

    fwrite(mensagemCodificada,sizeof(short),N,pont_arq);

    for(i=0;i<L;i++){
        for(j=0;j<C;j++){
            printf("%d",matrizCodificacao[i][j]);
            printf("\t");
        }
        printf("\n");
    }
    for(i=0;i<40;i++){
        printf("%d",mensagemCodificada[i]);
    }

    return 0;
}
