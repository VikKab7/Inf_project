#include <windows.h>
#include <vector>
#include <ctime>
#include <string>

const int CELL = 20;
const int W = 25;
const int H = 20;

struct Point {
    int x, y;
};

enum Dir {
    LEFT,
    RIGHT,
    UP,
    DOWN
};

class Snake {
public:
    std::vector<Point> body;
    Dir dir = RIGHT;
    bool grow = false;
    bool alive = true;

    Snake(Point start) {
        body = {
            start,
            {start.x - 1, start.y},
            {start.x - 2, start.y}
        };
    }

    void Move() {
        if (!alive) return;

        Point h = body[0];

        switch (dir) {
        case LEFT:  h.x--; break;
        case RIGHT: h.x++; break;
        case UP:    h.y--; break;
        case DOWN:  h.y++; break;
        }

        body.insert(body.begin(), h);

        if (!grow)
            body.pop_back();
        else
            grow = false;
    }

    Point Head() {
        return body[0];
    }

    bool HitSelf() {
        Point h = body[0];

        for (size_t i = 1; i < body.size(); i++) {
            if (body[i].x == h.x && body[i].y == h.y)
                return true;
        }

        return false;
    }
};

Snake s1({5, 5});
Snake s2({18, 12});

Point food;
std::vector<Point> walls;

HWND hwnd;

int score1 = 0;
int score2 = 0;

void SpawnFood() {
    food = { rand() % W, rand() % H };
}

void SpawnWalls() {
    walls.clear();

    walls.push_back({10, 5});
    walls.push_back({10, 6});
    walls.push_back({10, 7});

    walls.push_back({15, 10});
    walls.push_back({16, 10});
    walls.push_back({17, 10});
}

bool IsWall(Point p) {
    for (auto &w : walls) {
        if (w.x == p.x && w.y == p.y)
            return true;
    }

    return false;
}

bool HitSnake(Snake &s, Point p) {
    for (auto &b : s.body) {
        if (b.x == p.x && b.y == p.y)
            return true;
    }

    return false;
}

void ResetGame() {

    s1 = Snake({5, 5});
    s2 = Snake({18, 12});

    score1 = 0;
    score2 = 0;

    SpawnFood();
    SpawnWalls();
}

void Draw(HDC hdc) {

    RECT r = {0, 0, W * CELL, H * CELL};

    HBRUSH bg = CreateSolidBrush(RGB(25,25,25));
    FillRect(hdc, &r, bg);
    DeleteObject(bg);

    // FOOD

    HBRUSH foodBrush = CreateSolidBrush(RGB(255,0,0));

    RECT fr = {
        food.x * CELL,
        food.y * CELL,
        (food.x + 1) * CELL,
        (food.y + 1) * CELL
    };

    FillRect(hdc, &fr, foodBrush);

    DeleteObject(foodBrush);

    // WALLS

    HBRUSH wallBrush = CreateSolidBrush(RGB(255,255,0));

    for (auto &p : walls) {

        RECT wr = {
            p.x * CELL,
            p.y * CELL,
            (p.x + 1) * CELL,
            (p.y + 1) * CELL
        };

        FillRect(hdc, &wr, wallBrush);

        FrameRect(
            hdc,
            &wr,
            (HBRUSH)GetStockObject(BLACK_BRUSH)
        );
    }

    DeleteObject(wallBrush);

    // SNAKE 1

    HBRUSH green = CreateSolidBrush(RGB(0,255,0));

    for (auto &p : s1.body) {

        RECT sr = {
            p.x * CELL,
            p.y * CELL,
            (p.x + 1) * CELL,
            (p.y + 1) * CELL
        };

        FillRect(hdc, &sr, green);
    }

    DeleteObject(green);

    // SNAKE 2

    HBRUSH blue = CreateSolidBrush(RGB(0,150,255));

    for (auto &p : s2.body) {

        RECT sr = {
            p.x * CELL,
            p.y * CELL,
            (p.x + 1) * CELL,
            (p.y + 1) * CELL
        };

        FillRect(hdc, &sr, blue);
    }

    DeleteObject(blue);

    // TEXT

    std::string text =
        "Player1: " + std::to_string(score1) +
        "   Player2: " + std::to_string(score2);

    TextOutA(
        hdc,
        10,
        10,
        text.c_str(),
        text.size()
    );
}

LRESULT CALLBACK WndProc(HWND h, UINT m, WPARAM w, LPARAM l) {

    switch (m) {

    case WM_CREATE:
    {
        hwnd = h;

        srand((unsigned)time(0));

        ResetGame();

        // МЕДЛЕННЕЕ СКОРОСТЬ
        SetTimer(h, 1, 180, NULL);

        return 0;
    }

    case WM_KEYDOWN:
    {
        // PLAYER 1

        if (w == 'A') s1.dir = LEFT;
        if (w == 'D') s1.dir = RIGHT;
        if (w == 'W') s1.dir = UP;
        if (w == 'S') s1.dir = DOWN;

        // PLAYER 2

        if (w == VK_LEFT)  s2.dir = LEFT;
        if (w == VK_RIGHT) s2.dir = RIGHT;
        if (w == VK_UP)    s2.dir = UP;
        if (w == VK_DOWN)  s2.dir = DOWN;

        return 0;
    }

    case WM_TIMER:
    {
        s1.Move();
        s2.Move();

        Point h1 = s1.Head();
        Point h2 = s2.Head();

        // ГРАНИЦЫ

        auto CheckBorder = [&](Snake &s) {

            Point h = s.Head();

            if (h.x < 0 || h.x >= W ||
                h.y < 0 || h.y >= H)
            {
                s.alive = false;
            }
        };

        CheckBorder(s1);
        CheckBorder(s2);

        // СТЕНЫ

        if (IsWall(h1)) s1.alive = false;
        if (IsWall(h2)) s2.alive = false;

        // САМОУКУС

        if (s1.HitSelf()) s1.alive = false;
        if (s2.HitSelf()) s2.alive = false;

        // СТОЛКНОВЕНИЕ МЕЖДУ ИГРОКАМИ

        if (HitSnake(s1, h2))
            s2.alive = false;

        if (HitSnake(s2, h1))
            s1.alive = false;

        // ЕДА

        if (h1.x == food.x && h1.y == food.y) {
            s1.grow = true;
            score1 += 10;
            SpawnFood();
        }

        if (h2.x == food.x && h2.y == food.y) {
            s2.grow = true;
            score2 += 10;
            SpawnFood();
        }

        // АВТОРЕСТАРТ

        if (!s1.alive || !s2.alive) {

            Sleep(1500);

            ResetGame();
        }

        InvalidateRect(h, NULL, TRUE);

        return 0;
    }

    case WM_PAINT:
    {
        PAINTSTRUCT ps;

        HDC hdc = BeginPaint(h, &ps);

        Draw(hdc);

        EndPaint(h, &ps);

        return 0;
    }

    case WM_DESTROY:
    {
        PostQuitMessage(0);
        return 0;
    }
    }

    return DefWindowProc(h, m, w, l);
}

int WINAPI WinMain(
    HINSTANCE hI,
    HINSTANCE,
    LPSTR,
    int nCmdShow
) {

    const char CLASS_NAME[] = "SnakePvP";

    WNDCLASS wc = {};

    wc.lpfnWndProc = WndProc;
    wc.hInstance = hI;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    HWND h = CreateWindowEx(
        0,
        CLASS_NAME,
        "Snake PvP",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        W * CELL + 16,
        H * CELL + 39,
        NULL,
        NULL,
        hI,
        NULL
    );

    ShowWindow(h, nCmdShow);

    MSG msg = {};

    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
} 