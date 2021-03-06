/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * This file defines a wrapper around libudev so we can avoid
 * linking directly to it and use dlopen instead.
 */

#ifndef HAL_LINUX_UDEV_H_
#define HAL_LINUX_UDEV_H_

#include <dlfcn.h>

namespace mozilla {

struct udev;
struct udev_device;
struct udev_enumerate;
struct udev_list_entry;
struct udev_monitor;

class udev_lib {
 public:
  udev_lib() : lib(dlopen("libudev.so.0", RTLD_LAZY | RTLD_GLOBAL)),
               udev(NULL) {
    if (lib && LoadSymbols())
      udev = udev_new();
  }

  ~udev_lib() {
    if (udev) {
      udev_unref(udev);
    }

    if (lib) {
      dlclose(lib);
    }
  }

  operator bool() {
    return udev;
  }

 private:
  bool LoadSymbols() {
#define DLSYM(s) \
  do { \
    s = (typeof(s))dlsym(lib, #s); \
    if (!s) return false; \
  } while (0)

    DLSYM(udev_new);
    DLSYM(udev_unref);
    DLSYM(udev_device_unref);
    DLSYM(udev_device_new_from_syspath);
    DLSYM(udev_device_get_devnode);
    DLSYM(udev_device_get_property_value);
    DLSYM(udev_device_get_action);
    DLSYM(udev_enumerate_new);
    DLSYM(udev_enumerate_unref);
    DLSYM(udev_enumerate_add_match_subsystem);
    DLSYM(udev_enumerate_scan_devices);
    DLSYM(udev_enumerate_get_list_entry);
    DLSYM(udev_list_entry_get_next);
    DLSYM(udev_list_entry_get_name);
    DLSYM(udev_monitor_new_from_netlink);
    DLSYM(udev_monitor_filter_add_match_subsystem_devtype);
    DLSYM(udev_monitor_enable_receiving);
    DLSYM(udev_monitor_get_fd);
    DLSYM(udev_monitor_receive_device);
    DLSYM(udev_monitor_unref);
#undef DLSYM
    return true;
  }

  void* lib;

 public:
  struct udev* udev;

  // Function pointers returned from dlsym.
  struct udev* (*udev_new)(void);
  void (*udev_unref)(struct udev*);

  void (*udev_device_unref)(struct udev_device*);
  struct udev_device* (*udev_device_new_from_syspath)(struct udev*,
                                                      const char*);
  const char* (*udev_device_get_devnode)(struct udev_device*);
  const char* (*udev_device_get_property_value)(struct udev_device*,
                                                const char*);
  const char* (*udev_device_get_action)(struct udev_device*);

  struct udev_enumerate* (*udev_enumerate_new)(struct udev*);
  void (*udev_enumerate_unref)(struct udev_enumerate*);
  int (*udev_enumerate_add_match_subsystem)(struct udev_enumerate*,
                                            const char*);
  int (*udev_enumerate_scan_devices)(struct udev_enumerate*);
  struct udev_list_entry* (*udev_enumerate_get_list_entry)
    (struct udev_enumerate*);

  struct udev_list_entry* (*udev_list_entry_get_next)(struct udev_list_entry *);
  const char* (*udev_list_entry_get_name)(struct udev_list_entry*);

  struct udev_monitor* (*udev_monitor_new_from_netlink)(struct udev*,
                                                        const char*);
  int (*udev_monitor_filter_add_match_subsystem_devtype)
    (struct udev_monitor*, const char*, const char*);
  int (*udev_monitor_enable_receiving)(struct udev_monitor*);
  int (*udev_monitor_get_fd)(struct udev_monitor*);
  struct udev_device* (*udev_monitor_receive_device)(struct udev_monitor*);
  void (*udev_monitor_unref)(struct udev_monitor*);
};

} // namespace mozilla

#endif // HAL_LINUX_UDEV_H_
