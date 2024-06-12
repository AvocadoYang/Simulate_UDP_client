#include <arpa/inet.h>
#include <thread>
#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

struct sockaddr_in IR_addr_serv;
using namespace cv;
using namespace std;

void setInterval(std::function<void()> func, int interval) {
    while (true) {
        func();
        std::this_thread::sleep_for(std::chrono::milliseconds(interval));
    }
}

#define IR_SERVER_PORT 30001
#define IR_SERVER_IP_ADDRESS "127.0.0.1"

int main(int argc, char **argv) {
    int IR_sock_fd;
    int IR_len;

    IR_sock_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if (IR_sock_fd == -1) {
        printf("%s", "Can't create socket");
        return 0;
    };

    memset(&IR_addr_serv, 0, sizeof(IR_addr_serv));
    IR_addr_serv.sin_family = AF_INET;
    IR_addr_serv.sin_addr.s_addr = inet_addr(IR_SERVER_IP_ADDRESS);
    IR_addr_serv.sin_port = htons(IR_SERVER_PORT);
    IR_len = sizeof(IR_addr_serv);

    vector<Mat> images;
    for (int i = 0; i <= 28; i++) {
        string filename = "./images/test-" + to_string(i) + ".png";
        Mat image = imread(filename, IMREAD_COLOR);
        if (!image.empty())
            images.push_back(image);
    }

    if (images.empty()) {
        cerr << "圖片載入失敗" << endl;
    }

    auto sendImageData = [&](const Mat& image) {
        vector<uchar> data_encode;
        vector<int> quality_param;
        quality_param.push_back(IMWRITE_JPEG_QUALITY);
        quality_param.push_back(30);
        imencode(".png", image, data_encode, quality_param);

        int nSize = data_encode.size();
        unsigned char *encodeImg = new unsigned char[nSize];
        for (int i = 0; i < nSize; i++)
            encodeImg[i] = data_encode[i];

        int bytes_sent = sendto(IR_sock_fd, encodeImg, nSize, 0, (struct sockaddr *)&IR_addr_serv, IR_len);
        if (bytes_sent == -1) {
            perror("Failed to send data");
        } else {
            cout << "Sent " << bytes_sent << " bytes to the server" << endl;
        }
        delete[] encodeImg;
    };

    int index = 0;

    setInterval([&]() {
        sendImageData(images[index]);
        index = (index + 1) % images.size();
    }, 100);

    return 0;
}
