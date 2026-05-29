# Security and Stability Audit

This document outlines the findings of a comprehensive security and stability audit performed on the `desktop_pet` codebase. 

## 1. Core Application Logic (`src/main.c`)

### 1.1. Missing NULL Check for Window Creation
**Vulnerability / Stability Issue:** 
In `main.c`, the application destroys `temp_win` and creates the primary application window `win`. However, it fails to verify if `window_create` returned a valid pointer before proceeding to call `window_resize`, `window_set_input_region`, and entering the main loop.
**Impact:** 
If the window system fails to create the primary window, the application will attempt a NULL pointer dereference, causing an immediate segmentation fault (crash).
**Fix:**
Add a NULL check immediately after `window_create` for the main window.

## 2. Windowing System (`src/window/`)

### 2.1. Linux: Missing Window Destruction (`linux_window.c`)
**Memory/Resource Leak:**
The `window_destroy` function in `linux_window.c` closes the X11 Display connection via `XCloseDisplay` and frees the `window_t` struct, but it neglects to explicitly destroy the underlying X11 window using `XDestroyWindow`.
**Impact:**
While the X server typically cleans up resources when a client disconnects, explicitly destroying the window is best practice and prevents temporary resource leaks on the X server side.
**Fix:**
Add `XDestroyWindow` before closing the display.

### 2.2. Linux: Unclosed uinput File Descriptor (`linux_window.c`)
**Resource Leak:**
`uinput_init` opens `/dev/uinput` and assigns it to `uinput_fd`. There is no corresponding function to close this file descriptor when the application exits.
**Impact:**
File descriptor leak.
**Fix:**
Implement a cleanup routine to close `uinput_fd`.

### 2.3. Linux: Unclosed Dedicated Query Display (`linux_window.c`)
**Resource Leak:**
A secondary X11 display connection `query_display` is opened for robust mouse tracking, but it is never closed.
**Impact:**
X11 connection leak.
**Fix:**
Implement a cleanup function for `query_display`.

## 3. Renderer (`src/renderer/renderer.c`)

### 3.1. Linux: Unfreed Graphics Context (`global_gc`)
**Memory/Resource Leak:**
In `renderer_draw_image`, `global_gc` is created via `XCreateGC` if it does not exist. However, there is no corresponding `XFreeGC` call when the application tears down the renderer.
**Impact:**
Memory leak on the X Server.
**Fix:**
Add a `renderer_cleanup(window_t* win)` function that frees `global_gc`.

### 3.2. Unchecked `malloc` for Scaled Pixels
**Stability Issue:**
The renderer allocates memory for scaled pixels (`malloc(sw * sh * 4)`). It does not verify if `malloc` succeeded before using the memory.
**Impact:**
If the system is under heavy memory pressure, `malloc` could return NULL, leading to a segmentation fault during the scaling loop.
**Fix:**
Add a NULL check.

## 4. System Tray Integration (`src/tray/`)

### 4.1. Linux: Memory Leak in Tray Items (`linux_tray.c`)
**Memory Leak:**
In `tray_add_item`, memory is allocated for `struct tray_item_data` to hold the callback and user data. When `tray_destroy` is called, it destroys the GTK menu, but the dynamically allocated `tray_item_data` structs are never freed.
**Impact:**
Memory leak during application lifecycle.
**Fix:**
Maintain a list of allocated pointers or attach them to the GTK widget lifecycle.

## 5. Logging Subsystem (`src/logger/logger.h`)

### 5.1. Inefficient File I/O
**Stability/Performance Issue:**
The `_log_msg` function opens, writes to, and closes the log file on *every single log call*.
**Impact:**
This causes significant disk I/O overhead and performance degradation, especially inside the 60FPS main loop.
**Fix:**
Keep the file stream open and flush it periodically, then close it cleanly upon application exit.

### 5.2. Potential Format String Vulnerability
**Security Vulnerability:**
The logging macros act as wrappers around `vfprintf`. If a user-controlled string is passed directly as the `format` argument, it would result in a format string vulnerability.
**Fix:**
Ensure strict linting (e.g., `-Wformat -Werror=format-security`) in `meson.build`.

## 6. OS & Permissions (`src/window/linux_window.c`)

### 6.1. Excessive Permissions for `uinput`
**Security Risk:**
The app requires write access to `/dev/uinput` to move the mouse on Linux. Users might be tempted to run the app as `root` or with `sudo` to satisfy this.
**Impact:**
Running a UI app with X11/GTK dependencies as `root` is a major security risk. Any vulnerability in the app (like the format string one) could lead to full system compromise.
**Fix:**
Implement a udev rule recommendation in the README to allow the current user access to `uinput`, and explicitly check/warn if the app is running as root.

## 7. Asset Loading (`src/renderer/renderer.c`)

### 7.1. Path Traversal & Malicious Assets
**Security Risk:**
`image_load` takes a filename. While the app currently hardcodes paths, if this were ever exposed to user configuration, it could be used for path traversal. Furthermore, using `stb_image` (a single-header library) for parsing untrusted PNGs is risky as it is prone to buffer overflows.
**Impact:**
Arbitrary file read or remote code execution via a specially crafted image file.
**Fix:**
Validate all asset paths. Use a more robust, system-provided library like `libpng` or `gdk-pixbuf` which receive more frequent security updates than `stb_image`.

