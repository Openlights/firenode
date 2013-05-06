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
	libusb_context *ctx;
	_handle = NULL;

    if (libusb_init(&ctx) < 0) {
    	qDebug() << "Error initializing libusb.";
    	return false;
    }

    _handle = libusb_open_device_with_vid_pid(ctx, STRAND_CONTROLLER_VID, STRAND_CONTROLLER_PID);

    if (_handle == NULL) {
    	qDebug() << "Could not find strand controller.";
    	return false;
    }

    qDebug() << "Connected to strand controller.";

    return true;
}


void USBStrandController::write_data(QByteArray *data)
{
	/*
    _port.clear();

    if (!_port.write(*data)) {
        qDebug() << "Write error";
    }

    if (!_port.waitForBytesWritten(30)) {
        qDebug() << "Timeout!";
    }

    _port.flush();
    */
}