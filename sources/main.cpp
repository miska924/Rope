#include "rope.hpp"

using vvb = vector<vector<bool> >;
using vvi = vector<vector<int> >;

const int K = 3;
const int WinHei = 800;
const int WinWid = 800;

const double EPS = 1e-6;
const double SLOWNESS = 0.9999;
const int INF = 1e9;
const int SZ = 200;

const int DIM = 200;
const int ALL = SZ * 4;
const int ROPE = 4;
const double STEP = 0.001;

class point {
  public:
    double x, y;

    point(double xx, double yy) {
      x = xx;
      y = yy;
    }

    point(int id) {
      assert(0 <= id && id < ALL);
      if (id < SZ) {
        x = double(id) / SZ;
        y = 0;
      } else if (id < SZ * 2) {
        x = 1.0;
        y = double(id - SZ) / SZ;
      } else if (id < SZ * 3) {
        x = 1.0 - double(id - SZ * 2) / SZ;
        y = 1.0;
      } else {
        x = 0;
        y = 1.0 - double(id - SZ * 3) / SZ;
      }
    }

    point norm(double len) const {
      return point(x * len / (*this).len(), y * len / (*this).len());
    }

    point operator-() const {
      return point(-x, -y);
    }

    point operator+(const point& other) const {
      return point(x + other.x, y + other.y);
    }

    point operator-(const point& other) const {
      return point(x - other.x, y - other.y);
    }

    point operator*(double k) const {
      return point(x * k, y * k);
    }

    point operator/(double k) const {
      return point(x / k, y / k);
    }

    double len() const {
      return sqrt(x * x + y * y);
    }

    bool operator==(const point& other) const {
      return (abs(x - other.x) < EPS) && (abs(y - other.y) < EPS);
    }

    bool operator!=(const point& other) const {
      return !(*this == other);
    }
};

vvb cur;
long long cure;
vvi cur_got;
Mat image;

void fitness() { // const Mat<CV_8UC1>& im
  vector<vector<int> > got(DIM, vector<int>(DIM, 0));
  for (int i = 0; i < ALL; ++i) {
    for (int j = i + 1; j < ALL; ++j) {
      if (cur[i][j]) {
        point a(i);
        point b(j);
        for (double k = 0; k < (b - a).len(); k += STEP) {
          point cur = a + (b - a).norm(k);
          if (cur.x < 1.0 && cur.y < 1.0 && cur.x >= 0 && cur.y >= 0) {
            got[int(cur.x * DIM)][int(cur.y * DIM)]++;
          }
        }
      }
    }
  }
  long long res = 0;
  for (int i = 0; i < DIM; ++i) {
    for (int j = 0; j < DIM; ++j) {
      long long coef = int(K * (point(0.5, 0.5) - point(1.0 * i / DIM, 1.0 * j / DIM)).len());
      coef *= coef;
      coef *= coef;
      long long x = max(0, abs(255 - got[i][j] * ROPE - int(image.at<uint8_t>(i, j))));
      x *= x * (4ll * K * K * K * K - coef);
      res += x;
    }
  }
  cure = res;
  cur_got = got;
}

void Draw() {
	glClear(GL_COLOR_BUFFER_BIT);
	glPushMatrix();
/*
  Mat got(DIM, DIM, CV_8UC1);
  for (int i = 0; i < got.rows; ++i) {
    for (int j = 0; j < got.cols; ++j) {
      got.at<uint8_t>(i, j) = 255;
    }
  }

  for (int i = 0; i < ALL; ++i) {
    for (int j = i + 1; j < ALL; ++j) {
      if (cur[i][j]) {
        point a(i);
        point b(j);
        for (double k = 0; k < (b - a).len(); k += STEP) {
          point cur = a + (b - a).norm(k);
          if (cur.x < 1.0 && cur.y < 1.0 && cur.x >= 0 && cur.y >= 0) {
            got.at<uint8_t>(int(cur.x * DIM) , int(cur.y * DIM)) = max(0, int(got.at<uint8_t>(int(cur.x * DIM), int(cur.y * DIM))) - ROPE);
          }
        }
      }
    }
  }

  for (int i = 0; i < DIM; ++i) {
    for (int j = 0; j < DIM; ++j) {
      double c = double(got.at<uint8_t>(i, j)) / 255;
      glColor4f(c, c, c, 0);
      glBegin(GL_QUADS);
        glVertex2f(double(j) / DIM, 1.0 - double(i) / DIM);
        glVertex2f(double(j + 1) / DIM, 1.0 - double(i) / DIM);
        glVertex2f(double(j + 1) / DIM, 1.0 - double(i + 1) / DIM);
        glVertex2f(double(j) / DIM, 1.0 - double(i + 1) / DIM);
      glEnd();
    }
  }*/

  glColor4f(0, 0, 0, 0.9);
  for (int i = 0; i < ALL; ++i) {
    for (int j = i + 1; j < ALL; ++j) {
      if (cur[i][j]) {
        glBegin(GL_LINES);
          glVertex2f(point(i).y, 1.0 - point(i).x);
          glVertex2f(point(j).y, 1.0 - point(j).x);
        glEnd();
      }
    }
  }

  glPopMatrix();
	glutSwapBuffers();
}

void save(const string& s) {
  ofstream out(s);
  out << SZ << endl;
  for (int i = 0; i < ALL; ++i) {
    for (int j = 0; j < ALL; ++j) {
      out << cur[i][j] << " ";
    }
    out << endl;
  }
}

long long k = 0;
const long long P = 1e9 + 9;

void next(vvb& cur, vvi& got, long long& cure) {
  int i, j;
  if (rand() % 2) {
    i = k % ALL; //rand() % ALL;
    j = k / ALL; //rand() % ALL;
    k = (k + P) % (ALL * ALL);
  } else {
    i = rand() % ALL;
    j = rand() % ALL;
  }

  if (i > j) {
    swap(i, j);
  }
  cur[i][j] = (cur[i][j] ^ true);

  int kk = -1;
  if (cur[i][j]) {
    kk = 1;
  }

  vector<pair<int, int> > changed;
  vector<int> was;
  point a(i);
  point b(j);
  pair<int, int> last = {-1, -1};
  for (double k = 0; k < (b - a).len(); k += STEP) {
    point cur = a + (b - a).norm(k);
    if (cur.x < 1.0 && cur.y < 1.0 && cur.x >= 0 && cur.y >= 0) {
      pair<int, int> p = {int(cur.x * DIM), int(cur.y * DIM)};
      if (last != p) {
        changed.push_back(p);
        was.push_back(got[p.first][p.second]);
      }
      got[p.first][p.second] += kk;
      last = p;
    }
  }

  long long res = cure;
  for (int id = 0; id < changed.size(); ++id) {
    auto p = changed[id];
    int i = p.first;
    int j = p.second;
    long long x_was = max(0, abs(255 - was[id] * ROPE - int(image.at<uint8_t>(i, j))));

    long long x_now = max(0, abs(255 - (got[i][j]) * ROPE - int(image.at<uint8_t>(i, j))));

    long long coef = int(K * (point(0.5, 0.5) - point(1.0 * i / DIM, 1.0 * j / DIM)).len());
    coef *= coef;
    coef *= coef;
    coef *= coef;
    x_now *= x_now * (4ll * K * K * K * K - coef);
    x_was *= x_was * (4ll * K * K * K * K - coef);

    res += x_now - x_was;
  }

  if (res > cure) {
    for (int id = 0; id < changed.size(); ++id) {
      got[changed[id].first][changed[id].second] = was[id];
    }
    cur[i][j] = (cur[i][j] ^ true);
  } else {
    cure = res;
  }
}

int iter = 0;

void Timer(int value) {
  cerr << iter++ << "000 " << cure << endl;
  for (int i = 0; i < 1000; ++i) {
    next(cur, cur_got, cure);
  }
  Draw();
  //fitness();
	glutTimerFunc(50, Timer, 1);
}

void Initialize() {
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, 1, 0, 1, -1, 1);
	glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char* argv[]) {
  srand(time(NULL));
  Mat I = imread(argv[1], 1);
  Mat im = I.clone();
  resize(I, im, cv::Size(DIM, DIM), 0, 0, INTER_LINEAR);
  cvtColor(im, im, cv::COLOR_BGR2GRAY);
  image = im.clone();
  bilateralFilter(im, image, 3, 75, 75);
  im = image.clone();

  int mx = -1;
  int mn = 1000;
  for (int i = 0; i < DIM; ++i) {
    for (int j = 0; j < DIM; ++j) {
      mn = min(mn, int(image.at<uint8_t>(i, j)));
      mx = max(mx, int(image.at<uint8_t>(i, j)));
    }
  }
  for (int i = 0; i < DIM; ++i) {
    for (int j = 0; j < DIM; ++j) {
      image.at<uint8_t>(i, j) = int(image.at<uint8_t>(i, j) - mn) * (150) / (mx) + 50;
    }
  }

  namedWindow( "Display window", WINDOW_AUTOSIZE );// Create a window for display.
  imshow( "Display window", image);                   // Show our image inside it.
  waitKey(0);

  cur = vvb(ALL, vector<bool>(ALL, 0));
  fitness();

  glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB);
	glutInitWindowSize(WinWid, WinHei);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("float");
	glutDisplayFunc(Draw);
	glutTimerFunc(50, Timer, 0);//Анимация
	Initialize();
	glutMainLoop();
}
