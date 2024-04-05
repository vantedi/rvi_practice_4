//Задание 1
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

int main() {
    setlocale(LC_ALL, "Russian");

    String videoPath = "C:/Users/James-Bond/Downloads/video.mp4";
    VideoCapture cap(videoPath);

    if (!cap.isOpened()) {
        cout << "ошибка загрузки видео" << endl;
        return -1;
    }

    int width = static_cast<int>(cap.get(CAP_PROP_FRAME_WIDTH));
    int height = static_cast<int>(cap.get(CAP_PROP_FRAME_HEIGHT));

    int low_H = 100, low_S = 10, low_V = 140;
    int high_H = 255, high_S = 60, high_V = 255;

    int video_codec = VideoWriter::fourcc('X', 'V', 'I', 'D');
    VideoWriter videoOutput("C:/Users/James-Bond/Downloads/output_video.mp4", video_codec, 20, Size(width, height));

    while (true) {
        Mat frame;
        cap >> frame;

        if (frame.empty())
            break;

        namedWindow("исходное видео", WINDOW_NORMAL);
        resizeWindow("исходное видео", width, height);
        imshow("исходное видео", frame);

        GaussianBlur(frame, frame, Size(3, 3), 0);

        Mat hsv;
        cvtColor(frame, hsv, COLOR_BGR2HSV);

        Vec3b lower_hsv = Vec3b(low_H, low_S, low_V);
        Vec3b upper_hsv = Vec3b(high_H, high_S, high_V);

        Mat mask;
        inRange(hsv, lower_hsv, upper_hsv, mask);

        namedWindow("HSV видео", WINDOW_NORMAL);
        resizeWindow("HSV видео", width, height);
        imshow("HSV видео", mask);

        Mat edges;
        Canny(mask, edges, 30, 90);

        dilate(edges, edges, getStructuringElement(MORPH_RECT, Size(3, 3)));
        erode(edges, edges, getStructuringElement(MORPH_RECT, Size(3, 3)));

        vector<vector<Point>> contours_white;
        findContours(edges, contours_white, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

        for (auto& contour : contours_white) {
            double area = contourArea(contour);
            if (area > 1000) {
                RotatedRect rotated_rect = minAreaRect(contour);
                Point2f vertices[4];
                rotated_rect.points(vertices);
                for (int i = 0; i < 4; ++i) {
                    line(frame, vertices[i], vertices[(i + 1) % 4], Scalar(255, 0, 0), 2);
                }
            }
        }

        namedWindow("полученное видео", WINDOW_NORMAL);
        resizeWindow("полученное видео", width, height);
        imshow("полученное видео", frame);
        videoOutput.write(frame);

        if (waitKey(1) == 27) {
            break;
        }
    }

    cap.release();
    videoOutput.release();
    destroyAllWindows();

    return 0;
}
