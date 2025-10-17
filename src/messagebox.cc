
#if defined(HAVE_QT) && ! defined(_WIN32)
#include <QApplication>
#include <qmessagebox.h>

void MessageBox(void*, const char* title, const char* message, int flags) {
  QMessageBox msgBox(QMessageBox::Information, message, title);
  msgBox.exec();
}

#else
#include <windows.h>
#endif

int main(int argc, char **argv) {
#if defined(HAVE_QT) && ! defined(_WIN32)
  QApplication app(argc, argv);
#endif
#if defined(_WIN32)
  wchar_t wtextA[200];
  wchar_t wtextB[200];
  mbstowcs(wtextA, argv[1], strlen(argv[1]));
  mbstowcs(wtextB, argv[2], strlen(argv[2]));
  LPWSTR ptrA = wtextA;
  LPWSTR ptrB = wtextB;
#endif

  if (argc == 3) {
#if defined(_WIN32)
    MessageBox(NULL, ptrA, ptrB, 0x20);
#else
    MessageBox(NULL, argv[1], argv[2], 0x20);
#endif

  } else {
#if defined(_WIN32)
    MessageBox(NULL,  L"Usage", L"Usage:  \n\nmessagebox title message", 0x20);
#else
    MessageBox(NULL,  "Usage", "Usage:  \n\nmessagebox title message", 0x20);
#endif
  }
  return 0;
}
