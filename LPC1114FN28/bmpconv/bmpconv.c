#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define headerlen	62
#define inrow		64
#define incolumn	16
#define outcolumn	128
#define outrow		8

struct header{
	uint16_t dummy;			//for alignment
//BITMAPFILEHEADER
	uint16_t bfType;		//0x4D42
	uint32_t bfSize;
	uint16_t bfReserved1;
	uint16_t bfReserved2;
	uint32_t bfOffBits;
//BITMAPINFOHEADER
	uint32_t biSize;		//=0x28
	int32_t biWidth;
	int32_t biHeight;
	uint16_t biPlanes;
	uint16_t biBitCount;
	uint32_t biCompression;
	uint32_t biSizeImage;
	int32_t biXPelsPerMeter;
	int32_t biYPelsPerMeter;
	uint32_t biClrUsed;
	uint32_t biClrImportant;
};

uint8_t headerin[headerlen+2],datain[inrow][incolumn],dataout[outrow][outcolumn];

int checkHeader(){
	struct header *head;

	head=(struct header *)headerin;
	if (head->bfType!=0x4D42) return 1;
	if (head->bfSize!=0x043E) return 1;
	if (head->bfOffBits!=0x3E) return 1;
	if (head->biSize!=0x28) return 1;
	if (head->biWidth!=0x80) return 1;
	if (head->biHeight!=0x40) return 1;
	if (head->biPlanes!=0x01) return 1;
	if (head->biBitCount!=0x01) return 1;
	if (head->biCompression!=0x00) return 1;
	if (head->biSizeImage!=0x0400) return 1;
	return 0;
}

int main(){
	int i,j,k;

	if (fread(headerin+2,headerlen,1,stdin)<1){
		fprintf(stderr,"Header too small\n");
		return -1;
	}
	if (checkHeader()){
		fprintf(stderr,"Header mismatch\n");
		return -1;
	}
	if (fread(datain,incolumn,inrow,stdin)<inrow){
		fprintf(stderr,"Data too small\n");
		return -1;
	}
	for (i=0;i<inrow;++i){
		for (j=0;j<incolumn;++j){
			for (k=0;k<8;++k){
				if (datain[i^(inrow-1)][j]&(0x80>>k)){
					dataout[i>>3][(j<<3)|k]|=0x01<<(i&0x07);
				}else{
					dataout[i>>3][(j<<3)|k]&=~(0x01<<(i&0x07));
				}
			}
		}
	}
	for (i=0;i<outrow;++i){
		for (j=0;j<outcolumn;++j){
			if (!(j&0x07)){
				printf("\t");
			}
			printf("0x%02x,",dataout[i][j]);
			if (!((j^0x07)&0x07)){
				printf("\n");
			}
		}
	}
	return 0;
}
