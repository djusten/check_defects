#include <vector>
#include <memory>
#include <fstream>

#include <arv.h>
#include "camera.h"

Camera::Camera()
{
  _camera = nullptr;
  _stream = nullptr;
}

void Camera::close()
{
  if(_camera) {
    stopCapture();
  }
}

std::string Camera::getDeviceNameById(int id)
{
  arv_update_device_list();

  if((id >= 0) && (id < (int)arv_get_n_devices())) {
    return arv_get_device_id(id);
  }

  return {};
}

bool Camera::create(int index)
{
  std::string deviceName = Camera::getDeviceNameById(index);
  if (deviceName.size() > 0) {
    _camera.reset(arv_camera_new(deviceName.c_str()));
    return true;
  }

  return false;
}

bool Camera::initBuffers()
{
  _stream.reset(arv_camera_create_stream(_camera.get(), NULL, NULL));
  if (_stream) {
    _payload = arv_camera_get_payload (_camera.get());

    arv_stream_push_buffer(_stream.get(), arv_buffer_new(_payload, NULL));

    return true;
  }

  return false;
}

bool Camera::open(int index)
{
  if(create(index)) {
    return startCapture();
  }
  return false;
}

std::vector<char> Camera::getFramebuffer()
{
  char *_framebuffer = NULL;

  if(_stream) {
    ArvBuffer *arv_buffer = NULL;
    int max_tries = 10;
    int tries = 0;
    for(; tries < max_tries; tries ++) {
      arv_buffer = arv_stream_timeout_pop_buffer(_stream.get(), 200000);
      if (arv_buffer != NULL && arv_buffer_get_status(arv_buffer) != ARV_BUFFER_STATUS_SUCCESS) {
        arv_stream_push_buffer(_stream.get(), arv_buffer);
      } else break;
    }
    if(arv_buffer != NULL && tries < max_tries) {
      size_t buffer_size;
      _framebuffer = (char *)arv_buffer_get_data(arv_buffer, &buffer_size);

      arv_buffer_get_image_region(arv_buffer, NULL, NULL, &_width, &_height);

      std::vector<char> raw_data;

      raw_data.assign(_framebuffer, _framebuffer + buffer_size);

      arv_stream_push_buffer(_stream.get(), arv_buffer);
      return raw_data;
    }
  }

  return {};
}

void Camera::stopCapture()
{
  arv_camera_stop_acquisition(_camera.get());
}

bool Camera::startCapture()
{
  if(initBuffers() ) {
    arv_camera_set_acquisition_mode(_camera.get(), ARV_ACQUISITION_MODE_CONTINUOUS);
    arv_camera_start_acquisition(_camera.get());

    return true;
  }
  return false;
}

void Camera::save_to_file(std::string filename, std::vector<char> data)
{
  gchar *head;
  int max_color_component = 255;
  std::ofstream output_file(filename);

  head = g_strdup_printf ("P5\n %d\n %d\n %d\n", _height, _width, max_color_component);

  output_file << head;

  for (auto raw : data)
    output_file << raw;

  output_file.close();
  g_free(head);
}

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
