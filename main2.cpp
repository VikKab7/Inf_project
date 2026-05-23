std::string line1 = "GAME OVER   " + winner;
        std::string line2 = "Final score — P1: " + std::to_string(score1) +
                            "   P2: " + std::to_string(score2);

        SetTextColor(hdc, RGB(255, 80, 80));
        TextOutA(hdc,
            W * CELL / 2 - (int)line1.size() * 4,
            H * CELL / 2 - 12,
            line1.c_str(), (int)line1.size());

        SetTextColor(hdc, RGB(200, 200, 200));
        TextOutA(hdc,
            W * CELL / 2 - (int)line2.size() * 4,
            H * CELL / 2 + 10,
            line2.c_str(), (int)line2.size());
    }
}

// ================= WINDOW =================

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM w, LPARAM l) {
    switch (msg) {

    case WM_CREATE:
        srand((unsigned)time(0));
        InitGame();
        SetTimer(hwnd, 1, 180, NULL);
        return 0;

    case WM_KEYDOWN:
        if (!gameOver) {
            if (w == 'A') s1.dir = LEFT;
            if (w == 'D') s1.dir = RIGHT;
            if (w == 'W') s1.dir = UP;
            if (w == 'S') s1.dir = DOWN;

            if (w == VK_LEFT)  s2.dir = LEFT;
            if (w == VK_RIGHT) s2.dir = RIGHT;
            if (w == VK_UP)    s2.dir = UP;
            if (w == VK_DOWN)  s2.dir = DOWN;
        }
        return 0;

    case WM_TIMER: {
        // Если игра окончена — ничего не делаем, таймер убит
        if (gameOver)
            return 0;

        // Движение
        s1.Move();
        s2.Move();

        Point h1 = s1.Head();
        Point h2 = s2.Head();

        // Проверка границ
        auto CheckBorder = [&](Snake &s) {
            Point h = s.Head();
            if (h.x < 0  h.x >= W  h.y < 0 || h.y >= H)
                s.alive = false;
        };

        CheckBorder(s1);
        CheckBorder(s2);

        // Стены
        if (IsWall(h1)) s1.alive = false;
        if (IsWall(h2)) s2.alive = false;

        // Столкновение с собой
        if (s1.HitSelf()) s1.alive = false;
        if (s2.HitSelf()) s2.alive = false;

        // Столкновение змеек друг с другом
        if (HitSnake(s1, h2)) s2.alive = false;
        if (HitSnake(s2, h1)) s1.alive = false;

        // Еда
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

        // Проверка смерти
        if (!s1.alive || !s2.alive) {
            gameOver = true;
            KillTimer(hwnd, 1);  // останавливаем таймер — игра не перезапустится
        }

        InvalidateRect(hwnd, NULL, FALSE);
        return 0;
    }

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        Draw(hdc);
        EndPaint(hwnd, &ps);
        return 0;
    }

    case WM_DESTROY:
        KillTimer(hwnd, 1);
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hwnd, msg, w, l);
}

// ================= MAIN =================

int WINAPI WinMain(HINSTANCE hI, HINSTANCE, LPSTR, int nCmdShow) {

    const char CLASS_NAME[] = "SnakePvP";

    WNDCLASS wc = {};
    wc.lpfnWndProc   = WndProc;
    wc.hInstance     = hI;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        "Snake PvP",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        W * CELL + 16,
        H * CELL + 39,
        NULL, NULL, hI, NULL
    );

    ShowWindow(hwnd, nCmdShow);

    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
SetTextColor(hdc, RGB(200, 200, 200));
        TextOutA(hdc, W*CELL/2 - (int)line2.size()*4, H*CELL/2 + 10,
                 line2.c_str(), (int)line2.size());
    }
}

// ================= WINDOW =================
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM w, LPARAM l) {
    switch (msg) {

    case WM_CREATE:
        srand((unsigned)time(0));
        InitGame();                  // игра стартует только здесь, один раз
        SetTimer(hwnd, 1, 180, NULL);
        return 0;

    case WM_KEYDOWN:
        if (!gameOver) {             // после смерти управление игнорируется
            if (w == 'A') s1.dir = LEFT;
            if (w == 'D') s1.dir = RIGHT;
            if (w == 'W') s1.dir = UP;
            if (w == 'S') s1.dir = DOWN;
            if (w == VK_LEFT)  s2.dir = LEFT;
            if (w == VK_RIGHT) s2.dir = RIGHT;
            if (w == VK_UP)    s2.dir = UP;
            if (w == VK_DOWN)  s2.dir = DOWN;
        }
        return 0;

    case WM_TIMER: {
        if (gameOver) return 0;      // подстраховка: после смерти ничего не делаем

        s1.Move();
        s2.Move();

        Point h1 = s1.Head();
        Point h2 = s2.Head();

        auto CheckBorder = [&](Snake &s) {
            Point h = s.Head();
            if (h.x < 0  h.x >= W  h.y < 0  h.y >= H)  // вернул  
                s.alive = false;
        };
        CheckBorder(s1);
        CheckBorder(s2);

        if (IsWall(h1)) s1.alive = false;
        if (IsWall(h2)) s2.alive = false;

        if (s1.HitSelf()) s1.alive = false;
        if (s2.HitSelf()) s2.alive = false;

        if (HitSnake(s1, h2)) s2.alive = false;
        if (HitSnake(s2, h1)) s1.alive = false;

        if (h1.x == food.x && h1.y == food.y) { s1.grow = true; score1 += 10; SpawnFood(); }
        if (h2.x == food.x && h2.y == food.y) { s2.grow = true; score2 += 10; SpawnFood(); }

        // Кто-то умер -> полностью останавливаем игру
        if (!s1.alive || !s2.alive) {
            gameOver = true;
            KillTimer(hwnd, 1);      // стоп. Нет авто-рестарта, окно двигается свободно
        }

        InvalidateRect(hwnd, NULL, FALSE);
        return 0;
    }

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        Draw(hdc);
        EndPaint(hwnd, &ps);
        return 0;
    }

    case WM_DESTROY:
        KillTimer(hwnd, 1);
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hwnd, msg, w, l);
}

// ================= MAIN =================
int WINAPI WinMain(HINSTANCE hI, HINSTANCE, LPSTR, int nCmdShow) {
    const char CLASS_NAME[] = "SnakePvP";

    WNDCLASS wc = {};
    wc.lpfnWndProc   = WndProc;
    wc.hInstance     = hI;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0, CLASS_NAME, "Snake PvP", WS_OVER LAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        W * CELL + 16, H * CELL + 39,
        NULL, NULL, hI, NULL
    );

    ShowWindow(hwnd, nCmdShow);

    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}