#include <iostream>
#include <arv.h>

using namespace std;

#define GENICAM_FILENAME "/home/diogo/work/check_defects/src/arv-fake-camera.xml"

static void
fill_pattern_cb (ArvBuffer *buffer, void *fill_pattern_data, guint32 exposure_time_us, guint32 gain, ArvPixelFormat pixel_format)
{
  gint *counter = (gint *)fill_pattern_data;

  (*counter)++;
}

int main(int argc, char** argv)
{
  ArvCamera *camera;
  ArvDevice *device;
  ArvFakeCamera *fake_camera;
  ArvStream *stream;
  ArvBuffer *buffer;
  gint payload;
  gint counter = 0;

  arv_enable_interface("Fake");

  arv_update_device_list();

  arv_set_fake_camera_genicam_filename(GENICAM_FILENAME);

  camera = arv_camera_new ("Fake_1");
  g_assert (ARV_IS_CAMERA (camera));

  device = arv_camera_get_device (camera);
  g_assert (ARV_IS_DEVICE (device));

  fake_camera = arv_fake_device_get_fake_camera (ARV_FAKE_DEVICE (device));
  g_assert (ARV_IS_FAKE_CAMERA (fake_camera));

  stream = arv_camera_create_stream (camera, NULL, NULL);
  g_assert (ARV_IS_STREAM (stream));

  arv_fake_camera_set_fill_pattern (fake_camera, fill_pattern_cb, &counter);

  payload = arv_camera_get_payload (camera);
  printf("payload: %d\n", payload);
  arv_stream_push_buffer (stream,  arv_buffer_new (payload, NULL));
  arv_camera_set_acquisition_mode (camera, ARV_ACQUISITION_MODE_SINGLE_FRAME);
  arv_camera_start_acquisition (camera);
  buffer = arv_stream_pop_buffer (stream);
  arv_camera_stop_acquisition (camera);

  arv_fake_camera_set_fill_pattern (fake_camera, NULL, NULL);

  printf("counter: %d\n", counter);
  printf("height: %d\n", arv_buffer_get_image_height(buffer));
  printf("width: %d\n", arv_buffer_get_image_height(buffer));

  if (arv_buffer_get_payload_type (buffer) == ARV_BUFFER_PAYLOAD_TYPE_IMAGE)
    printf("image\n");

  printf("pixelforma: %x\n", arv_buffer_get_image_pixel_format(buffer)); //ARV_PIXEL_FORMAT_MONO_8


  size_t buffer_size;
  //const void *framebuffer = arv_buffer_get_data (buffer, &buffer_size);
  const char *framebuffer = (const char *) arv_buffer_get_data (buffer, &buffer_size);
  printf("buffer size: %d\n", (int)buffer_size);

  if(framebuffer) {
    printf("boa famebuffer\n");
  }

  int i;
  //for (i = 0; i < (int)buffer_size; i++) {
  //  printf("content: %x \n", framebuffer[i]);
  //}

  g_file_set_contents ("/tmp/output.raw", (const gchar *)framebuffer, buffer_size, NULL);


  //--
  const int dimx = 512, dimy = 512;
  //int i;
  int j;
  FILE *fp = fopen("/tmp/first.ppm", "wb"); /* b - binary mode */
  (void) fprintf(fp, "P6\n%d %d\n255\n", dimx, dimy);

  printf("%p\n", framebuffer);
  for (j = 0; j < dimy; ++j)
  {
    for (i = 0; i < dimx; ++i)
    {
      static unsigned char color[3];
      color[0] = i % 256;  /* red */
      color[1] = j % 256;  /* green */
      color[2] = (i * j) % 256;  /* blue */
      (void) fwrite(color, 1, 3, fp);
    }
  }
  (void) fclose(fp);

  g_clear_object (&buffer);
  g_clear_object (&stream);
  g_clear_object (&camera);
  arv_shutdown();

  cout << "Done." << endl;

  return 0;
}
