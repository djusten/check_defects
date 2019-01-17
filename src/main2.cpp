#include <arv.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>

#include <string>
#include <vector>
#include <algorithm>
#include <memory>

/*
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
*/

class ICamera {
public:
  virtual ~ICamera() = default;

  virtual bool open(int) = 0;
  virtual void close() = 0;
  //virtual std::vector<char> getFramebuffer(std::vector<int>) = 0;
  virtual std::vector<char> getFramebuffer(int) = 0;
  virtual void
  arv_save_pgm(const char *, gchar *, int) = 0; // FIXME
};

class Camera : public ICamera {
public:
  Camera();
  virtual ~Camera() { close(); }

  bool open(int) override;
  void close() override;
  //std::vector<char> getFramebuffer(std::vector<int>) override;
  std::vector<char> getFramebuffer(int) override;
  void arv_save_pgm(const char *, gchar *, int) override;

private:
  bool create(int);
  bool initBuffers();

  void stopCapture();
  bool startCapture();

  // string vazia nao achou, ou poderia usar boost::optional
  std::string getDeviceNameById(int id);

  std::unique_ptr<ArvCamera> _camera;
  std::unique_ptr<ArvStream> _stream;
  //char *_framebuffer;

  unsigned int _payload{0};

  int _width{0};
  int _height{0};
};

Camera::Camera()
{
    //camera = NULL;
    //stream = NULL;
    //framebuffer = NULL;

    //payload = 0;
		//width, height = 0;
}

void Camera::close()
{
    if(_camera) {
        stopCapture();

        //g_object_unref(_camera);
        _camera = nullptr;
    }
}

//bool Camera::getDeviceNameById(int id, std::string &device)
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
    //if vazio ruim //FIXME
    //else//

    _camera.reset(arv_camera_new(deviceName.c_str()));
    //return (NULL != (_camera = arv_camera_new(deviceName.c_str())));
    return true;
}

bool Camera::initBuffers()
{
    if(_stream) {
        //g_object_unref(stream);
        _stream = nullptr;
    }

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

//char *Camera::getFramebuffer(int *size)
//std::vector<char> getFramebuffer(std::vector<int>)
std::vector<char> Camera::getFramebuffer(int size)
{
  printf("get frame\n" );
/*
    char *_framebuffer = NULL;

    if(_stream) {
        ArvBuffer *arv_buffer = NULL;
        int max_tries = 10;
        int tries = 0;
        for(; tries < max_tries; tries ++) {
            arv_buffer = arv_stream_timeout_pop_buffer (_stream, 200000);
            if (arv_buffer != NULL && arv_buffer_get_status (arv_buffer) != ARV_BUFFER_STATUS_SUCCESS) {
                arv_stream_push_buffer (_stream, arv_buffer);
            } else break;
        }
        if(arv_buffer != NULL && tries < max_tries) {
            size_t buffer_size;
            _framebuffer = arv_buffer_get_data (arv_buffer, &buffer_size);

						arv_buffer_get_image_region(arv_buffer, NULL, NULL, &_width, &_height);

						gchar *raw_data = (gchar *) g_memdup(_framebuffer, buffer_size);
						*size = buffer_size;

            arv_stream_push_buffer(_stream, arv_buffer);
            return raw_data;
        }
    }
*/
  return {};
}

void Camera::stopCapture()
{
    arv_camera_stop_acquisition(_camera.get());

    if(_stream) {
        //g_object_unref(_stream);
        _stream = NULL;
    }
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

/*
Camera* newCamera(int index)
{
    Camera* capture = new Camera;

    if(capture->open(index)) {
        return capture;
    }

    delete capture;
    return NULL;
}
*/

void Camera::arv_save_pgm(const char * filename, gchar *data, int size)
{
		gchar *head;
		gchar *buff;

		head = g_strdup_printf ("P5\n %d\n %d\n 255\n", _height, _width);

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

  ICamera *cam = new Camera();

	//Camera *cameraCapture = newCamera(index);

  if (!cam->open(index)) {
		printf("Error\n");
	} else {

		int size;
		char *p;

    std::vector<char> data = cam->getFramebuffer(size);
		//p = cameraCapture->getFramebuffer(&size);

		cam->arv_save_pgm("/tmp/file.pgm", p, size);

		cam->close();

		g_free(p);
	}

  arv_shutdown();

	return 0;
}
