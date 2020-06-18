#include "rope.hpp"

using vvb = vector<vector<bool> >;

const int WinHei = 1000;
const int WinWid = 1000;

const double EPS = 1e-6;
const double SLOWNESS = 0.99999;
const int INF = 1e9;
const int SZ = 10;

const int ALL = SZ * 4;
const int ROPE = max(1, 100 / SZ);
const double STEP = 0.1;

class point {
  public:
    int x, y;

    point(int xx, int yy) {
      x = xx;
      y = yy;
    }

    point(int id) {
      assert(0 <= id && id < ALL);
      if (id < SZ) {
        x = id;
        y = 0;
      } else if (id < SZ * 2) {
        x = SZ;
        y = id - SZ;
      } else if (id < SZ * 3) {
        x = SZ - (id - SZ * 2);
        y = SZ;
      } else {
        x = 0;
        y = SZ - (id - SZ * 3);
      }
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
      return point(int((double(x)) * k), int((double(y)) * k));
    }

    double len() const {
      return sqrt(x * x + y * y);
    }

    bool operator==(const point& other) const {
      return (x == other.x) && (y == other.y);
    }

    bool operator!=(const point& other) const {
      return (x != other.x) || (y != other.y);
    }
};

double fitness(const vvb& pic, const Mat& im) { // const Mat<CV_8uC1>& im
  vector<vector<int> > got(SZ, vector<int>(SZ, 0));
  for (int i = 0; i < ALL; ++i) {
    //cerr << "     i: " << i << endl;
    assert(pic.size() == ALL && pic[i].size() == ALL);
    for (int j = i + 1; j < ALL; ++j) {
      //cerr << "       j: " << j << endl;
      if (pic[i][j]) {
        point a(i);
        point b(j);
        point last(-1, -1);
        for (double k = 0; k < (b - a).len(); k += STEP) {
          point cur = a + (b - a) * k;
          if (cur != last && cur.x < SZ && cur.y < SZ && cur.x >= 0 && cur.y >= 0) {
            got[cur.x][cur.y]++;
          }
          last = cur;
        }
      }
    }
  }
  //cerr << "{" << endl;
  double res = 0;
  for (int i = 0; i < im.rows; ++i) {
    for (int j = 0; j < im.cols; ++j) {
      res += double(abs(255 - got[i][j] * ROPE - int(im.at<uint8_t>(i, j))))
        * (double(2 * SZ) - (point(SZ / 2, SZ / 2) - point(i, j)).len())
      ;
    }
  }
  //cerr << "}" << endl;
  return res;
}

void showPic(const vvb& pic) {
  Mat got(SZ, SZ, CV_8UC1);
  for (int i = 0; i < got.rows; ++i) {
    for (int j = 0; j < got.cols; ++j) {
      got.at<uint8_t>(i, j) = 255;
    }
  }
  for (int i = 0; i < ALL; ++i) {
    for (int j = i + 1; j < ALL; ++j) {
      if (pic[i][j]) {
        point a(i);
        point b(j);
        point last(-1, -1);
        for (int k = 0; k <= (b - a).len(); ++k) {
          point cur = a + (b - a) * (double(k) / (b - a).len());
          if (cur != last && cur.x < SZ && cur.y < SZ && cur.x >= 0 && cur.y >= 0) {
            got.at<uint8_t>(cur.x, cur.y) = max(0, int(got.at<uint8_t>(cur.x, cur.y)) - ROPE);
          }
          last = cur;
        }
      }
    }
  }
  namedWindow("Display Image", WINDOW_AUTOSIZE);
  imshow("Display Image", got);
  waitKey(0);
}

vvb next(vvb cur, double t) {
  int cnt = min(ALL * ALL, max(int(ALL * ALL * (1e3 * t)), 1));
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

// vvb burn(const Mat& im) {
//   vvb cur(ALL, vector<bool>(ALL, 0)), opt(ALL, vector<bool>(ALL, 0));
//   int cure = fitness(cur, im);
//   int opte = cure;
//   double t = 1;
//   while (t > EPS) {
//     cerr << t << " " << cure << " " << opte << endl;
//     vvb nxt = next(cur, t);
//     double nxte = fitness(nxt, im);
//     if (nxte < cure || (exp((opte - nxte) / t) > double(rand() % 1000) / 1000.0)) {
//       cure = nxte;
//       cur = nxt;
//     }
//     if (opte > cure) {
//       opt = cur;
//       opte = cure;
//     }
//     t *= SLOWNESS;
//   }
//   showPic(opt);
//   return opt;
// }

vvb cur, opt;
double cure;
double opte;
double t;
Mat res;

void Draw() {
	glClear(GL_COLOR_BUFFER_BIT);
	glPushMatrix();


  Mat got(SZ, SZ, CV_8UC1);
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
        point last(-1, -1);
        for (double k = 0; k < (b - a).len(); k += STEP) {
          point cur = a + (b - a) * k;
          if (cur != last && cur.x < SZ && cur.y < SZ && cur.x >= 0 && cur.y >= 0) {
            got.at<uint8_t>(cur.x, cur.y) = max(0, int(got.at<uint8_t>(cur.x, cur.y)) - ROPE);
          }
          last = cur;
        }
      }
    }
  }

  for (int i = 0; i < SZ; ++i) {
    for (int j = 0; j < SZ; ++j) {
      double c = double(got.at<uint8_t>(i, j)) / 255;
      glColor4f(c, c, c, 0);
      glBegin(GL_QUADS);
        glVertex2f(j, SZ - i);
        glVertex2f(j + 1, SZ - i);
        glVertex2f(j + 1, SZ - i - 1);
        glVertex2f(j, SZ - i - 1);
      glEnd();
    }
  }

  glColor4f(0, 0, 0, 0.5);
  for (int i = 0; i < ALL; ++i) {
    for (int j = i + 1; j < ALL; ++j) {
      if (cur[i][j]) {
        glBegin(GL_LINES);
          glVertex2f(point(i).y, SZ - point(i).x);
          glVertex2f(point(j).y, SZ - point(j).x);
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
	glOrtho(0, SZ, 0, SZ, -1, 1);
	glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char* argv[]) {
  srand(time(NULL));
  Mat I = imread(argv[1], 1);
  res = Mat(SZ, SZ, CV_8UC1);
  for (int i = 0; i < SZ; ++i) {
    for (int j = 0; j < SZ; ++j) {
      int sum = 0;
      int cnt = 0;
      for (int x = i * (I.rows / SZ); x < min((i + 1) * (I.rows / SZ), I.rows); ++x) {
        for (int y = j * (I.cols / SZ); y < min((j + 1) * (I.cols / SZ), I.cols); ++y) {
          cnt++;
          sum += (int(I.at<Vec3b>(x, y)[0]) + int(I.at<Vec3b>(x, y)[1]) + int(I.at<Vec3b>(x, y)[2])) / 3;
        }
      }
      res.at<uint8_t>(i, j) = sum / cnt;
    }
  }

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
