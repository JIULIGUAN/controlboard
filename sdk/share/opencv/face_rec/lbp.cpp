/*
 * Local Binary Pattern 3x3
 */
#include <stdio.h>
#include "cv.h"
#include "highgui.h"
#include "face_recognizer.h"

/* key histogram
									 . . . . . . .
									 . x x . x x .
					key1		 . x x . x x .  key2
									 . x x . x x .
									 . . x x x . .
									 . . x x x . .  key3
									 . . x x x . .
									 . . . . . . . 
 */
int key1[6][2] = {{1,1}, {2, 1}, {1, 2}, {2, 2}, {1, 3}, {2, 3}};
int key2[6][2] = {{4,1}, {5, 1}, {4, 2}, {5, 2}, {4, 3}, {5, 3}};
int key3[9][2] = {{2,4}, {3, 4}, {4, 4}, {2, 5}, {3, 5}, {4, 5}, {2, 6}, {3, 6}, {4, 6}};

typedef struct _LBP {
	int				nTrainFaces;			// the number of training images
	unsigned  char hist_idx[256];
	int				faceId;
	CvMat		**lbpHistVectArr;
	CvMat		**keyHistVectArr;
} LBP;

static LBP *lbp_recognizer = 0;
static int lbpGridX = 7, lbpGridY = 8;

static CvMat* freadCvMat(FILE * fileStorage)
{
	uchar *pdata;
	int step;
	CvSize size;
	int rows = 0, cols = 0, bytes;
	uchar type = 0;
	CvMat *mat;
	fread(&rows, sizeof(int), 1, fileStorage);
	fread(&cols, sizeof(int), 1, fileStorage);
	fread(&type, sizeof(uchar), 1, fileStorage);
	if (cols == 0 || rows == 0) {
		printf("Read CvMat error\n");
		return NULL;
	}

	if (type == 'i') {
		bytes = 4;
		mat = cvCreateMat( rows, cols, CV_32SC1 );
	} else if (type == 's') {
		bytes = 2;
		mat = cvCreateMat( rows, cols, CV_16SC1 );
	} else if (type == 'u') {
		bytes = 1;
		mat = cvCreateMat( rows, cols, CV_8UC1 );
	} else if (type == 'f') {
		bytes = 4;
		mat = cvCreateMat( rows, cols, CV_32FC1 );
	} else {
		printf("type error\n");
		return NULL;
	}

	cvGetRawData(mat, &pdata, &step, &size);
	fread(pdata, bytes, size.height*size.width, fileStorage);
	return mat;
}

static int fwriteCvMat(FILE *fileStorage, CvMat *mat)
{
	uchar *pdata;
	int step;
	CvSize size;
	int rows = 0, cols = 0, bytes;
	uchar c;

	if (CV_MAT_TYPE(mat->type) == CV_32SC1) {
		bytes = 4;
		c = 'i';
	} else if (CV_MAT_TYPE(mat->type) == CV_16SC1) {
		bytes = 2;
		c = 's';
	} else if (CV_MAT_TYPE(mat->type) == CV_8UC1) {
		bytes = 1;
		c = 'u';
	} else if (CV_MAT_TYPE(mat->type) == CV_32FC1) {
		bytes = 4;
		c = 'f';
	} else {
		printf("type error\n");
		return -1;
	}
	fwrite(&mat->rows, sizeof(int), 1, fileStorage);
	fwrite(&mat->cols, sizeof(int), 1, fileStorage);
	fwrite(&c, sizeof(uchar), 1, fileStorage);
	cvGetRawData(mat, &pdata, &step, &size);
	fwrite(pdata, bytes, size.height*size.width, fileStorage);
	return 0;
}

static void calculate_hist_index(unsigned char *hist_idx)
{
	int i, idx, cnt;

	hist_idx[0] = 0;
	hist_idx[255] = 1;
	idx = 2;
	for (i=1;i<255;i++) {
		cnt = 0;
		if ((i&0x01) != ((i>>1)&0x01))
			cnt++;
		if (((i>>1)&0x01) != ((i>>2)&0x01))
			cnt++;
		if (((i>>2)&0x01) != ((i>>3)&0x01))
			cnt++;
		if (((i>>3)&0x01) != ((i>>4)&0x01))
			cnt++;
		if (((i>>4)&0x01) != ((i>>5)&0x01))
			cnt++;
		if (((i>>5)&0x01) != ((i>>6)&0x01))
			cnt++;
		if (((i>>6)&0x01) != ((i>>7)&0x01))
			cnt++;
		if (((i>>7)&0x01) != (i&0x01))
			cnt++;

		if (cnt>2)
			hist_idx[i] = 255;
		else
			hist_idx[i] = idx++;
	}

	//for(i=0;i<256;i++)
	//	printf("hist_idx[%x] = %d\n", i, hist_idx[i]);

}

//------------------------------------------------------------------------------
// LBP 3x3
//------------------------------------------------------------------------------
static inline void olbp(IplImage* src, IplImage* dst) 
{

    // calculate patterns
    for(int i=1;i<src->height-1;i++) {
        for(int j=1;j<src->width-1;j++) {
            unsigned char center = (unsigned char)src->imageData[i*src->width+j];
            unsigned char code = 0;
            code |= ((unsigned char)src->imageData[(i-1)*src->width+j-1]>= center) << 7;
            code |= ((unsigned char)src->imageData[(i-1)*src->width+j]>= center) << 6;
            code |= ((unsigned char)src->imageData[(i-1)*src->width+j+1] >= center) << 5;
            code |= ((unsigned char)src->imageData[i*src->width+j+1] >= center) << 4;
            code |= ((unsigned char)src->imageData[(i+1)*src->width+j+1] >= center) << 3;
            code |= ((unsigned char)src->imageData[(i+1)*src->width+j] >= center) << 2;
            code |= ((unsigned char)src->imageData[(i+1)*src->width+j-1] >= center) << 1;
            code |= ((unsigned char)src->imageData[i*src->width+j-1] >= center) << 0;
            dst->imageData[(i-1)*src->width+j-1] = code;
        }
    }
}

static void histc(IplImage* src, CvMat *histogram, int idx)
{
	int i;
    // Establish the number of bins.
    int bins = 256;
    // Set the ranges.
    float range[] = { 0, 256 };
    float *ranges[] = {range};

	CvHistogram* hist = cvCreateHist( 1, &bins, CV_HIST_ARRAY, ranges, 1 );
	cvClearHist(hist);

    // calc histogram
    cvCalcHist(&src, hist);
	for(i=0;i<bins;i++) {
		float bin_val = cvQueryHistValue_1D( hist, i);
		if (lbp_recognizer->hist_idx[i]<LBP_BINS)
			histogram->data.s[idx+lbp_recognizer->hist_idx[i]] += (short)bin_val;
	}
	cvReleaseHist(&hist);
}

static CvMat* spatial_histogram(IplImage* srcImg)
{
    // calculate LBP patch size
    int grid_x = srcImg->width/LBP_BLOCK_X;
    int grid_y = srcImg->height/LBP_BLOCK_Y;
	IplImage* lbpImg = cvCreateImage(cvSize(srcImg->width, srcImg->height), IPL_DEPTH_8U, 1);  
	CvMat	*histogram = cvCreateMat(grid_x*grid_y*LBP_BINS, 1, CV_16SC1);
	int i,j;

	olbp(srcImg, lbpImg);
	/*{
		IplImage* testImg = cvCreateImage(cvSize(srcImg->width, srcImg->height), IPL_DEPTH_8U, 1);  
		for(i=0;i<srcImg->width*srcImg->height;i++)  {
			if (lbp_recognizer->hist_idx[lbpImg->imageData[i]] == 255)
				testImg->imageData[i] = lbp_recognizer->hist_idx[lbpImg->imageData[i]] ;
			else
				testImg->imageData[i] = 4*lbp_recognizer->hist_idx[lbpImg->imageData[i]] ;
		}
		cvSaveImage("test.bmp", testImg);
		//cvShowImage( "lbp", testImg );
		cvReleaseImage(&testImg);
	}*/
	cvZero(histogram);
    for (i = 0; i< grid_y; i++) {
		for (j = 0; j < grid_x; j++) {
			cvSetImageROI(lbpImg, cvRect(j*LBP_BLOCK_X, i*LBP_BLOCK_Y, LBP_BLOCK_X, LBP_BLOCK_Y));
            histc(lbpImg, histogram, (i*grid_x+j)*LBP_BINS);
        }
    }

	cvReleaseImage(&lbpImg);
	return histogram;
}

static void free_histArray(void)
{
	int i;
	if (lbp_recognizer->lbpHistVectArr) {
		for(i = 0; i < lbp_recognizer->nTrainFaces; i++) {
			if (lbp_recognizer->lbpHistVectArr[i])
				cvReleaseMat(&lbp_recognizer->lbpHistVectArr[i]);
		}
		cvFree(&lbp_recognizer->lbpHistVectArr);
		lbp_recognizer->lbpHistVectArr = 0;
	}
	if (lbp_recognizer->keyHistVectArr) {
		for(i = 0; i < lbp_recognizer->nTrainFaces; i++) {
			if (lbp_recognizer->keyHistVectArr[i])
				cvReleaseMat(&lbp_recognizer->keyHistVectArr[i]);
		}
		cvFree(&lbp_recognizer->keyHistVectArr);
		lbp_recognizer->keyHistVectArr = 0;
	}
}
void LBP_create(void)
{
	lbp_recognizer = (LBP *)cvAlloc(sizeof(LBP));
	memset(lbp_recognizer, 0, sizeof(LBP));
	calculate_hist_index(lbp_recognizer->hist_idx);
	lbp_recognizer->faceId = -1;
	lbp_recognizer->keyHistVectArr = 0;
	lbp_recognizer->lbpHistVectArr = 0;
}

void LBP_release(void)
{
	free_histArray();
	cvFree_(lbp_recognizer);
}

int LBP_load(const char *filename)
{
	FILE * fileStorage;
	int i,j,k;

	// create a file-storage interface
	fileStorage = fopen( filename, "rb" );
	if( !fileStorage ) {
		printf("Can't open training database file %s.\n", filename);
		return -1;
	}

	free_histArray();
	// Load the data
	fread(&lbp_recognizer->nTrainFaces, sizeof(int), 1, fileStorage);	
	lbp_recognizer->lbpHistVectArr = (CvMat **)cvAlloc(lbp_recognizer->nTrainFaces*sizeof(CvMat *));
	lbp_recognizer->keyHistVectArr = (CvMat **)cvAlloc(lbp_recognizer->nTrainFaces*sizeof(CvMat *));	

	for(i=0; i<lbp_recognizer->nTrainFaces; i++) {
		lbp_recognizer->keyHistVectArr[i] = cvCreateMat(3*LBP_BINS, 1, CV_16SC1);
		lbp_recognizer->lbpHistVectArr[i] = (CvMat *)freadCvMat(fileStorage);  

		for(j=0; j<LBP_BINS; j++) {
			lbp_recognizer->keyHistVectArr[i] ->data.s[j] = 0;
			lbp_recognizer->keyHistVectArr[i] ->data.s[LBP_BINS+j] = 0;
			lbp_recognizer->keyHistVectArr[i] ->data.s[LBP_BINS*2+j] = 0;

			for(k=0;k<6;k++) {
				lbp_recognizer->keyHistVectArr[i] ->data.s[j] += lbp_recognizer->lbpHistVectArr[i]->data.s[(key1[k][1]*lbpGridX+key1[k][0])*LBP_BINS+j];
				lbp_recognizer->keyHistVectArr[i] ->data.s[LBP_BINS+j] += lbp_recognizer->lbpHistVectArr[i]->data.s[(key2[k][1]*lbpGridX+key2[k][0])*LBP_BINS+j];
			}
			for(k=0;k<9;k++) 
				lbp_recognizer->keyHistVectArr[i] ->data.s[LBP_BINS*2+j] += lbp_recognizer->lbpHistVectArr[i]->data.s[(key3[k][1]*lbpGridX+key3[k][0])*LBP_BINS+j];
		}
	}

	fclose(fileStorage);
	printf("Loaded %d faces\n", lbp_recognizer->nTrainFaces);
	return 0;
}

// Save the training data to the file 'facedata.xml'.
int LBP_save(const char *filename)
{
	FILE *fileStorage;
	int i;

	// create a file-storage interface
	fileStorage = fopen(filename, "wb" );
	if( !fileStorage ) {
		printf("Can't open training database file %s.\n", filename);
		return -1;
	}

	fwrite(&lbp_recognizer->nTrainFaces, sizeof(int), 1, fileStorage);
	for(i=0; i<lbp_recognizer->nTrainFaces; i++)
		fwriteCvMat(fileStorage, lbp_recognizer->lbpHistVectArr[i]);

	fclose(fileStorage);
	//printf("Store %d training images\n", recognizer->nTrainFaces);
	return 0;
}

void generate_key_histogram(CvMat *keyHistogram, CvMat *histogram)
{
	int j;
	for(j=0; j<LBP_BINS; j++) {
			int k;
			keyHistogram->data.s[j] = 0;
			keyHistogram->data.s[LBP_BINS+j] = 0;
			keyHistogram->data.s[LBP_BINS*2+j] = 0;

			for(k=0;k<6;k++) {
				keyHistogram->data.s[j] += histogram->data.s[ (key1[k][1]*lbpGridX+key1[k][0])*LBP_BINS+j];
				keyHistogram->data.s[LBP_BINS+j] += histogram->data.s[(key2[k][1]*lbpGridX+key2[k][0])*LBP_BINS+j];
			}
			for(k=0;k<9;k++) 
				keyHistogram->data.s[LBP_BINS*2+j] += histogram->data.s[(key3[k][1]*lbpGridX+key3[k][0])*LBP_BINS+j];
	}
}

int LBP_train(int nTrainFaces, IplImage** faceImgArr) 
{
	int i;
	CvMat *kernel = cvCreateMat(3, 3, CV_32FC1);
	IplImage *blur = cvCreateImage (cvSize(faceImgArr[0]->width, faceImgArr[0]->height), IPL_DEPTH_16U, 1);
	
	cvSet(kernel, cvScalar(1));

	free_histArray();
	lbp_recognizer->nTrainFaces = nTrainFaces;
	lbp_recognizer->lbpHistVectArr = (CvMat **)cvAlloc(lbp_recognizer->nTrainFaces*sizeof(CvMat *));	
	lbp_recognizer->keyHistVectArr = (CvMat **)cvAlloc(lbp_recognizer->nTrainFaces*sizeof(CvMat *));	

	for(i = 0; i < nTrainFaces; i++)  {
		// blur the image
		cvConvertScale(faceImgArr[i], blur, 1);
		cvFilter2D(blur, blur, kernel);
		cvConvertScale(blur, faceImgArr[i], 1./9);
        lbp_recognizer->lbpHistVectArr[i] = spatial_histogram(faceImgArr[i]);
		lbp_recognizer->keyHistVectArr[i] = cvCreateMat(3*LBP_BINS, 1, CV_16SC1);;
		generate_key_histogram(lbp_recognizer->keyHistVectArr[i], lbp_recognizer->lbpHistVectArr[i]);
	}
	cvReleaseImage(&blur);
	cvReleaseMat(&kernel);
	return 0;
}

int LBP_predict(IplImage* faceImg)
{
	int i, iTrain, iNearest = -1;
	int leastDistSq = LBP_RECOGNIZED_HIGH;
	CvMat *testHistogram;
	CvMat *testKeyHistogram = cvCreateMat(3*LBP_BINS, 1, CV_16SC1);;
	CvMat *kernel = cvCreateMat(3, 3, CV_32FC1);
	IplImage *blur = cvCreateImage (cvSize(faceImg->width, faceImg->height), IPL_DEPTH_16U, 1);

	// blur the image
	cvConvertScale(faceImg, blur, 1);
	cvSet(kernel, cvScalar(1));
	cvFilter2D(blur, blur, kernel);
	cvConvertScale(blur, faceImg, 1./9);
	cvReleaseImage(&blur);
	cvReleaseMat(&kernel);

	testHistogram	= spatial_histogram(faceImg);
	generate_key_histogram(testKeyHistogram, testHistogram);

	for(iTrain=0; iTrain<lbp_recognizer->nTrainFaces; iTrain++)
	{
		int distSq=0;
		int keyDistSq=0;
		int skip = 0;

		for(i=0; i<lbp_recognizer->keyHistVectArr[iTrain]->rows; i++) {
			int d_i = testKeyHistogram->data.s[i] - lbp_recognizer->keyHistVectArr[iTrain]->data.s[i];
			keyDistSq += abs(d_i);
			if (keyDistSq>LBP_KEY_THRESHOLD) {
				skip = 1;
				break;
			}
		}
		if (skip) continue;

		for(i=0; i<lbp_recognizer->lbpHistVectArr[iTrain]->rows; i++) {
			int d_i = testHistogram->data.s[i] - lbp_recognizer->lbpHistVectArr[iTrain]->data.s[i];
			//int s_i = testHistogram->data.s[i] + lbp_recognizer->lbpHistVectArr[iTrain]->data.s[i];
			distSq += abs(d_i);///s_i;
			if (distSq > LBP_RECOGNIZED_HIGH)
				break;
		}

		if (distSq < LBP_RECOGNIZED_HIGH)
			printf("<%d> recognized %d\n", iTrain, distSq );

		if(distSq < leastDistSq)
		{
			leastDistSq = distSq;
			iNearest = iTrain;
		}
		if(leastDistSq < LBP_RECOGNIZED_LOW)
			break;
	}
	cvReleaseMat(&testHistogram);
	cvReleaseMat(&testKeyHistogram);
	lbp_recognizer->faceId = iNearest;
	// Return the found index.
	return iNearest;
}

int LBP_status(void)
{
	return lbp_recognizer->faceId;
}

int LBP_generate(IplImage* faceImg, short* lbpData)
{
	int i;
	CvMat *kernel = cvCreateMat(3, 3, CV_32FC1);
	IplImage *blur = cvCreateImage (cvSize(faceImg->width, faceImg->height), IPL_DEPTH_16U, 1);
	CvMat *histogram;

    lbpGridX= faceImg->width/LBP_BLOCK_X;
    lbpGridY = faceImg->height/LBP_BLOCK_Y;
	cvSet(kernel, cvScalar(1));
	// blur the image
	cvConvertScale(faceImg, blur, 1);
	cvFilter2D(blur, blur, kernel);
	cvConvertScale(blur, faceImg, 1./9);
    histogram = spatial_histogram(faceImg);
	for(i=0; i<histogram->rows; i++) {
		lbpData[i] = histogram->data.s[i] ;
	}
	cvReleaseMat(&histogram);
	cvReleaseImage(&blur);
	cvReleaseMat(&kernel);
	return 0;
}

int LBP_updateDB(int nTrainFaces, short *lbpDataArr)
{
	int i,j;

	free_histArray();
	lbp_recognizer->nTrainFaces = nTrainFaces;
	lbp_recognizer->lbpHistVectArr = (CvMat **)cvAlloc(lbp_recognizer->nTrainFaces*sizeof(CvMat *));	
	lbp_recognizer->keyHistVectArr = (CvMat **)cvAlloc(lbp_recognizer->nTrainFaces*sizeof(CvMat *));	

	for(i = 0; i < nTrainFaces; i++)  {
		CvMat	*histogram = cvCreateMat(lbpGridX*lbpGridY*LBP_BINS, 1, CV_16SC1);
		CvMat	*keyhistogram = cvCreateMat(3*LBP_BINS, 1, CV_16SC1);
		for(j=0; j<histogram->rows; j++) {
			histogram->data.s[j] = lbpDataArr[LBP_DATA_SIZE*i+j] ;
		}
		lbp_recognizer->lbpHistVectArr[i] = histogram;
		generate_key_histogram(keyhistogram, histogram);
		lbp_recognizer->keyHistVectArr[i] = keyhistogram;
	}
	return 0;
}

FaceRecognizer LBPRecognizer =
{
    "lbp",
	LBP_create,
	LBP_release,
	LBP_load,
	LBP_save,
	LBP_train,
	LBP_predict,
	LBP_status
};
