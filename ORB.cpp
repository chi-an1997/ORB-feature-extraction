#include <iostream>
#include <opencv2//core/core.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;
int main() {
	cv::Mat img_1 = cv::imread("001.tif");
	cv::Mat img_2 = cv::imread("002.tif");
	imshow("img_1", img_1);
	imshow("img_2", img_2);
	waitKey(0);
	destroyAllWindows();

	vector<cv::KeyPoint> keypoints_1, keypoints_2;
	cv::Mat descripions_1, descripions_2;
	cv::Ptr<cv::FeatureDetector> detector = cv::ORB::create();    //feature points detector
	cv::Ptr<cv::DescriptorExtractor> extractor = cv::ORB::create();    //feature points descriptor
	cv::Ptr<cv::DescriptorMatcher> matcher = cv::DescriptorMatcher::create("BruteForce-Hamming"); //Hamming distance matching

	//STEP1 detect feature points
	detector->detect(img_1, keypoints_1);  //detect feature points on img_1，store in keypoints_1
	detector->detect(img_2, keypoints_2);

	//STEP2 compute descriptor
	extractor->compute(img_1, keypoints_1, descripions_1);
	extractor->compute(img_2, keypoints_2, descripions_2);

	cv::Mat outimg_1, outimg_2;
	cv::drawKeypoints(img_1, keypoints_1, outimg_1, cv::Scalar::all(-1),
		cv::DrawMatchesFlags::DEFAULT);
	cv::drawKeypoints(img_2, keypoints_2, outimg_2, cv::Scalar::all(-1),
		cv::DrawMatchesFlags::DEFAULT);
	cv::imshow("ORB Keypoints_img_1", outimg_1);
	cv::imshow("ORB Keypoints_img_2", outimg_2);
	cv::waitKey(0);
	cv::destroyAllWindows();

	//STEP3 Matching
	vector<cv::DMatch> matches;
	matcher->match(descripions_1, descripions_2, matches);  //store descripions_1 and descripions_2 into Dmatch

	//STEP4 matching points select
	auto min_max = minmax_element(matches.begin(), matches.end(),
		[](const cv::DMatch& m1, const cv::DMatch& m2) {return m1.distance < m2.distance; }
	); // compute minimum distance and maximun distance
	double min_dist = min_max.first->distance;
	double max_dist = min_max.second->distance;

	vector<cv::DMatch> good_matches;
	for (int i = 0; i < descripions_1.rows; i++) {
		if (matches[i].distance <= max(2 * min_dist, 30.0)) {
			good_matches.push_back(matches[i]);
		}
	}

	//STEP5 draw matching result
	cv::Mat img_match;
	cv::Mat img_goodmatch;
	cv::drawMatches(img_1, keypoints_1, img_2, keypoints_2, matches, img_match);
	cv::drawMatches(img_1, keypoints_1, img_2, keypoints_2, good_matches, img_goodmatch);
	cv::imshow("all matches", img_match);
	cv::imshow("good matches", img_goodmatch);
	cv::waitKey(0);
	cv::destroyAllWindows();
	return 0;
}
