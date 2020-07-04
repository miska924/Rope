#include <opencv4/opencv2/opencv.hpp>
#include <GL/freeglut.h>
#include <bits/stdc++.h>

using namespace std;
using namespace cv;

using ll = long long;
using vvb = vector<vector<bool> >;
using vvi = vector<vector<int> >;

const int K = 3;
const int WinHei = 1000;
const int WinWid = 1000;

const double EPS = 1e-6;
const double SLOWNESS = 0.9999;
const int INF = 1e9;
const int SZ = 150;//200

const int DIM = 1000;
const int ALL = SZ * 4;
const int ROPE = 1;
const double STEP = 0.1 / DIM;

const long long P = 503 * 503;

class point {
  public:
    double x, y;

    point(double xx, double yy);

    point(int id);

    point norm(double len) const;

    point operator-() const;

    point operator+(const point& other) const;

    point operator-(const point& other) const;

    point operator*(double k) const;

    point operator/(double k) const;

    double len() const;

    bool operator==(const point& other) const;

    bool operator!=(const point& other) const;
};
