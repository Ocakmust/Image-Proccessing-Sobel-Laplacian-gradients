#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <math.h>
#include <time.h>
#pragma pack(1)
typedef unsigned char BYTE;//1 bytes of memory
typedef unsigned short WORD;//2 bytes of memory
typedef unsigned int DWORD;//4 bytes of memory
typedef int LONG;//4 bytes of memory
typedef struct _BMPFH//takes 14 bytes of memory
{
	BYTE bftype1;//1 byte and must be 'B'
	BYTE bftype2;//1 byte and must be 'M'
	DWORD bfsize;//4 bytes gives us the all size of bmp file (including headers,palette (if it has) data)
	WORD bfreserved1;// 2 btyes of memory could be set as 0
	WORD bfreserved2;// 2 btyes of memory could be set as 0
	DWORD bfOffbits;//4 bytes of memeory gives the position of data starts in the bmp file
} BMPFH;
typedef struct _BMPIH//40 bytes for windows bitmap file
{
	DWORD bisize;//4 bytes and it gives the size of info header
	LONG  biw;//4 bytes and it is the width of image
	LONG bih;//4 bytes and it is the height of iimage
	WORD biplane;//2 bytes which is not important for us
	WORD bibitcount;//2 bytes it is about the type of bitmap file if it is 1 2 color, 4 (16 colors) ..
	DWORD biComp;//4 bytes not important
	DWORD bisizeimage;//4 bytes and it gives the size of data in the image 
	LONG bix;//4 bytes not importatnt
	LONG biy;//4 bytes not important
	DWORD biclused;//4 bytes not important
 	DWORD biclimp;//4 byets not importatnt for us
}  BMPIH;
typedef struct _PALET//in palette it describes colors (what is the color number)
{
	BYTE rgbblue;//blue commponent
	BYTE rgbgreen;//green component
	BYTE rgbred;//red component
	BYTE rgbreserved;//reserved byte the user can use this for therir aims
} PALET;
typedef struct _IMAGE
{
	BMPFH   bmpfh;
	BMPIH   bmpih;
	PALET   *palet;
	BYTE    *data;
}  IMAGE;

IMAGE *ImageRead(IMAGE *image,char *filename)
{
	BMPFH bmpfh;
	BMPIH bmpih;
	FILE *fp;
	DWORD r,rowsize,size;
	fp=fopen(filename,"rb");//tries to open the filename
	if(fp==NULL) {printf("File is not found..");exit(1);}
	fread(&bmpfh,sizeof(BMPFH),1,fp);//reads bitmap file header from the file and set to bmph
	if(bmpfh.bftype1!='B' || bmpfh.bftype2!='M')
	{
		printf("This is not a bitmap file.");
		exit(1);
	}
    fread(&bmpih,sizeof(BMPIH),1,fp);//reads bitmap info header from the file and set to bmpih
	image=(IMAGE *) malloc(bmpfh.bfsize);//allocates memory for image
	if(image==NULL) {printf("There is no enough memory for this operation");exit(1);}
	image->bmpfh=bmpfh;//sets bmpfh to image 
	image->bmpih=bmpih;//sets bmpih to image
	
	r=0;//r is set to 0 in case 24 bits per pixel or more (this kind of images does not have color palette)
    if(bmpih.bibitcount==1) r=2;//if the image 1 bit per pixel (the number of clor is 2)
	if(bmpih.bibitcount==4) r=16;//if the image 4 bits per pixel (the number of clor is 16)
	if(bmpih.bibitcount==8) r=256;////if the image 8 bits per pixel (the number of clor is 256)
	if(r!=0)
	{
		image->palet=(PALET *) malloc(4*r);//allocate memory for color palette
		fread(image->palet,sizeof(BYTE),4*r,fp);//read color palette from image to the memory
	}
	rowsize=(image->bmpih.biw*image->bmpih.bibitcount+31)/32*4;//calculates 1 row of image size 
	size=rowsize*image->bmpih.bih;//all size of image is calculated and set to size 
	image->data=(BYTE *)malloc(size);//allocates enough memory for iamage data
	fread(image->data,size,1,fp);//reads image data from file and sets to image->data
	fclose(fp); //closes the file
	return image;//returns the adress of image on the memory
}       
void ImageWrite(IMAGE *image,char *filename)
{
	FILE *fp;
	int r,rowsize,size;
	
	fp=fopen(filename,"wb");//opens the file );exit(1);}
	if(fp==NULL) {printf("Fie opening error!");exit(1);}
	image->bmpfh.bftype2='M';
	fwrite(&image->bmpfh,sizeof(BMPFH),1,fp);//writes the bitmap file header to the file
	fwrite(&image->bmpih,sizeof(BMPIH),1,fp);//writes he bitmep info header to the file		
	r=0;
    if(image->bmpih.bibitcount==1) r=2;//if the image has 2 colors
	if(image->bmpih.bibitcount==4) r=16;//if the image has 16 colors
	if(image->bmpih.bibitcount==8) r=256;//if the image has 256 colors
	if(r!=0) fwrite(image->palet,4*r,1,fp);//if the image has color palette the palette is written to the file
	rowsize=(image->bmpih.biw*image->bmpih.bibitcount+31)/32*4;//a row size of image is calculated 
	size=rowsize*image->bmpih.bih;//all size of image is calculated
	fwrite(image->data,size,1,fp);//write image data to the file
	fclose(fp);//closes the file 
}
void writeInfo(IMAGE *image,char *fname)
{
	printf("--------Info about %s  image file\n",fname);
	printf("bfType value		:%c%c\n",image->bmpfh.bftype1,image->bmpfh.bftype2);
	printf("bfsize			:%d\n",image->bmpfh.bfsize);
	printf("bfreserved1		:%d\n",image->bmpfh.bfreserved1);
	printf("bfreserved2		:%d\n",image->bmpfh.bfreserved2);
	printf("bfOffbits		:%d\n",image->bmpfh.bfOffbits);
	printf("bisize			:%d\n",image->bmpih.bisize);
	printf("Width			:%d\n",image->bmpih.biw);
	printf("Height			:%d\n",image->bmpih.bih);
	printf("biplane			:%d\n",image->bmpih.biplane);
	printf("bibitcount		:%d\n",image->bmpih.bibitcount);
	printf("Compression		:%d\n",image->bmpih.biComp);
	printf("bisizeimage		:%d\n",image->bmpih.bisizeimage);
	printf("bix			:%d\n",image->bmpih.bix);
	printf("biy			:%d\n",image->bmpih.biy);
	printf("bi color used		:%d\n",image->bmpih.biclused);
	printf("bi color imp.		:%d\n",image->bmpih.biclimp);
}


void medianFilter(IMAGE *image, int filterSize) {
    DWORD w, h, rowsize;
    int i, j, k, n, m, l;
    BYTE *temp;
    BYTE *result;
    w = image->bmpih.biw;
    h = image->bmpih.bih;
    rowsize = (image->bmpih.bibitcount * w + 31) / 32 * 4;
    temp = (BYTE *)malloc(h * rowsize * sizeof(BYTE));
    result = (BYTE *)malloc(h * rowsize * sizeof(BYTE));

    if (temp == NULL || result == NULL) exit(1);
    memcpy(temp, image->data, h * rowsize);

    m = filterSize / 2;
    for (i = 0; i < h; i++) {
        for (j = 0; j < w; j++) {
            BYTE window[filterSize * filterSize];
            l = 0;
            for (k = i - m; k <= i + m; k++) {
                for (n = j - m; n <= j + m; n++) {
                    if (k >= 0 && k < h && n >= 0 && n < w) {
                        window[l++] = temp[k * rowsize + n];
                    }
                }
            }
            // Medyanı hesapla
            int windowSize = l;
            for (int p = 0; p < windowSize - 1; p++) {
                for (int q = p + 1; q < windowSize; q++) {
                    if (window[p] > window[q]) {
                        BYTE tmp = window[p];
                        window[p] = window[q];
                        window[q] = tmp;
                    }
                }
            }
            // Orta elemanı al
            result[i * rowsize + j] = window[windowSize / 2];
        }
    }
    memcpy(image->data, result, h * rowsize);
    free(temp);
    free(result);
    return;
}
void enhanceContrast(IMAGE *image, double factor) {
    DWORD w, h, rowsize;
    int i, j;
    BYTE *temp;
    w = image->bmpih.biw;
    h = image->bmpih.bih;
    rowsize = (image->bmpih.bibitcount * w + 31) / 32 * 4;
    temp = (BYTE *)malloc(h * rowsize * sizeof(BYTE));

    if (temp == NULL) exit(1);
    memcpy(temp, image->data, h * rowsize);

    for (i = 0; i < h; i++) {
        for (j = 0; j < w; j++) {
            // Kontrastı artırma işlemi
            double pixel = (double)temp[i * rowsize + j];
            pixel = (pixel - 127.0) * factor + 127.0;
            if (pixel < 0) pixel = 0;
            if (pixel > 255) pixel = 255;
            // Sonucu ana görüntü veri alanına yerleştir
            image->data[i * rowsize + j] = (BYTE)pixel;
        }
    }
    free(temp);
    return;
}

void sobel(IMAGE *image) {
    DWORD w, h, rowsize;
    int i, j;
    double gx, gy;
    BYTE *temp;
    w = image->bmpih.biw;
    h = image->bmpih.bih;
    rowsize = (image->bmpih.bibitcount * w + 31) / 32 * 4;
    temp = (BYTE *)calloc(h * rowsize, sizeof(char));

    if (temp == NULL) exit(1);
    memcpy(temp, image->data, h * rowsize);

    for (i = 1; i < h - 1; i++) {
        for (j = 1; j < w - 1; j++) {
            // Kenar pikselleri hesaplamayın
            if (i == 0 || i == h - 1 || j == 0 || j == w - 1) continue;

            // Sobel operatörü ile gradyan hesaplaması
            gx = (temp[(i - 1) * rowsize + j + 1] + 2 * temp[i * rowsize + j + 1] + temp[(i + 1) * rowsize + j + 1]) -
                 (temp[(i - 1) * rowsize + j - 1] + 2 * temp[i * rowsize + j - 1] + temp[(i + 1) * rowsize + j - 1]);

            gy = (temp[(i + 1) * rowsize + j - 1] + 2 * temp[(i + 1) * rowsize + j] + temp[(i + 1) * rowsize + j + 1]) -
                 (temp[(i - 1) * rowsize + j - 1] + 2 * temp[(i - 1) * rowsize + j] + temp[(i - 1) * rowsize + j + 1]);

            // Gradient büyüklüğünü hesapla
            double gradientMagnitude = sqrt(gx * gx + gy * gy);

            // Sonucu ana görüntü veri alanına yerleştir
            image->data[i * rowsize + j] = (BYTE)gradientMagnitude;
        }
    }
    free(temp);
    return;
}
void laplacian(IMAGE *image) {
    DWORD w, h, rowsize;
    int i, j;
    double lap;
    BYTE *temp;
    w = image->bmpih.biw;
    h = image->bmpih.bih;
    rowsize = (image->bmpih.bibitcount * w + 31) / 32 * 4;
    temp = (BYTE *)calloc(h * rowsize, sizeof(char));

    if (temp == NULL) exit(1);
    memcpy(temp, image->data, h * rowsize);

    for (i = 1; i < h - 1; i++) {
        for (j = 1; j < w - 1; j++) {
            // Kenar pikselleri hesaplamayın
            if (i == 0 || i == h - 1 || j == 0 || j == w - 1) continue;

            // Laplacian hesaplaması
            lap = 4 * temp[i * rowsize + j] -
                  temp[(i - 1) * rowsize + j] -
                  temp[(i + 1) * rowsize + j] -
                  temp[i * rowsize + j - 1] -
                  temp[i * rowsize + j + 1] -
                  temp[(i - 1) * rowsize + j - 1] -
                  temp[(i - 1) * rowsize + j + 1] -
                  temp[(i + 1) * rowsize + j - 1] -
                  temp[(i + 1) * rowsize + j + 1];

            // Sonucu ana görüntü veri alanına yerleştir
            image->data[i * rowsize + j] = (BYTE)lap;
        }
    }
    free(temp);
    return;
}



void gradient(IMAGE *image) {
    DWORD w, h, rowsize;
    int i, j;
    double dx, dy;
    BYTE *temp;
    w = image->bmpih.biw;
    h = image->bmpih.bih;
    rowsize = (image->bmpih.bibitcount * w + 31) / 32 * 4;
    temp = (BYTE *)calloc(h * rowsize, sizeof(char));

    if (temp == NULL) exit(1);
    memcpy(temp, image->data, h * rowsize);

    for (i = 1; i < h - 1; i++) {
        for (j = 1; j < w - 1; j++) {
            // Kenar pikselleri hesaplamayın
            if (i == 0 || i == h - 1 || j == 0 || j == w - 1) continue;

            // Merkezi farklar yöntemi ile gradyan hesaplaması
            dx = (temp[(i + 1) * rowsize + j] - temp[(i - 1) * rowsize + j]) / 2.0;
            dy = (temp[i * rowsize + j + 1] - temp[i * rowsize + j - 1]) / 2.0;

            // Gradient büyüklüğünü hesapla
            double gradientMagnitude = sqrt(dx * dx + dy * dy);

            // Sonucu ana görüntü veri alanına yerleştir
            image->data[i * rowsize + j] = (BYTE)gradientMagnitude;
        }
    }
    free(temp);
    return;
}

void spatialHighPassFilter(IMAGE *image) {
    DWORD w, h, rowsize;
    int i, j;
    double filter[3][3] = { {-1, -1, -1},
                            {-1,  8, -1},
                            {-1, -1, -1} };
    double sum;
    BYTE *temp;
    w = image->bmpih.biw;
    h = image->bmpih.bih;
    rowsize = (image->bmpih.bibitcount * w + 31) / 32 * 4;
    temp = (BYTE *)malloc(h * rowsize * sizeof(BYTE));

    if (temp == NULL) exit(1);
    memcpy(temp, image->data, h * rowsize);

    for (i = 1; i < h - 1; i++) {
        for (j = 1; j < w - 1; j++) {
            // Apply the high-pass filter
            sum = filter[0][0] * temp[(i - 1) * rowsize + (j - 1)] +
                  filter[0][1] * temp[(i - 1) * rowsize +  j      ] +
                  filter[0][2] * temp[(i - 1) * rowsize + (j + 1)] +
                  filter[1][0] * temp[ i      * rowsize + (j - 1)] +
                  filter[1][1] * temp[ i      * rowsize +  j      ] +
                  filter[1][2] * temp[ i      * rowsize + (j + 1)] +
                  filter[2][0] * temp[(i + 1) * rowsize + (j - 1)] +
                  filter[2][1] * temp[(i + 1) * rowsize +  j      ] +
                  filter[2][2] * temp[(i + 1) * rowsize + (j + 1)];
            // Ensure the pixel value is within the valid range [0, 255]
            sum = sum < 0 ? 0 : (sum > 255 ? 255 : sum);
            // Assign the filtered value to the pixel
            image->data[i * rowsize + j] = (BYTE)sum;
        }
    }
    free(temp);
    return;
}





int main() {

    IMAGE *image = (IMAGE *)malloc(sizeof(IMAGE));

    // Sobel operators
    image = ImageRead(image, "C:\\Users\\Public\\Pictures\\kelebek.bmp");
    sobel(image);
    ImageWrite(image, "C:\\Users\\Public\\Pictures\\kelebek_with_sobel.bmp");
    
    //laplacian
    image = ImageRead(image, "C:\\Users\\Public\\Pictures\\kelebek.bmp");
    
    //medianFilter(image,5);     //can be used for better laplacian results
    //enhanceContrast(image,1.5);  //can be used for better laplacian results
    laplacian(image);
    ImageWrite(image, "C:\\Users\\Public\\Pictures\\kelebek_with_laplacian.bmp");
    
    //Gradient
    image = ImageRead(image, "C:\\Users\\Public\\Pictures\\kelebek.bmp");
    gradient(image);
    ImageWrite(image, "C:\\Users\\Public\\Pictures\\kelebek_with_gradient.bmp");
    
    //Spatial
    image = ImageRead(image, "C:\\Users\\Public\\Pictures\\kelebek.bmp");
    spatialHighPassFilter(image);
	ImageWrite(image, "C:\\Users\\Public\\Pictures\\kelebek_with_spatial.bmp");
	
	
    // Belleği serbest bırak
    free(image);
    return 0;
}