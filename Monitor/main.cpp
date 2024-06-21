#include <Monitor.h>

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::printf("Usage:\n\t%s Monitor.ini\n\n", argv[0]);
    return 0;
  }

  Monitor monitor(argv[1]);
  monitor.Start();

  return 0;
}
