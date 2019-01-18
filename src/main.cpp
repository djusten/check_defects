#include <vector>
#include <memory>
#include <fstream>

#include "camera.h"

int main(int argc, char** argv)
{
  int index = 0;

  arv_enable_interface("Fake");

  auto cam = new Camera();

  if (!cam->open(index)) {
    printf("Error\n");
  } else {

    std::vector<char> data = cam->getFramebuffer();

    cam->save_to_file("output.pgm", data);

    cam->close();
  }

  arv_shutdown();

  return 0;
}
