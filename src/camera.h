#include <arv.h>

#include "icamera.h"

class Camera : public ICamera {
  public:
    Camera();
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

    std::unique_ptr<ArvCamera> _camera;
    std::unique_ptr<ArvStream> _stream;

    unsigned int _payload{0};

    int _width{0};
    int _height{0};
};
