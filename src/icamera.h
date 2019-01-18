class ICamera {
  public:
    virtual ~ICamera() = default;

    virtual bool open(int) = 0;
    virtual void close() = 0;
    virtual std::vector<char> getFramebuffer() = 0;
    virtual void save_to_file(std::string, std::vector<char>) = 0;
};
