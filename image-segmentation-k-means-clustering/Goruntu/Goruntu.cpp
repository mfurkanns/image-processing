#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <ctime>
#include <math.h>
#include <iostream>
using namespace std;
using namespace cv;


int euclidianDistance(Vec3b sample, Vec3b main) {
	return pow(pow((sample.val[0] - main.val[0]),2)+ pow((sample.val[1] - main.val[1]), 2) + pow((sample.val[2] - main.val[2]), 2),0.5);
}

class MColor {

public:
	int label;
	int numberOfElement = 0;
	int toplamX = 0;
	int toplamY = 0;
	int toplamZ = 0;
	Vec3b color;
	
};


int yakinBul(Vec3b a, vector<MColor> &means) {

	int min = euclidianDistance(a, means.at(0).color);
	int ind = 0;
	for (int i = 1; i < means.size(); i++)
	{
		int tmp = euclidianDistance(a, means.at(i).color);
		if (tmp < min) {
			min = tmp;
			ind = means.at(i).label;
		}
	}
	return ind;

}

Mat labeling(Mat &image,Mat &labels, vector<MColor> &means,int m) {

	for (int i = 0; i < image.rows; i++)
	{
		for (int j = 0; j < image.cols; j++)
		{
			int tmp = yakinBul(image.at<Vec3b>(i,j),means);

			int temp = labels.at<Vec3b>(i, j)[0];

			if (temp == m) {

				labels.at<Vec3b>(i, j)[0] = tmp;
				
				means.at(tmp).toplamX += image.at<Vec3b>(i, j)[0];
				means.at(tmp).toplamY += image.at<Vec3b>(i, j)[1];
				means.at(tmp).toplamZ += image.at<Vec3b>(i, j)[2];
				means.at(tmp).numberOfElement += 1;
				means.at(tmp).color[0] = (means.at(tmp).toplamX / means.at(tmp).numberOfElement);
				means.at(tmp).color[1] = (means.at(tmp).toplamY / means.at(tmp).numberOfElement);
				means.at(tmp).color[2] = (means.at(tmp).toplamZ / means.at(tmp).numberOfElement);
				
			}
			else if((tmp!=temp)){			
				means.at(temp).toplamX -= image.at<Vec3b>(i, j)[0];
				means.at(temp).toplamY -= image.at<Vec3b>(i, j)[1];
				means.at(temp).toplamZ -= image.at<Vec3b>(i, j)[2];
				means.at(temp).numberOfElement -= 1;				

				labels.at<Vec3b>(i, j)[0] = tmp;

				means.at(tmp).toplamX += image.at<Vec3b>(i, j)[0];
				means.at(tmp).toplamY += image.at<Vec3b>(i, j)[1];
				means.at(tmp).toplamZ += image.at<Vec3b>(i, j)[2];
				means.at(tmp).numberOfElement += 1;
			}			
		}
	}

	for (int  i = 0; i < m-1; i++)
	{
		if (means.at(i).numberOfElement==0)
			means.at(i).numberOfElement = 1;
		means.at(i).color[0] = (means.at(i).toplamX / means.at(i).numberOfElement);
		means.at(i).color[1] = (means.at(i).toplamY / means.at(i).numberOfElement);
		means.at(i).color[2] = (means.at(i).toplamZ / means.at(i).numberOfElement);
	}

	return labels;
}



Mat connecteComp(Mat labels ) {
	
	map<int, int> relation;

	int** dizi =0;
	dizi = (int**)malloc(sizeof(int*) * labels.rows);

	for (int h = 0; h < labels.rows; h++)
	{
		dizi[h] = (int*)malloc(sizeof(int) * labels.cols);
	}

	
	dizi[0][0] = 0;	
	relation[dizi[0][0]] = 0;
	int x = 1;
	for (int i = 1; i < labels.cols; i++)
	{
		if (labels.at<Vec3b>(0, i)[0] == labels.at<Vec3b>(0, i - 1)[0]) {
			dizi[0][i] = dizi[0][i - 1];	
		}
		else {
			dizi[0][i] = x;
			relation[dizi[0][i]] = dizi[0][i];
			x++;
		}
	}

	for (int i = 1; i < labels.rows; i++)
	{
		if (labels.at<Vec3b>(i, 0)[0] == labels.at<Vec3b>(i - 1, 0)[0]) {
			dizi[i][0] = dizi[i - 1][0];					
		}
		else {
			dizi[i][0] = x;
			relation[dizi[i][0]] = dizi[i][0];
			x++;
		}
	}
		
	for (int i = 1; i < labels.rows; i++)
	{
		for (int j = 1; j < labels.cols; j++)
		{
			if (labels.at<Vec3b>(i - 1, j)[0] == labels.at<Vec3b>(i, j - 1)[0]) {
				if (relation[ dizi[i - 1][j]] <relation [dizi[i][j-1]]) {
					relation[dizi[i][j - 1]] = relation[dizi[i - 1][j]];
				}
				else {
					relation[dizi[i - 1][j]] = relation[dizi[i][j - 1]];
				}
			}			
			
			if (labels.at<Vec3b>(i - 1, j)[0] == labels.at<Vec3b>(i, j)[0]) {				
				dizi[i][j] = relation[dizi[i - 1][j]] ;
			}

			else if (labels.at<Vec3b>(i, j - 1)[0] == labels.at<Vec3b>(i, j)[0]) {
				dizi[i][j] = relation[dizi[i][j - 1]];			
			}
			else if (labels.at<Vec3b>(i - 1, j - 1)[0] == labels.at<Vec3b>(i, j)[0]) {
				dizi[i][j] =relation[ dizi[i - 1][j - 1]];			
			}
			else {
				dizi[i][j] = x;
				relation[dizi[i][j]]= x;
				x++;
			}			
		}
	}


	for (int  i = 0; i < labels.rows; i++)
	{
		for (int j = 0; j < labels.cols; j++)
		{
			dizi[i][j] = relation[dizi[i][j]];
		}
	}

	map<int, Vec3b> color;

	srand((unsigned)time(0));

	for (int i = 0; i < relation.size(); i++)
	{
		int x = unsigned(((rand() % 1500) * 97) % 256);
		int y = unsigned(((rand() % 1700) * 31) % 256);
		int z = unsigned(((rand() % 2500) * 17) % 256);

		color[relation[i]][0] = x;
		color[relation[i]][1] = y;
		color[relation[i]][2] = z;
		
	}

	Mat result =  labels.clone();

	for (int i = 0; i < labels.rows; i++)
	{
		for (int j = 0; j < labels.cols; j++)
		{			
			result.at<Vec3b>(i, j) = color[dizi[i][j]];
		}
	}

	return result;
}


int main() {

	srand((unsigned)time(0));
	int k = 8;
	int m = k + 1;
	Mat image, tmp;
	bool cik = true;
	map<int, int> relation;

	image = imread("shark.jpg", 1);
	Mat labels = imread("shark.jpg", 1);
	Mat result = imread("shark.jpg", 1);
	


	vector<MColor> means;
	for (int i = 0; i < k; i++)
	{
		MColor tmp;
		int x = rand() % image.rows;
		int y = rand() % image.cols;
		Vec3b colorr = image.at<Vec3b>(x,y);
		tmp.color = colorr;
		tmp.label = i;	
		means.push_back(tmp);
	}

	for (int i = 0; i < labels.rows; i++)
	{
		for (int j = 0; j < labels.cols; j++)
		{
			labels.at<Vec3b>(i, j)[0] = m;
		}
	}
	
	while (cik) {


		int tmp22 = 0;
		
		vector<MColor> tmpMeans = means;
		
		labeling(image, labels, means,m);	

		for (int i = 0; i < k; i++)
		{
			tmp22 += euclidianDistance(tmpMeans.at(i).color, means.at(i).color);
		}
		printf("%d \n", tmp22);
		if (tmp22 == 0)
			cik = false;
		else
			tmp22 = 0;
		
		printf("\n");
	}

	Mat output = connecteComp(labels);	

	for (int i = 0; i < image.rows; i++)
		{
			for (int j = 0; j < image.cols; j++)
			{
				result.at<Vec3b>(i, j) = means.at(labels.at<Vec3b>(i, j)[0]).color;
			}
		}

	namedWindow("b", WINDOW_FULLSCREEN);
	imshow("b", output);

	namedWindow("im2", WINDOW_AUTOSIZE);
	imshow("im2", image);

	namedWindow("a", WINDOW_FULLSCREEN);
	imshow("a", result);
	waitKey(0);


	return 0;
}