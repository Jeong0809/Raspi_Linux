#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;

int main()
{
		cv::Point3_<int> pt1(100, 200, 300);
		cv::Point3_<float> pt2(92.3f, 125.23f, 250.f);
		
		cv::Point3f pt3(0.3f, 0.f, 15.7f);
		cv::Point3d pt4(0.25, 0.6, 33.3);

		cv::Point3i pt5 = pt1 - (cv::Point3i)pt2;
		cv::Point3f pt6 = pt2 * 3.14f;
		cv::Point3d pt7 = (pt4 + (cv::Point3d)pt3)*10.f;

		std::cout << "pt5 = "<< pt5.x << ", " << pt5.y << ", " << pt5.z <<  std::endl;
		std::cout << "두 벡터(pt4, pt4)의 내적" << pt4.dot(pt7) << std::endl;
		std::cout << "[pt6]=" << pt6 << std::endl;
		std::cout << "[pt7]=" << pt7 << std::endl;
		return 0;

		/*cv::Point_<int> pt1(100, 200);
		cv::Point_<float> pt2(92.3f, 125.23f);
		cv::Point_<double> pt3(100.2, 300.9);
		
		cv::Point2i pt4(120, 69);
		cv::Point2f pt5(0.3f, 0.f), pt6(0.f, 0.4f);
		cv::Point2d pt7(0.25, 0.6);
		

		cv::Point pt8 = pt1 + (cv::Point) pt2;
		cv::Point2f pt9 = pt6 * 3.14f;
		cv::Point2d pt10 = (pt3 + (cv::Point2d) pt6)*10;

		std::cout << "pt8="<< pt8.x << ", " << pt8.y << std::endl;
		std::cout << "[pt9]=" << pt9 << std::endl;
		std::cout << (pt2==pt6) << std::endl;
		std::cout << "pt7과 pt8의 내적" << pt7.dot(pt8) << std::endl;
		return 0;*/
}
