#include <iostream>
#include <arv.h>

using namespace std;

#define GENICAM_FILENAME "genicam.xml"

int main(int argc, char** argv)
{
  arv_enable_interface("Fake");

  arv_update_device_list();

  arv_set_fake_camera_genicam_filename(GENICAM_FILENAME);

  arv_shutdown();

  cout << "Done." << endl;

  return 0;
}
