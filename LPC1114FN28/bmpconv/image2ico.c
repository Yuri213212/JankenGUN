#include <stdio.h>
#include <stdint.h>
#include <string.h>

const uint8_t Rock[1024]={
#include "Rock.txt"
};

const uint8_t Scissors[1024]={
#include "Scissors.txt"
};

const uint8_t Paper[1024]={
#include "Paper.txt"
};

const uint8_t Lose[1024]={
#include "Lose.txt"
};

const uint8_t Draw[1024]={
#include "Draw.txt"
};

const uint8_t Win[1024]={
#include "Win.txt"
};

uint8_t data[128],font[256][8]={
	{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,},
	{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,},
};
int used=2;

int addpic(const uint8_t *pic){
	int i,j;

	for (i=0;i<128;++i){
		for (j=0;j<used;++j){
			if (!memcmp(pic+i*8,font[j],8)){
				data[i]=j;
				goto next;
			}
		}
		if (used>0xFF){
			fprintf(stderr,"\nOut of range\n");
			return 1;
		}
		memcpy(font[used],pic+i*8,8);
		data[i]=used;
		++used;
next:
		;
	}
	for (i=0;i<8;++i){
		printf("\n\t\t");
		for (j=0;j<16;++j){
			printf("0x%02x,",data[i*16+j]);
		}
	}
	printf("\n");
	return 0;
}

int main(){
	int i,j;

	printf("const uint8_t picture[6][8*16]={\n\t{//Rock");
	if (addpic(Rock)) return -1;
	printf("\t},{//Scissors");
	if (addpic(Scissors)) return -1;
	printf("\t},{//Paper");
	if (addpic(Paper)) return -1;
	printf("\t},{//Lose");
	if (addpic(Lose)) return -1;
	printf("\t},{//Draw");
	if (addpic(Draw)) return -1;
	printf("\t},{//Win");
	if (addpic(Win)) return -1;
	printf("\t},\n};\n");
	printf("\n//font:%d/256\n",used);
	printf("const uint8_t oled_ico8[256*8]={");
	for (i=0;i<used;++i){
		printf("\n\t");
		for (j=0;j<8;++j){
			printf("0x%02x,",font[i][j]);
		}
	}
	printf("\n};\n");
	return 0;
}
