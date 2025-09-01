#pragma once
#include "ftdi.h"
#include <cstring>
#include <iostream>
#include <unistd.h>

class SimpleDMXSender {
public:
  SimpleDMXSender(const char *serial, uint16_t VID, uint16_t PID)
      : m_serial(serial), m_vid(VID), m_pid(PID) {
    if (ftdi_init(&m_handle) < 0) {
      std::cerr << "Failed to init ftdi: " << ftdi_get_error_string(&m_handle)
                << std::endl;
    } 
  }

  ~SimpleDMXSender() {
    close();
    ftdi_deinit(&m_handle);
  }

  bool tryOpenIfClosed() {
    if (isOpen)
      return true;
    return open();
  }

  bool open() {
    isOpen = false;
    if (ftdi_usb_open(&m_handle, m_vid, m_pid) < 0) {
      std::cerr << "Failed to open device: " << ftdi_get_error_string(&m_handle)
                << std::endl;
      return false;
    }
    if (ftdi_set_baudrate(&m_handle, 250000) < 0) {
      std::cerr << "Failed to set baudrate: "
                << ftdi_get_error_string(&m_handle) << std::endl;
      return false;
    }
    if (ftdi_set_line_property(&m_handle, BITS_8, STOP_BIT_2, NONE) < 0) {
      std::cerr << "Failed to set line prop ftdi: "
                << ftdi_get_error_string(&m_handle) << std::endl;
      return false;
    };

    if (ftdi_setrts(&m_handle, 0) < 0) {
      std::cerr << "Failed to clearRts " << ftdi_get_error_string(&m_handle)
                << std::endl;
      return false;
    }

    if (ftdi_usb_purge_buffers(&m_handle) < 0) {
      std::cerr << "Failed to clearRts " << ftdi_get_error_string(&m_handle)
                << std::endl;
      return false;
    }
    std::cout << "Opened ftdi ! " << std::endl;
    isOpen = true;
    return true;
  }

  bool setBreak(bool on) {
    if(!isOpen)return false;
    ftdi_break_type type;
    if (on == true)
      type = BREAK_ON;
    else
      type = BREAK_OFF;

    if (ftdi_set_line_property2(&m_handle, BITS_8, STOP_BIT_2, NONE, type) <
        0) {
      std::cerr << "Failed to set break: " << ftdi_get_error_string(&m_handle)
                << std::endl;
    isOpen = false;
      return false;
    } else {
      return true;
    }
  }

  bool isConnected() { return isOpen; }
  void close() {
    isOpen = false;
    if (ftdi_usb_close(&m_handle) < 0) {
      std::cerr << "closing failed" << ftdi_get_error_string(&m_handle)
                << std::endl;
    }
  }

  bool sendDMXFrame(const uint8_t *frame, size_t size) {
    if (!isOpen) {
      std::cerr << "device not open ignoring DMX frame "
                << ftdi_get_error_string(&m_handle) << std::endl;
      return false;
    }
    if (ftdi_write_data(&m_handle, frame, size) < 0) {
      std::cerr << "Failed to send DMX frame: "
                << ftdi_get_error_string(&m_handle) << std::endl;
     isOpen = false;
      return false;
    }
    return true;
  }

private:
  struct ftdi_context m_handle;
  const char *m_serial;
  uint16_t m_vid;
  uint16_t m_pid;

  bool isOpen = false;
};
