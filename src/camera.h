#include <arv.h>

#include "icamera.h"

class GObjectDeleter {
public:
  void operator()(gpointer p) {
    g_clear_object(&p);
  }
};

class Camera : public ICamera {
  public:
    Camera() {}
    virtual ~Camera() { close(); }

    bool open(int) override;
    void close() override;
    std::vector<char> getFramebuffer() override;
    void save_to_file(std::string, std::vector<char>) override;

  private:
    bool create(int);
    bool initBuffers();

    void stopCapture();
    bool startCapture();

    std::string getDeviceNameById(int id);

    template <typename T>
    using unique_glib_ptr = std::unique_ptr<T, GObjectDeleter>;
    unique_glib_ptr<ArvCamera> _camera;
    unique_glib_ptr<ArvStream> _stream;

    unsigned int _payload{0};

    int _width{0};
    int _height{0};
};
