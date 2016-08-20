#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>

using namespace cv;

// Function Declarations
void printUsage(char* arg0);
void threshold(int, void*);
void initLiveWindow();
void run(Mat img);

// Global Variables
Mat hue;
Mat lastThreshResult;
int minHue = 0;
int maxHue = 0;
std::string windowId = "CV LIVE THRESHOLDING DEMO";

int main(int argc, char* argv[])
{
    Mat inputImg;

    std::cout << "argc: " << argc << std::endl;

    // Live Window Mode
    if(argc == 3)
    {
        if(strcmp(argv[1], "-d") != 0)
        {
            printUsage(argv[0]);
            return 1;
        }

        // This accepts numerical input, or argument like /dev/video0 (tested)
        std::cout << "Initializing Camera @ Index " << argv[2] << std::endl;
        VideoCapture cam = VideoCapture(atoi(argv[2]));

        initLiveWindow();

        while(true)
        {
            Mat inputImg;
            cam.read(inputImg);

            run(inputImg);

            // Only breaks on Esc
            if(waitKey(30) == 27)
                break;
        }
    }
    // File Mode
    else if(argc == 2)
    {
        // Flag needs argument, can't be alone
        if(strcmp(argv[1], "-d") == 0)
        {
            printUsage(argv[0]);
            return 1;
        }

        // TODO: Exits immediately, fix plz
        std::cout << "File Mode\n";
        inputImg = imread(argv[1]);
        run(inputImg);
        return 0;
    }
    else
    {
        printUsage(argv[0]);
        return 1;
    }

    return 0;
}

// Initialize the Window and the Trackbars
void initLiveWindow()
{
    namedWindow(windowId, WINDOW_NORMAL);

    createTrackbar("hueMin", windowId, &minHue, 255, threshold);
    createTrackbar("hueMax", windowId, &maxHue, 255, threshold);
}

// Do all the thresholding everytime a slider value is updated
void threshold(int, void*)
{
    using namespace cv;

    Mat threshLow;
    Mat threshHigh;
    threshold(hue, threshLow, minHue, 255, THRESH_BINARY);
    threshold(hue, threshHigh, maxHue, 255, THRESH_BINARY_INV);
    lastThreshResult = threshLow & threshHigh;

    imshow(windowId, lastThreshResult);
}

// Processes the image and performs thresholding
void run(Mat img)
{
    // Blur the image to smooth it out (especially with JPG's)
    GaussianBlur(img, img, Size(3, 3), 1, 1);

    // Convert to HSV
    Mat cvted;
    cvtColor(img, cvted, CV_BGR2HSV);

    // Isolate the Hue Channel, and store in global variable
    std::vector<Mat> separated(3);
    split(cvted, separated);
    hue = separated.at(0).clone();

    imshow(windowId, img);
    // Do the image processing once initially (parameters have no significance)
    threshold(1, NULL);
}

void printUsage(char* arg0)
{
        std::cout << std::endl;
        std::cout << "Image Loading:\n";
        std::cout << "  Usage: " << arg0 << " <imgpath>\n\n";
        std::cout << "Live Video Feed Loading:\n";
        std::cout << "  Usage: " << arg0 << " -d <videoDeviceID>\n\n";
}
