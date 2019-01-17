#include <arv.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>

#include <string>

class Camera {
public:
    Camera();
    virtual ~Camera()
    {
        close();
    }

    virtual bool open(int);
    virtual void close();
    virtual char *getFramebuffer(int*);
    virtual void arv_save_pgm(const char*, gchar*, int);

protected:
    bool create(int);
    bool init_buffers();

    void stopCapture();
    bool startCapture();

    bool getDeviceNameById(int id, std::string &device);

    ArvCamera *camera;
    ArvStream *stream;
    const void *framebuffer;

    unsigned int payload;

		int width;
		int height;
};

Camera::Camera()
{
    camera = NULL;
    stream = NULL;
    framebuffer = NULL;

    payload = 0;
		width, height = 0;
}

void Camera::close()
{
    if(camera) {
        stopCapture();

        g_object_unref(camera);
        camera = NULL;
    }
}

bool Camera::getDeviceNameById(int id, std::string &device)
{
    arv_update_device_list();

    if((id >= 0) && (id < (int)arv_get_n_devices())) {
        device = arv_get_device_id(id);
        return true;
    }

    return false;
}

bool Camera::create(int index)
{
    std::string deviceName;
    if(!getDeviceNameById(index, deviceName))
        return false;

    return NULL != (camera = arv_camera_new(deviceName.c_str()));
}

bool Camera::init_buffers()
{
    if(stream) {
        g_object_unref(stream);
        stream = NULL;
    }
    if( (stream = arv_camera_create_stream(camera, NULL, NULL)) ) {
        payload = arv_camera_get_payload (camera);

				arv_stream_push_buffer(stream, arv_buffer_new(payload, NULL));

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

char *Camera::getFramebuffer(int *size)
{
    // remove content of previous frame
    framebuffer = NULL;

    if(stream) {
        ArvBuffer *arv_buffer = NULL;
        int max_tries = 10;
        int tries = 0;
        for(; tries < max_tries; tries ++) {
            arv_buffer = arv_stream_timeout_pop_buffer (stream, 200000);
            if (arv_buffer != NULL && arv_buffer_get_status (arv_buffer) != ARV_BUFFER_STATUS_SUCCESS) {
                arv_stream_push_buffer (stream, arv_buffer);
            } else break;
        }
        if(arv_buffer != NULL && tries < max_tries) {
            size_t buffer_size;
            framebuffer = arv_buffer_get_data (arv_buffer, &buffer_size);

						arv_buffer_get_image_region(arv_buffer, NULL, NULL, &width, &height);

						gchar *raw_data = (gchar *) g_memdup(framebuffer, buffer_size);
						*size = buffer_size;

            arv_stream_push_buffer(stream, arv_buffer);
            return raw_data;
        }
    }
    return NULL;
}

void Camera::stopCapture()
{
    arv_camera_stop_acquisition(camera);

    if(stream) {
        g_object_unref(stream);
        stream = NULL;
    }
}

bool Camera::startCapture()
{
    if(init_buffers() ) {
        arv_camera_set_acquisition_mode(camera, ARV_ACQUISITION_MODE_CONTINUOUS);
        arv_camera_start_acquisition(camera);

        return true;
    }
    return false;
}

Camera* newCamera(int index)
{
    Camera* capture = new Camera;

    if(capture->open(index)) {
        return capture;
    }

    delete capture;
    return NULL;
}

void Camera::arv_save_pgm(const char * filename, gchar *data, int size)
{
		gchar *head;
		gchar *buff;

		head = g_strdup_printf ("P5\n %d\n %d\n 255\n", height, width);

		buff = (gchar *)malloc(strlen(head) + size);
		memcpy (buff, head, strlen(head));
		memcpy (buff + strlen(head), data, size);

		g_file_set_contents (filename, (const gchar *)buff, size + strlen(head), NULL);

		g_free(head);
		free(buff);
}

int main(int argc, char** argv)
{
	int index = 0;

	int frame_trigger = 0;

	if (argc > 1) {
		frame_trigger = strtol (argv[1], NULL, 0);
	}

	arv_enable_interface("Fake");

	Camera *cameraCapture = newCamera(index);

	if (cameraCapture == NULL) {
		printf("Error\n");
	} else {

		int size;
		char *p;

		p = cameraCapture->getFramebuffer(&size);

		cameraCapture->arv_save_pgm("/tmp/file.pgm", p, size);

		cameraCapture->close();

		g_free(p);
	}

  arv_shutdown();

	return 0;
}
