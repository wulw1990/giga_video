#include <string>
#include <vector>
#include <algorithm>

using namespace std;

int train(int argc, char** argv);

int main(int argc, char **argv){
	assert(argc >= 2);
	string mode = string( argv[1] );
	argc--;
	argv++;

	transform(mode.begin(), mode.end(), mode.begin(), ::tolower);

	if(mode == "train") train(argc, argv);
	else {
		cerr << "main mode error : " << mode << endl;
		return -1;
	}
	return 0;
}

int alignment_test(int argc, char** argv){
	AlignerCpp aligner;
	Mat gray = imread(argv[3], 0);
	Alignment align(argv[1], argv[2]);
	imgbase::Rect rect;
	vector<imgbase::Point2f> align21;
	ofstream out("pose_deopencv.txt");
	if(aligner.alignOnlyOneImage(gray.data, gray.rows, gray.cols, rect, align21)){
		vector<imgbase::Point2f> align106 = align.alignment(gray.data, gray.rows, gray.cols, 
							gray.channels(), align21);

		cv_dealer.drawPoint(gray, align106);
		imshow("a", gray);
		waitKey();
	}else{
		cout << "no face" << endl;
	}
	out.close();
	return 0;
}

int demo(int argc, char** argv){
	cout << "demo" << endl;
	return 0;
}