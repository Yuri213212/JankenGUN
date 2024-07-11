const wchar_t imagePath[ST_NULL+1][32]={
	L"./res/Rock.bmp",
	L"./res/Scissors.bmp",
	L"./res/Paper.bmp",
	L"./res/Lose.bmp",
	L"./res/Draw.bmp",
	L"./res/Win.bmp",
	L"./res/Null.bmp",
};

HBITMAP image[ST_NULL+1];

wchar_t *loadImages(){
	HBITMAP hbitmap;
	int i,j;

	for (i=0;i<=ST_NULL;++i){
		hbitmap=LoadImageW(NULL,imagePath[i],IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
		if (!hbitmap){
			for (j=i;j;--j){
				DeleteObject(image[j]);
			}
			return (wchar_t *)imagePath[i];
		}
		image[i]=hbitmap;
	}
	return NULL;
}

void destroyImages(){
	int i;

	for (i=0;i<=ST_NULL;++i){
		DeleteObject(image[i]);
	}
}
