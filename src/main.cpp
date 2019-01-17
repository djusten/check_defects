#include <iostream>
#include <arv.h>
#include <string.h>

using namespace std;

#define GENICAM_FILENAME "/home/diogo/work/personal/check_defects/src/arv-fake-camera.xml"

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
  int i;

  arv_enable_interface("Fake");

  arv_update_device_list();

	unsigned int n_devices = arv_get_n_devices ();
  for (i = 0; i < (int)n_devices; i++) {
    printf("protocol: %s\n", arv_get_device_protocol (i));
	}



  //arv_set_fake_camera_genicam_filename(GENICAM_FILENAME);

  camera = arv_camera_new ("Fake_1");
  g_assert (ARV_IS_CAMERA (camera));

  device = arv_camera_get_device (camera);
  g_assert (ARV_IS_DEVICE (device));

  fake_camera = arv_fake_device_get_fake_camera (ARV_FAKE_DEVICE (device));
  g_assert (ARV_IS_FAKE_CAMERA (fake_camera));

  stream = arv_camera_create_stream (camera, NULL, NULL);
  g_assert (ARV_IS_STREAM (stream));

  //arv_fake_camera_set_fill_pattern (fake_camera, fill_pattern_cb, &counter);

  payload = arv_camera_get_payload (camera);
  printf("payload: %d\n", payload);
  arv_stream_push_buffer (stream,  arv_buffer_new (payload, NULL));
  //arv_camera_set_acquisition_mode (camera, ARV_ACQUISITION_MODE_SINGLE_FRAME);
  arv_camera_set_acquisition_mode (camera, ARV_ACQUISITION_MODE_CONTINUOUS);
  arv_camera_start_acquisition (camera);
  payload = arv_camera_get_payload (camera);
  printf("payload: %d\n", payload);
	printf("debug 1\n");
  buffer = arv_stream_pop_buffer (stream);
	printf("debug 2\n");
  arv_camera_stop_acquisition (camera);

  arv_fake_camera_set_fill_pattern (fake_camera, NULL, NULL);

  printf("counter: %d\n", counter);
  printf("height: %d\n", arv_buffer_get_image_height(buffer));
  printf("width: %d\n", arv_buffer_get_image_height(buffer));

  if (arv_buffer_get_payload_type (buffer) == ARV_BUFFER_PAYLOAD_TYPE_IMAGE)
    printf("image\n");

  printf("pixelforma: %x\n", arv_buffer_get_image_pixel_format(buffer)); //ARV_PIXEL_FORMAT_MONO_8


	for (i = 0; i < 1; i++) {
		size_t buffer_size;
		//const void *framebuffer = arv_buffer_get_data (buffer, &buffer_size);
		const char *framebuffer = (const char *) arv_buffer_get_data (buffer, &buffer_size);
		printf("buffer size: %d\n", (int)buffer_size);

		if(framebuffer) {
			printf("boa famebuffer\n");
		}

		//for (i = 0; i < (int)buffer_size; i++) {
		//for (i = 0; i < 10; i++) {
			printf("content: %x \n", framebuffer[145]);
		//}

		gchar *head = g_strdup_printf ("P5\n 512\n 512\n 255\n");

		char *x = (char *)malloc(strlen(head) + buffer_size);
		memcpy (x, head, strlen(head));
		memcpy (x + strlen(head), framebuffer, buffer_size);

		g_file_set_contents ("/tmp/output.pgm", (const gchar *)x, buffer_size, NULL);
		//g_file_set_contents ("/tmp/output.pgm", (const gchar *)buf, buffer_size + strlen(head), NULL);
	}


  g_clear_object (&buffer);
  g_clear_object (&stream);
  g_clear_object (&camera);
  arv_shutdown();

  cout << "Done." << endl;

  return 0;
}
