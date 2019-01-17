#include <iostream>
#include <arv.h>
#include <string.h>
#include <png.h>

using namespace std;

static void arv_save_pgm(ArvBuffer * buffer, const char * filename)
{
		size_t buffer_size;
		const char *framebuffer;
		gchar *head;
		gchar *buff;

		framebuffer = arv_buffer_get_data (buffer, &buffer_size);

		head = g_strdup_printf ("P5\n %d\n %d\n 255\n", arv_buffer_get_image_height(buffer), arv_buffer_get_image_width(buffer));

		buff = malloc(strlen(head) + buffer_size);
		memcpy (buff, head, strlen(head));
		memcpy (buff + strlen(head), framebuffer, buffer_size);

		g_file_set_contents (filename, (const gchar *)buff, buffer_size + strlen(head), NULL);

		free(head);
		free(buff);
}

static void new_buffer_cb (ArvStream *stream, void *user_data)
{
  ArvBuffer *buffer;

  buffer = arv_stream_try_pop_buffer (stream);
  if (buffer != NULL) {
    if (arv_buffer_get_status (buffer) == ARV_BUFFER_STATUS_SUCCESS) {
			arv_stream_push_buffer (stream, buffer);
		}
  }
}

static void stream_cb (void *user_data, ArvStreamCallbackType type, ArvBuffer *buffer)
{
	if (type == ARV_STREAM_CALLBACK_TYPE_BUFFER_DONE) {
		printf("stram_CB frameid: %d\n", arv_buffer_get_frame_id (buffer));
	}
}

int main(int argc, char** argv)
{
  ArvCamera *camera;
  ArvStream *stream;
  ArvBuffer *buffer;
  gint payload;
  gint counter = 0;
  int i;

  arv_enable_interface("Fake");

  arv_update_device_list();

  camera = arv_camera_new ("Fake_1");
  g_assert (ARV_IS_CAMERA (camera));

  stream = arv_camera_create_stream (camera, stream_cb, NULL);
  g_assert (ARV_IS_STREAM (stream));

  payload = arv_camera_get_payload (camera);

	//for (i = 0; i <2; i++) {
	//	arv_stream_push_buffer (stream,  arv_buffer_new (payload, NULL));
	//}

  arv_camera_set_acquisition_mode (camera, ARV_ACQUISITION_MODE_CONTINUOUS);
  arv_camera_start_acquisition(camera);

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
