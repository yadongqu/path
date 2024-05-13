#include <MiniFB.h>
#include <stdint.h>
#include <stdlib.h>
int main() {
  struct mfb_window *window = mfb_open_ex("my display", 800, 600, WF_RESIZABLE);
  if (!window)
    return 0;

  uint32_t *buffer = (uint32_t *)malloc(800 * 600 * 4);

  do {
    int state;

    // TODO: add some fancy rendering to the buffer of size 800 * 600

    state = mfb_update_ex(window, buffer, 800, 600);

    if (state < 0) {
      window = NULL;
      break;
    }
  } while (mfb_wait_sync(window));

  free(buffer);
}