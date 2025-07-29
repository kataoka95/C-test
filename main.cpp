#define NOMINMAX
#include <Windows.h>
#include <windowsx.h>

#include <cmath>
#include <cstdlib>
#include <string>

struct Target {
  // 0=rectangle: 1=ellipse
  int type;
  int size;
  int x;
  int y;
};

class Model {
 public:
  Model() {
    SetTarget();
    score_ = 0;
  }
  void Clicked(int x, int y) {
    if (target_.type == 0) {
      if (target_.x - target_.size < x && x < target_.x + target_.size &&
          target_.y - target_.size < y && y < target_.y + target_.size) {
        ++score_;
        SetTarget();
      } else {
        --score_;
      }
    } else {
      double dx = static_cast<double>(x - target_.x);
      double dy = static_cast<double>(y - target_.y);
      if (std::sqrt(dx * dx + dy * dy) < target_.size) {
        ++score_;
        SetTarget();
      } else {
        --score_;
      }
    }
  }
  Target target() const { return target_; }
  int score() const { return score_; }
 private:
  void SetTarget() {
    target_.type = std::rand() % 2;
    target_.size = std::rand() % 10 + 40;
    target_.x = std::rand() % 500 + 50;
    target_.y = std::rand() % 300 + 50;
  }
  Target target_;
  int score_;
};

class View {
 public:
  void Paint(HDC hdc, Model& model) const {
    Target target = model.target();
    if (target.type == 0) {
      Rectangle(hdc, target.x - target.size, target.y - target.size,
                target.x + target.size, target.y + target.size);
    } else {
      Ellipse(hdc, target.x - target.size, target.y - target.size,
                target.x + target.size, target.y + target.size);
    }
    std::wstring text = L"Score:" + std::to_wstring(model.score());
    TextOutW(hdc, 0, 0, text.c_str(), text.length());
  }
};

class Controller {
 public:
  bool ProcessMessage(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    if (msg == WM_PAINT) {
      PAINTSTRUCT ps;
      HDC hdc = BeginPaint(hwnd, &ps);
      view_.Paint(hdc, model_);
      EndPaint(hwnd, &ps);
      return true;
    } else if (msg == WM_LBUTTONUP) {
      model_.Clicked(GET_X_LPARAM(lp), GET_Y_LPARAM(lp));
      InvalidateRect(hwnd, nullptr, TRUE);
      return true;
    }
    return false;
  }
 private:
  Model model_;
  View view_;
};

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
  static Controller controller;
  if (controller.ProcessMessage(hwnd, msg, wp, lp)) return 0;
  
  if (msg == WM_DESTROY) {
    PostQuitMessage(0);
    return 0;
  } else {
    return DefWindowProcW(hwnd, msg, wp, lp);
  }
}

BOOL RegisterApplication(const wchar_t* window_id, WNDPROC window_proc) {
  WNDCLASSEXW wc;
  wc.cbSize = sizeof(WNDCLASSEXW);
  wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
  wc.lpfnWndProc = window_proc;
  wc.cbClsExtra = 0;
  wc.cbWndExtra = 0;
  wc.hInstance = GetModuleHandleW(NULL);
  wc.hIcon = (HICON)LoadIconW(NULL, IDI_APPLICATION);
  wc.hCursor = (HCURSOR)LoadCursorW(NULL, IDC_ARROW);
  wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
  wc.lpszMenuName = NULL;
  wc.lpszClassName = window_id;
  wc.hIconSm = (HICON)LoadImageW(NULL, IDI_APPLICATION, IMAGE_ICON, 0, 0,
                                 LR_DEFAULTSIZE | LR_SHARED);
  return (RegisterClassExW(&wc));
}

int main(void) {
  const wchar_t* windowid = L"MyFirstApp";
  if (!RegisterApplication(windowid, WndProc)) return 1;
  HWND hwnd = CreateWindowW(windowid, L"Title", WS_OVERLAPPEDWINDOW, 0, 0, 600,
                            400, NULL, NULL, GetModuleHandleW(NULL), NULL);

  ShowWindow(hwnd, SW_SHOW);
  UpdateWindow(hwnd);

  MSG msg;
  BOOL value;
  while (true) {
    value = GetMessageW(&msg, NULL, 0, 0);
    if (value == 0) break;
    if (value == -1) return 1;

    TranslateMessage(&msg);
    DispatchMessageW(&msg);
  }

  return msg.wParam;
}
