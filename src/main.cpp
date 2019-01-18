#include <iostream>
#include <arv.h>

int main(int argc, char** argv)
{
  arv_enable_interface("Fake");

  arv_update_device_list();

  arv_shutdown();

  std::cout << "Done." << std::endl;

  return 0;
}
