#include"ImageMatch.h"


float distance(const float *dstA, const float *dstB,int num)
{
	float dist = 0.0;
	float a;
	for(int i=0;i<num;i++)
	{
		a = (dstA[i]-dstB[i])*(dstA[i]-dstB[i]);
		dist +=a;
	}
	return sqrt(dist);
}

int comp(const void *Dist1,const void *Dist2)
{
	float a = ((Dst*)Dist1)->dist-((Dst*)Dist2)->dist;
	int a1 = *((int*)(&a));
	int a2 = a;
	return *((int*)(&a));
}

/*-------------------------------------------------
function: LLC_Coding
intput  ��src      Ϊ�����ͼ������
		  codebook Ϊ�뱾
		  knn      Ϊ����ڵĸ�����
return  ��Coding���������
----------------------------------------------------*/
Mat LLC_coding_appr(Mat src,Mat codebook,int knn)
{
	int nframe = src.rows;
	int nbase = codebook.rows;
	int fPointdim = src.cols;

	
	Dst *Dist = (Dst *)malloc(sizeof(Dst)*nframe*nbase);

	for(int i=0;i<nframe;i++)
	{
		for(int j=0;j<nbase;j++)
		{
			Dist[i*nbase+j].ID = j;
			Dist[i*nbase+j].dist = norm(src.row(i),codebook.row(j),NORM_L2);
			//Dist[i*nbase+j].dist = distance(src.ptr<float>(i),codebook.ptr<float>(j),fPointdim);
		}
		qsort(&(Dist[i*nbase]),nbase,sizeof(Dst),comp);
	}

	Mat Coeff=Mat::zeros(nframe,nbase,CV_32FC1);
	Mat z(knn,fPointdim,CV_32FC1);
	Mat C,w;
	Mat II = Mat::eye(knn,knn,CV_32FC1);
	Mat ones(1,knn,CV_32FC1,1);

	for(int i=0;i<nframe;i++)
	{
		for(int k=0;k<knn;k++)
		{
			int ID = Dist[i*nbase+k].ID;
			subtract(codebook.row(ID),src.row(i),z.row(k));

		}

		gemm(z,z,1,NULL,0,C,GEMM_2_T);
		Scalar tr = trace(C);
		gemm(C,II,1,II,tr.val[0]*0.0001,C,GEMM_2_T);
		gemm(C.inv(),ones,1,NULL,0,w,GEMM_2_T);

		float sum=0;
		for(int row=0;row<w.rows;row++)
		{
			sum += w.at<float>(row,0);
		}
		for(int k=0;k<knn;k++)
		{
			int ID = Dist[i*nbase+k].ID;
			Coeff.at<float>(i,ID) = w.at<float>(k,0)/sum;
		}
	}

	free(Dist);
	return Coeff.t();
}

/*-------------------------------------------------
function: LLC���� 
intput  ��feature  Ϊ�����ͼ������
		  codebook Ϊ�뱾
		  knn      Ϊ����ڵĸ���
		  width    ����ͼ��Ŀ�
		  height   ����ͼ��ĸ�
return  ��������������
----------------------------------------------------*/

int* LLC_pooling(Mat feature,Mat codebook,int knn,int width, int height)
{
	/*����Dense sift�����ö�߶�pooling������Ĭ���ǵ��߶ȵ�*/
	const int NUM = 1;//Ϊ����Ƶ��
	int  pyramid[NUM]={1};

	Mat LLC_codes = LLC_coding_appr(feature,codebook,knn);
	
	int tBins =0;
	for(int i=0;i<NUM;i++)
	{
		tBins +=  pyramid[i]* pyramid[i];
	}
	
	int *beta = (int *)malloc(sizeof(int)*tBins*LLC_codes.rows);
	float *beta_f = (float *)malloc(sizeof(float)*tBins*LLC_codes.rows);
	float sum =0;
	int feature_dim = 0;
	int bId = 0;

	for(int iter1 = 0; iter1<NUM; iter1++)
	{
		int nBins = pyramid[iter1]*pyramid[iter1];

		int wUnit = width/pyramid[iter1] + 1;
		int hUnit = height/pyramid[iter1] + 1;

		for(int iter2 = 0; iter2<nBins; iter2++)
		{
			bId ++;
			for(int i=0;i<LLC_codes.rows;i++)
			{
				float *p = LLC_codes.ptr<float>(i); 
				float max =0;
				for(int j=0; j<LLC_codes.cols; j++)
				{
					max = max>p[j]?max:p[j];
				}

				beta_f[feature_dim]= max;
				sum += max*max;
				feature_dim++;
			}
		}
	}
	if(bId!=tBins)
	{
		printf("Index number error!");
		return NULL;
	}
	sum = 1/sqrt(sum);
	for(int i=0;i<feature_dim;i++)
	{
		beta[i] = beta_f[i]*sum*131072;//���㶨�㻯
	}
	free(beta_f);
    return beta;
}


/*----------------------------------------------------------
function��������ȡ(Ŀǰû�м���LLC)
input:    image      �����RGB24ͼ��ָ��
height     ����ͼ��ĸ�
width      ����ͼ��Ŀ�
widthstep  һ��ͼ����ռ���ֽ���
featuredim ���ݿ���ͼ��������ά��

return    ��ȡ��ͼ��������ָ��
----------------------------------------------------------*/

int* ImageFeatureExtract(uchar *Image, int width, int height, int widthstep, int featuredim)//, FILE * fp_codebook)  //featuredim = 1024
{
	initModule_nonfree();//��ʼ��ģ�飬ʹ��SIFT��SURFʱ�õ�
	Ptr<FeatureDetector> detector = FeatureDetector::create("SIFT");//����SIFT���������
	Ptr<DescriptorExtractor> descriptor_extractor = DescriptorExtractor::create("SIFT");//������������������
	if (detector.empty() || descriptor_extractor.empty()){
		cout << "fail to create detector!";
		return NULL;
	}
	Mat img(height, width, CV_8UC3, Image, widthstep);//TODO:���ݲ�ͬ���ѡ�����
	int maxImSize = 300;
	int maxEdge = width > height?width:height;    //resize �������
	if (maxEdge > maxImSize){
		resize(img,img,Size(width/(maxEdge/(float)maxImSize),height/(maxEdge/(float)maxImSize)));
	}
	vector<KeyPoint> keypoints1;
	Mat descriptors1;
	detector->detect(img, keypoints1);
	descriptor_extractor->compute(img, keypoints1, descriptors1);
	cout << "Num of SIFT : " << descriptors1.size() << endl;

	if(keypoints1.empty()){
		int *value = (int *)malloc(sizeof(int)*featuredim);
		for(int i=0;i < featuredim;i++){
			value[i] = 0;
		}
		cout << "zero feature" << endl;
		return value;
	}
	for(int i=0;i<descriptors1.rows;i++){    //�Ķ��Ĳ���
		normalize(descriptors1.row(i),descriptors1.row(i));    //��ÿһ�н����˹�һ��
	}
	//LLC
	int *value = LLC_pooling(descriptors1,codebook,5, img.cols, img.rows);
	return value;
}

void ReleaseFeature(int *feature)
{
	free(feature);
	feature = NULL;
}
