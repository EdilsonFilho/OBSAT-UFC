#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define L 5
#define C 8
#define N 40

int main()
{
    int i,j;
    int a=4;
    short matrizMensagem[L][C]; //Matriz da mensagem codificada recuperada do txt
    short mensagemRecuperada[N]; //Vetor de recuperação da mensagem para depois passar para matriz
    short sindromeColuna[12];
    short sindromeLinhas[5][3];
    short sindromeParLinhas[5];
    short numLinhasSindrome=0;
    short orSindromeLinhas[5];
    memset(orSindromeLinhas, 0, sizeof(orSindromeLinhas));

    FILE *pont_arq;
    pont_arq = fopen("../Codificador/mensagemCodificada.txt","rb");
    if(pont_arq==NULL){
        printf("Erro! Arquivo não foi aberto!\n");
        return -1;
    }
    fread(mensagemRecuperada,sizeof(short),N,pont_arq);
    for(i=0;i<L;i++){
        for(j=0;j<C;j++){
            matrizMensagem[i][j] = mensagemRecuperada[C*i+j];
        }
    }
    printf("Matriz codificada:\n");
    for(i=0;i<L;i++){
        for(j=0;j<C;j++){
            printf("%d",matrizMensagem[i][j]);
            printf("\t");
        }
        printf("\n");
    }
   matrizMensagem[0][1]=1;
   matrizMensagem[0][2]=0;
   matrizMensagem[0][0]=0;

   printf("Matriz com erro:\n");
    for(i=0;i<L;i++){
        for(j=0;j<C;j++){
            printf("%d",matrizMensagem[i][j]);
            printf("\t");
        }
        printf("\n");
    }


    for(i=0;i<2;i++){

        for(i=0;i<C;i++){ //Calcula a sindrome de paridade de coluna, para identificar em qual coluna houve o erro
            sindromeColuna[i]=matrizMensagem[0][i]^matrizMensagem[1][i]^matrizMensagem[2][i]^matrizMensagem[3][i]^matrizMensagem[4][i];
        }

        for(i=0;i<L;i++){ //Calcula a sindrome de paridade de linha, para indicar em qual linha houve erros
            sindromeLinhas[i][2]=matrizMensagem[i][0]^matrizMensagem[i][1]^matrizMensagem[i][3]^matrizMensagem[i][4];
            sindromeLinhas[i][1]=matrizMensagem[i][0]^matrizMensagem[i][2]^matrizMensagem[i][3]^matrizMensagem[i][5];
            sindromeLinhas[i][0]=matrizMensagem[i][1]^matrizMensagem[i][2]^matrizMensagem[i][3]^matrizMensagem[i][6];
            sindromeParLinhas[i]=matrizMensagem[i][0]^matrizMensagem[i][1]^matrizMensagem[i][2]^matrizMensagem[i][3]^matrizMensagem[i][4]^matrizMensagem[i][5]^matrizMensagem[i][6]^matrizMensagem[i][7];
        }

        for(i=0;i<L;i++){ //Faz um OR de todos os elementos de uma linha da matriz Sindrome de Linha, linha a linha
            orSindromeLinhas[i]=sindromeLinhas[i][0]||sindromeLinhas[i][1]||sindromeLinhas[i][2];
        }

        numLinhasSindrome=1*(orSindromeLinhas[0]||sindromeParLinhas[0])+1*(orSindromeLinhas[1]||sindromeParLinhas[1])+1*(orSindromeLinhas[2]||sindromeParLinhas[2])+1*(orSindromeLinhas[3]||sindromeParLinhas[3])+1*(orSindromeLinhas[4]||sindromeParLinhas[4]); //Calcula o numero de erros detectados pelas sindromes de paridade

        for(i=0;i<4;i++){
            if((numLinhasSindrome==1)&&(orSindromeLinhas[i])!=0){
                for(j=0;j<C;j++){
                    if(sindromeColuna[j]!=0){
                        matrizMensagem[i][j]=~matrizMensagem[i][j]+2;
                    }
                }
            } else if((sindromeParLinhas[i]!=0)&&(orSindromeLinhas[i]==0)){
                for(j=0;j<C;j++){
                    if(sindromeColuna[j]!=0){
                        matrizMensagem[i][j]=~matrizMensagem[i][j]+2;
                    }
                }
            } else if(orSindromeLinhas[i]!=0){
                if(sindromeParLinhas[i]==0){
                    for(j=0;j<C;j++){
                        if(sindromeColuna[j]!=0){
                            matrizMensagem[i][j]=~matrizMensagem[i][j]+2;
                        }
                    }
                } else if(sindromeParLinhas[i]==1){
                    if((sindromeLinhas[i][0]==0)&&(sindromeLinhas[i][1]==0)&&(sindromeLinhas[i][2]==1)){
                        matrizMensagem[i][6]=~matrizMensagem[i][6]+2;
                    }
                    if((sindromeLinhas[i][0]==0)&&(sindromeLinhas[i][1]==1)&&(sindromeLinhas[i][2]==0)){
                        matrizMensagem[i][5]=~matrizMensagem[i][5]+2;
                    }
                    if((sindromeLinhas[i][0]==1)&&(sindromeLinhas[i][1]==0)&&(sindromeLinhas[i][2]==0)){
                        matrizMensagem[i][4]=~matrizMensagem[i][4]+2;
                    }
                    if((sindromeLinhas[i][0]==1)&&(sindromeLinhas[i][1]==1)&&(sindromeLinhas[i][2]==1)){
                        matrizMensagem[i][3]=~matrizMensagem[i][3]+2;
                    }
                    if((sindromeLinhas[i][0]==1)&&(sindromeLinhas[i][1]==1)&&(sindromeLinhas[i][2]==0)){
                        matrizMensagem[i][2]=~matrizMensagem[i][2]+2;
                    }
                    if((sindromeLinhas[i][0]==1)&&(sindromeLinhas[i][1]==0)&&(sindromeLinhas[i][2]==1)){
                        matrizMensagem[i][1]=~matrizMensagem[i][1]+2;
                    }
                    if((sindromeLinhas[i][0]==0)&&(sindromeLinhas[i][1]==1)&&(sindromeLinhas[i][2]==1)){
                        matrizMensagem[i][0]=~matrizMensagem[i][0]+2;
                    }

                }
            }
        }
    }
    printf("Matriz corrigida:\n");
    for( i=0;i<L;i++){
        for(j=0;j<C;j++){
            printf("%d",matrizMensagem[i][j]);
            printf("\t");
        }
        printf("\n");
    }
    //mostrando mensagem decodificada
    printf("Mensagem decodificada: ");
    for( i=0;i<a;i++){
        for(j=0;j<a;j++){
            printf("%d",matrizMensagem[i][j]);
           // printf("\t");
        }
      //  printf("\n");
    }

    printf("\nSindrome paridade de coluna:\n");
    for(i=0;i<C;i++){
        printf("%d",sindromeColuna[i]);
    }
    printf("\nSindrome de linha:\n");
    for(i=0;i<L;i++){
        for(j=0;j<3;j++){
            printf("%d",sindromeLinhas[i][j]);
            printf("\t");
        }
        printf("\n");
    }
    printf("Sindrome paridade de linha:\n");
    for(i=0;i<L;i++){
        printf("%d",sindromeParLinhas[i]);
    }
    printf("\nNumero de erros em uma linha: ");
    printf("%d",numLinhasSindrome);

    /*
    for(int i=0;i<5;i++){
        printf("%d",orSindromeLinhas[i]);
    }
    printf("\n");*/
}
