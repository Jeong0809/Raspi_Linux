#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;

void differOp(Mat img, Mat& dst, int mask_size){
	dst = Mat(img.size(), CV_8U, Scalar(0));
	Point h_m(mask_size / 2, mask_size / 2);
	int mask_length = mask_size * mask_size;

	for(int i = h_m.y; i < img.rows - h_m.y; i++){
		for(int j = h_m.x; j < img.cols - h_m.x; j++){
			vector<uchar> mask(mask_length, 0);
			for(int u = 0, k = 0; u < mask_size; u++){
				for(int v = 0; v < mask_size; v++, k++){
					int y = i + u - h_m.y;
					int x = j + v - h_m.x;
					mask[k] = img.at<uchar>(y, x);
				}
			}
			uchar max = 0;
			for(int k = 0; k < mask_length / 2; k++){
				int start = mask[k];
				int end = mask[mask_length - k - 1];

				uchar difference = abs(start - end);
				if(difference > max) max = difference;
			}
			dst.at<uchar>(i, j) = max;
		}
	}
}

int main()
{
	Mat image = imread("../Linux_1216/lena.bmp", 0);
	CV_Assert(image.data);

	Mat edge;
	differOp(image, edge, 3);

	imshow("image", image), imshow("edge", edge);
	waitKey();
	return 0;
}
