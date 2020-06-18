#include "rope.hpp"

using vvb = vector<vector<bool> >;

const int WinHei = 1000;
const int WinWid = 1000;

const double EPS = 1e-6;
const double SLOWNESS = 0.9999;
const int INF = 1e9;
const int SZ = 50;

const int DIM = 100;
const int ALL = SZ * 4;
const int ROPE = 1;
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

double fitness(const vvb& pic, const Mat& im) { // const Mat<CV_8UC1>& im
  vector<vector<int> > got(DIM, vector<int>(DIM, 0));
  for (int i = 0; i < ALL; ++i) {
    for (int j = i + 1; j < ALL; ++j) {
      if (pic[i][j]) {
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
  double res = 0;
  for (int i = 0; i < DIM; ++i) {
    for (int j = 0; j < DIM; ++j) {
      double x = max(0.0, (double(abs(255 - got[i][j] * ROPE - int(im.at<uint8_t>(i, j)))))
        * (2.0 - (point(0.5, 0.5) - point(double(i) / DIM, double(j) / DIM)).len()));
      res += x;
    }
  }
  return res;
}

vvb next(vvb cur, double t) {
  int cnt = min(ALL * ALL / 20, max(int(ALL * ALL * (1e3 * t)), 1));
  while (cnt--) {
    int i = rand() % ALL;
    int j = rand() % ALL;
    if (i > j) {
      swap(i, j);
    }
    cur[i][j] = (cur[i][j] ^ true);
  }
  return cur;
}

vvb cur, opt;
double cure;
double opte;
double t;
Mat res;

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

  glColor4f(0, 0, 0, 0.5);
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
      out << opt[i][j] << " ";
    }
    out << endl;
  }
}

void Timer(int value) {
  cerr << t << " " << cure << " " << opte << endl;
  for (int i = 0; i < 100; ++i) {
    vvb nxt = next(cur, t);
    double nxte = fitness(nxt, res);
    if (nxte < cure || (exp((opte - nxte) / max(1e-5, t)) > double(rand() % 1000) / 1000.0)) {
      cure = nxte;
      cur = nxt;
    }
    if (opte > cure) {
      opt = cur;
      opte = cure;
    }
    t *= SLOWNESS;
  }
  save("table.save");
  Draw();
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
  res = im.clone();
  bilateralFilter(im, res, 3, 75, 75);
  im = res.clone();

  /*for (int i = 0; i < DIM; ++i) {
    for (int j = 0; j < DIM; ++j) {
      int sum = 0;
      for (int dx = -1; dx <= 1; ++dx) {
        for (int dy = -1; dy <= 1; ++dy) {
          if (abs(dx) + abs(dy) != 0) {
            if (0 <= i + dx && i + dx < DIM &&
                0 <= j + dy && j + dy < DIM) {
                  sum -= im.at<uint8_t>(i + dx, j + dy);
            } else {
              sum -= int(im.at<uint8_t>(i, j));
            }
          } else if (dx == 0 && dy == 0) {
            sum += 8 * int(im.at<uint8_t>(i, j));
          }
        }
      }
      res.at<uint8_t>(i, j) = 100 - min(90, abs(sum));
    }
  }*/

  namedWindow( "Display window", WINDOW_AUTOSIZE );// Create a window for display.
  imshow( "Display window", res);                   // Show our image inside it.
  waitKey(0);

  cur = vvb(ALL, vector<bool>(ALL, 0));
  opt = vvb(ALL, vector<bool>(ALL, 0));
  cur = next(cur, 1);
  opt = cur;
  cure = fitness(cur, res);
  opte = cure;
  t = 1e-5;

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
