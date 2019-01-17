#include <iostream>
#include <arv.h>
#include <string.h>
#include <png.h>

using namespace std;

#define GENICAM_FILENAME "/home/diogo/work/personal/check_defects/src/arv-fake-camera.xml"

static void arv_save_png(ArvBuffer * buffer, const char * filename)
{
	// TODO: This only works on image buffers
	g_assert(arv_buffer_get_payload_type(buffer) == ARV_BUFFER_PAYLOAD_TYPE_IMAGE);
	
	size_t buffer_size;
	char * buffer_data = (char*)arv_buffer_get_data(buffer, &buffer_size); // raw data
	int width; int height;
	arv_buffer_get_image_region(buffer, NULL, NULL, &width, &height); // get width/height
	int bit_depth = ARV_PIXEL_FORMAT_BIT_PER_PIXEL(arv_buffer_get_image_pixel_format(buffer)); // bit(s) per pixel
	//TODO: Deal with non-png compliant pixel formats?
	// EG: ARV_PIXEL_FORMAT_MONO_14 is 14 bits per pixel, so conversion to PNG loses data
	
	int arv_row_stride = width * bit_depth/8; // bytes per row, for constructing row pointers
	int color_type = PNG_COLOR_TYPE_GRAY; //TODO: Check for other types?
	
	// boilerplate libpng stuff without error checking (setjmp? Seriously? How many kittens have to die?)
	png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	png_infop info_ptr = png_create_info_struct(png_ptr);
	FILE * f = fopen(filename, "wb");
	png_init_io(png_ptr, f);
	png_set_IHDR(png_ptr, info_ptr, width, height, bit_depth, color_type,
		PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
	png_write_info(png_ptr, info_ptr);
	
	// Need to create pointers to each row of pixels for libpng
	png_bytepp rows = (png_bytepp)(png_malloc(png_ptr, height*sizeof(png_bytep)));
	int i =0;
	for (i = 0; i < height; ++i)
		rows[i] = (png_bytep)(buffer_data + (height - i)*arv_row_stride);
	// Actually write image
	png_write_image(png_ptr, rows);
	png_write_end(png_ptr, NULL); // cleanup
	fclose(f);
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

	printf("%s\n", arv_camera_get_trigger_source (camera));

	guint n_sources;
	const char **trigger = arv_camera_get_available_trigger_sources(camera, &n_sources);
	printf("n_source: %d\n", n_sources);
	printf("source: %s\n", trigger[0]);

	const char **xx = arv_camera_get_available_triggers (camera, &n_sources);
	printf("n_source: %d\n", n_sources);
	printf("source: %s\n", xx[0]);
	printf("source: %s\n", xx[1]);

	buffer = arv_camera_acquisition (camera, 0);
	arv_save_png(buffer, "/tmp/png.png");
	return 0;

  device = arv_camera_get_device (camera);
  g_assert (ARV_IS_DEVICE (device));

  stream = arv_camera_create_stream (camera, NULL, NULL);
  g_assert (ARV_IS_STREAM (stream));

  payload = arv_camera_get_payload (camera);

	for (i = 0; i <2; i++) {
		arv_stream_push_buffer (stream,  arv_buffer_new (payload, NULL));
	}
  //arv_camera_set_acquisition_mode (camera, ARV_ACQUISITION_MODE_SINGLE_FRAME);
  arv_camera_set_acquisition_mode (camera, ARV_ACQUISITION_MODE_CONTINUOUS);
  arv_camera_start_acquisition (camera);
  buffer = arv_stream_pop_buffer (stream);

  printf("height: %d\n", arv_buffer_get_image_height(buffer));
  printf("width: %d\n", arv_buffer_get_image_width(buffer));

	arv_save_png(buffer, "/tmp/png.png");

	for (i = 0; i < 1; i++) {
		size_t buffer_size;
		buffer = arv_stream_pop_buffer (stream);

		const char *framebuffer = (const char *) arv_buffer_get_data (buffer, &buffer_size);

		gchar *head = g_strdup_printf ("P5\n %d\n %d\n 255\n", arv_buffer_get_image_height(buffer), arv_buffer_get_image_width(buffer));

		char *x = (char *)malloc(strlen(head) + buffer_size);
		memcpy (x, head, strlen(head));
		memcpy (x + strlen(head), framebuffer, buffer_size);

		//g_file_set_contents ("/tmp/output.pgm", (const gchar *)x, buffer_size, NULL);
		gchar *filename = g_strdup_printf("/tmp/output%d.pgm", i);
		//g_file_set_contents ("/tmp/output.pgm", (const gchar *)x, buffer_size + strlen(head), NULL);
		g_file_set_contents (filename, (const gchar *)x, buffer_size + strlen(head), NULL);

		free(x);
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
