#include<stdio.h>


main(int argc, char * argv[]){
    FILE * fp;
    char c;
    int n = 0;

    if((fp = fopen("file.txt", "r"))!= NULL){
        while((c = fgetc(fp)) != EOF){
            if( c == 'a' || c == 'A')
                n++;
        }
        fclose(fp);
        print("existem %d letras A no arquivo", n);
    }
    return 0;
}

