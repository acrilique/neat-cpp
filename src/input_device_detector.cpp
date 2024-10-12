#include "input_device_detector.h"
#include <QString>

InputDeviceDetector::InputDeviceDetector(QObject *parent)
    : QObject(parent)
#ifdef Q_OS_LINUX
    , m_udev(udev_new(), udev_unref)
#endif
{
#ifdef Q_OS_MACOS
    m_hidManager = IOHIDManagerCreate(kCFAllocatorDefault, kIOHIDOptionsTypeNone);
    if (m_hidManager) {
        IOHIDManagerOpen(m_hidManager, kIOHIDOptionsTypeNone);
        
        CFDictionaryRef matchingDict = IOServiceMatching("AppleMultitouchTrackpadDevice");
        IOHIDManagerSetDeviceMatching(m_hidManager, matchingDict);
        
        IOHIDManagerRegisterDeviceMatchingCallback(m_hidManager, deviceMatchingCallback, this);
        IOHIDManagerScheduleWithRunLoop(m_hidManager, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);
    }
#endif
}

InputDeviceDetector::~InputDeviceDetector()
{
#ifdef Q_OS_MACOS
    if (m_hidManager) {
        IOHIDManagerUnscheduleFromRunLoop(m_hidManager, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);
        IOHIDManagerClose(m_hidManager, kIOHIDOptionsTypeNone);
        CFRelease(m_hidManager);
    }
#endif
}

InputDeviceDetector::DeviceType InputDeviceDetector::getInputDeviceType()
{
#ifdef Q_OS_WIN
    m_lastDetectedType = isPointingDeviceTouchpad() ? DeviceType::Touchpad : DeviceType::Mouse;
#elif defined(Q_OS_MACOS)
    m_lastDetectedType = isTouchpadConnected() ? DeviceType::Touchpad : DeviceType::Mouse;
#elif defined(Q_OS_LINUX)
    m_lastDetectedType = isTouchpadConnected() ? DeviceType::Touchpad : DeviceType::Mouse;
#else
    m_lastDetectedType = DeviceType::Unknown;
#endif

    return m_lastDetectedType;
}

#ifdef Q_OS_WIN
bool InputDeviceDetector::isPointingDeviceTouchpad()
{
    int value = GetSystemMetrics(SM_DIGITIZER);
    return (value & NID_INTEGRATED_TOUCH) == NID_INTEGRATED_TOUCH;
}
#elif defined(Q_OS_MACOS)
bool InputDeviceDetector::isTouchpadConnected()
{
    return m_touchpadFound;
}

void InputDeviceDetector::deviceMatchingCallback(void *context, IOReturn result, void *sender, IOHIDDeviceRef device)
{
    Q_UNUSED(result);
    Q_UNUSED(sender);
    Q_UNUSED(device);
    
    auto *detector = static_cast<InputDeviceDetector*>(context);
    detector->m_touchpadFound = true;
}
#elif defined(Q_OS_LINUX)
bool InputDeviceDetector::isTouchpadConnected()
{
    if (!m_udev) return false;

    udev_enumerate* enumerate = udev_enumerate_new(m_udev.get());
    udev_enumerate_add_match_subsystem(enumerate, "input");
    udev_enumerate_scan_devices(enumerate);

    udev_list_entry* devices = udev_enumerate_get_list_entry(enumerate);
    udev_list_entry* dev_list_entry;

    bool foundTouchpad = false;

    udev_list_entry_foreach(dev_list_entry, devices) {
        const char* path = udev_list_entry_get_name(dev_list_entry);
        udev_device* dev = udev_device_new_from_syspath(m_udev.get(), path);

        const char* devnode = udev_device_get_devnode(dev);
        if (devnode && strstr(devnode, "event")) {
            udev_device* parent = udev_device_get_parent_with_subsystem_devtype(dev, "input", NULL);
            if (parent) {
                const char* driver = udev_device_get_driver(parent);
                if (driver && (strcmp(driver, "synaptics_i2c") == 0 || 
                               strcmp(driver, "elan_i2c") == 0 ||
                               strcmp(driver, "hid_multitouch") == 0)) {
                    foundTouchpad = true;
                    udev_device_unref(dev);
                    break;
                }
            }
        }
        udev_device_unref(dev);
    }

    udev_enumerate_unref(enumerate);
    return foundTouchpad;
}
#endif