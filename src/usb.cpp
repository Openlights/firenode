#include "usb.h"


USBStrandController::USBStrandController()
{
    _handle = NULL;
}


USBStrandController::~USBStrandController()
{
    if (_handle != NULL) {
        libusb_close(_handle);
    }
}


bool USBStrandController::connect()
{
    int ret, cfg = -999;
    libusb_context *ctx;
    _handle = NULL;

    if (libusb_init(&ctx) < 0) {
        qDebug() << "Error initializing libusb.";
        return false;
    }

    libusb_set_debug(ctx, 3);

    _handle = libusb_open_device_with_vid_pid(ctx, STRAND_CONTROLLER_VID, STRAND_CONTROLLER_PID);

    if (_handle == NULL) {
        qDebug() << "Could not find strand controller.";
        return false;
    }

    if (libusb_kernel_driver_active(_handle, 0)) {
        qDebug() << "Warn: Kernel driver is active";
        return false;
    }

    libusb_reset_device(_handle);

    ret = libusb_set_configuration(_handle, 1);
    if (ret < 0) {
        qDebug() << "Error setting config:" << libusb_error_name(ret);
        return false;
    }

    ret = libusb_get_configuration(_handle, &cfg);
    if (ret < 0) {
        qDebug() << "Error getting config:" << libusb_error_name(ret);
        return false;
    }

    ret = libusb_claim_interface(_handle, 0);
    if (ret < 0) {
        qDebug() << "Error claiming interface:" << libusb_error_name(ret);
        return false;
    }

    ret = libusb_set_interface_alt_setting(_handle, 0, 1);
    if (ret < 0) {
        qDebug() << "Error setting up interface:" << libusb_error_name(ret);
        return false;
    }

    qDebug() << "Connected to strand controller.";

    return true;
}


void USBStrandController::write_data(QByteArray *data)
{
    int transferred = 0, ret;
    unsigned char d[] = {0x04, 0x05};

    ret = libusb_bulk_transfer(_handle, EP_OUT, d, 2, &transferred, 1000);

    if (ret < 0) {
        qDebug() << "bulk_transfer returned" << libusb_error_name(ret);
    }

    /*if (transferred != data->size()) {
        qDebug() << "Tried to write" << data->size() << "but actually wrote" << transferred;
    }*/
}