
#ifndef INPUT_DEVICE_DETECTOR_H
#define INPUT_DEVICE_DETECTOR_H

#include <QObject>

#ifdef Q_OS_WIN
#include <windows.h>
#elif defined(Q_OS_MACOS)
#include <IOKit/hid/IOHIDManager.h>
#elif defined(Q_OS_LINUX)
#include <libudev.h>
#include <memory>
#endif

class InputDeviceDetector : public QObject
{
    Q_OBJECT

public:
    enum class DeviceType {
        Unknown,
        Mouse,
        Touchpad
    };

    explicit InputDeviceDetector(QObject *parent = nullptr);
    ~InputDeviceDetector();

    DeviceType getInputDeviceType();

private:
    DeviceType m_lastDetectedType = DeviceType::Unknown;

#ifdef Q_OS_WIN
    bool isPointingDeviceTouchpad();
#elif defined(Q_OS_MACOS)
    bool isTouchpadConnected();
    static void deviceMatchingCallback(void *context, IOReturn result, void *sender, IOHIDDeviceRef device);
    IOHIDManagerRef m_hidManager = nullptr;
    bool m_touchpadFound = false;
#elif defined(Q_OS_LINUX)
    bool isTouchpadConnected();
    std::unique_ptr<udev, decltype(&udev_unref)> m_udev;
#endif
};

#endif // INPUT_DEVICE_DETECTOR_H