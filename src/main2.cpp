#include <arv.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>

#include <string>

//class CvCaptureCAM_Aravis : public CvCapture {
class CvCaptureCAM_Aravis {
public:
    CvCaptureCAM_Aravis();
    virtual ~CvCaptureCAM_Aravis()
    {
        close();
    }

    virtual bool open(int);
    virtual void close();
    virtual bool grabFrame(); // CV_OVERRIDE;

protected:
    bool create(int);
    bool init_buffers();

    void stopCapture();
    bool startCapture();

    bool getDeviceNameById(int id, std::string &device);

    ArvCamera       *camera;                // Camera to control.
    ArvStream       *stream;                // Object for video stream reception.
    void            *framebuffer;           //

    unsigned int    payload;                // Width x height x Pixel width.
};

CvCaptureCAM_Aravis::CvCaptureCAM_Aravis()
{
    camera = NULL;
    stream = NULL;
    framebuffer = NULL;

    payload = 0;
}

void CvCaptureCAM_Aravis::close()
{
    if(camera) {
        stopCapture();

        g_object_unref(camera);
        camera = NULL;
    }
}

bool CvCaptureCAM_Aravis::getDeviceNameById(int id, std::string &device)
{
    arv_update_device_list();

    if((id >= 0) && (id < (int)arv_get_n_devices())) {
        device = arv_get_device_id(id);
        return true;
    }

    return false;
}

bool CvCaptureCAM_Aravis::create( int index )
{
		printf("create\n");
    std::string deviceName;
    if(!getDeviceNameById(index, deviceName))
        return false;

    return NULL != (camera = arv_camera_new(deviceName.c_str()));
}

bool CvCaptureCAM_Aravis::init_buffers()
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

bool CvCaptureCAM_Aravis::open( int index )
{
		printf("open\n");
    if(create(index)) {
        return startCapture();
    }
    return false;
}

bool CvCaptureCAM_Aravis::grabFrame()
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
            framebuffer = (void*)arv_buffer_get_data (arv_buffer, &buffer_size);

            arv_stream_push_buffer(stream, arv_buffer);
            return true;
        }
    }
    return false;
}

void CvCaptureCAM_Aravis::stopCapture()
{
    arv_camera_stop_acquisition(camera);

    if(stream) {
        g_object_unref(stream);
        stream = NULL;
    }
}

bool CvCaptureCAM_Aravis::startCapture()
{
    if(init_buffers() ) {
        arv_camera_set_acquisition_mode(camera, ARV_ACQUISITION_MODE_CONTINUOUS);
        arv_camera_start_acquisition(camera);

        return true;
    }
    return false;
}

//CvCapture* cvCreateCameraCapture_Aravis( int index )
CvCaptureCAM_Aravis* cvCreateCameraCapture_Aravis( int index )
{
    CvCaptureCAM_Aravis* capture = new CvCaptureCAM_Aravis;

    if(capture->open(index)) {
        return capture;
    }

    delete capture;
    return NULL;
}

int main ()
{
	//CvCapture* = cvCreateCameraCapture_Aravis(index);
	int index = 0;

	arv_enable_interface("Fake");

	CvCaptureCAM_Aravis *CvCapture = cvCreateCameraCapture_Aravis(index);

	if (CvCapture == NULL) {
		printf("Error\n");
	} else {
		printf("Sucess\n");
	}

	return 0;
}
