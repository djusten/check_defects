#include <iostream>
#include <arv.h>
#include <string.h>
#include <png.h>

using namespace std;

#define GENICAM_FILENAME "/home/diogo/work/personal/check_defects/src/arv-fake-camera.xml"

static void arv_save_pgm(ArvBuffer * buffer, const char * filename)
{
		size_t buffer_size;
		const char *framebuffer = (const char *) arv_buffer_get_data (buffer, &buffer_size);

		gchar *head = g_strdup_printf ("P5\n %d\n %d\n 255\n", arv_buffer_get_image_height(buffer), arv_buffer_get_image_width(buffer));

		char *x = (char *)malloc(strlen(head) + buffer_size);
		memcpy (x, head, strlen(head));
		memcpy (x + strlen(head), framebuffer, buffer_size);

		g_file_set_contents (filename, (const gchar *)x, buffer_size + strlen(head), NULL);

		free(x);
}

static void
new_buffer_cb (ArvStream *stream, void *data)
{
  ArvBuffer *buffer;

	printf("callback\n");
  buffer = arv_stream_try_pop_buffer (stream);
  if (buffer != NULL) {
    if (arv_buffer_get_status (buffer) == ARV_BUFFER_STATUS_SUCCESS) {
			arv_stream_push_buffer (stream, buffer);
		}
  }
}

static void
stream_cb (void *user_data, ArvStreamCallbackType type, ArvBuffer *buffer)
{
	printf("type: %d\n", type);
	if (type == ARV_STREAM_CALLBACK_TYPE_BUFFER_DONE )
	printf("stram_CB frameid: %d\n", arv_buffer_get_frame_id (buffer));
}

int main(int argc, char** argv)
{
  ArvCamera *camera;
  ArvDevice *device;
  ArvStream *stream;
  ArvBuffer *buffer;
  gint payload;
  gint counter = 0;
  int i;

  arv_enable_interface("Fake");

  arv_update_device_list();

  camera = arv_camera_new ("Fake_1");
  g_assert (ARV_IS_CAMERA (camera));

	ArvFakeCamera *camera_fake = 	arv_fake_camera_new("Fake_1");
	printf("acquisition: %d\n", arv_fake_camera_get_acquisition_status(camera_fake));

	arv_camera_set_trigger (camera, "Software");

	printf("Trigger: %s\n", arv_camera_get_trigger_source (camera));

	guint n_sources;
	const char **trigger = arv_camera_get_available_trigger_sources(camera, &n_sources);
	printf("n_source: %d\n", n_sources);
	printf("source: %s\n", trigger[0]);

	const char **xx = arv_camera_get_available_triggers (camera, &n_sources);
	printf("n_source: %d\n", n_sources);
	printf("source: %s\n", xx[0]);
	printf("source: %s\n", xx[1]);

	//buffer = arv_camera_acquisition (camera, 0);
	//arv_save_png(buffer, "/tmp/png.png");
	//return 0;

  device = arv_camera_get_device (camera);
  g_assert (ARV_IS_DEVICE (device));

  stream = arv_camera_create_stream (camera, stream_cb, NULL);
  g_assert (ARV_IS_STREAM (stream));

  payload = arv_camera_get_payload (camera);

	for (i = 0; i <2; i++) {
		arv_stream_push_buffer (stream,  arv_buffer_new (payload, NULL));
	}

  arv_camera_set_acquisition_mode (camera, ARV_ACQUISITION_MODE_CONTINUOUS);
  arv_camera_start_acquisition (camera);

	g_signal_connect (stream, "new-buffer", G_CALLBACK (new_buffer_cb), NULL);
	arv_stream_set_emit_signals (stream, TRUE);

  buffer = arv_stream_pop_buffer (stream);

  printf("height: %d\n", arv_buffer_get_image_height(buffer));
  printf("width: %d\n", arv_buffer_get_image_width(buffer));

	arv_save_pgm(buffer, "/tmp/png.pgm");

	GMainLoop *loop = g_main_loop_new (NULL, FALSE);
	printf("Antes do loop\n");
	g_main_loop_run (loop);
	printf("Depois do loop\n");

	for (i = 0; i < 1; i++) {
		buffer = arv_stream_pop_buffer (stream);

		gchar *filename = g_strdup_printf("/tmp/output%d.pgm", i);
		arv_save_pgm(buffer, filename);
		g_free(filename);
	}

  arv_camera_stop_acquisition (camera);

  g_clear_object (&buffer);
  g_clear_object (&stream);
  g_clear_object (&camera);
  arv_shutdown();

  cout << "Done." << endl;

  return 0;
}
