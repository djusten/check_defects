#include <iostream>
#include <arv.h>
#include <string.h>
#include <png.h>
#include <glib-unix.h>

using namespace std;

static void arv_save_pgm(ArvBuffer * buffer, const char * filename)
{
		size_t buffer_size;
		const void *framebuffer;
		gchar *head;
		gchar *buff;

		framebuffer = arv_buffer_get_data (buffer, &buffer_size);

		head = g_strdup_printf ("P5\n %d\n %d\n 255\n", arv_buffer_get_image_height(buffer), arv_buffer_get_image_width(buffer));

		buff = (gchar *)malloc(strlen(head) + buffer_size);
		memcpy (buff, head, strlen(head));
		memcpy (buff + strlen(head), framebuffer, buffer_size);

		g_file_set_contents (filename, (const gchar *)buff, buffer_size + strlen(head), NULL);

		free(head);
		free(buff);
}

static void new_buffer_cb (ArvStream *stream, int *frame_trigger)
{
  ArvBuffer *buffer;

  buffer = arv_stream_try_pop_buffer (stream);
  if (buffer != NULL) {
    if (arv_buffer_get_status (buffer) == ARV_BUFFER_STATUS_SUCCESS) {
			if (*frame_trigger == 0) {
				gchar *filename = g_strdup_printf("/tmp/output%d.pgm", arv_buffer_get_frame_id(buffer));
      	arv_save_pgm(buffer, filename);
			}
			(*frame_trigger)--;
			arv_stream_push_buffer (stream, buffer);
		}
  }
}

int main(int argc, char** argv)
{
  ArvCamera *camera;
  ArvStream *stream;
  gint payload;
  gint counter = 0;
  int i;
	int frame_trigger = 0;

	if (argc > 1) {
		frame_trigger = strtol (argv[1], NULL, 0);
	}

  arv_enable_interface("Fake");

  arv_update_device_list();

  camera = arv_camera_new ("Fake_1");
  g_assert (ARV_IS_CAMERA (camera));

  stream = arv_camera_create_stream (camera, NULL, NULL);
  g_assert (ARV_IS_STREAM (stream));

  payload = arv_camera_get_payload (camera);

	arv_stream_push_buffer (stream,  arv_buffer_new (payload, NULL));

  arv_camera_set_acquisition_mode (camera, ARV_ACQUISITION_MODE_CONTINUOUS);
  arv_camera_start_acquisition(camera);

	g_signal_connect (stream, "new-buffer", G_CALLBACK (new_buffer_cb), &frame_trigger);
	arv_stream_set_emit_signals (stream, TRUE);

	while (frame_trigger >= 0) {
		usleep(1000);
	}

  arv_camera_stop_acquisition (camera);

	arv_stream_set_emit_signals (stream, FALSE);

	g_object_unref (stream);
	g_object_unref (camera);

  arv_shutdown();

  cout << "Done." << endl;

  return 0;
}
