/* SPDX-License-Identifier: Unlicense */
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/XInput2.h>
#include <X11/extensions/XTest.h>
#include <assert.h>
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* Run only in the private Xvfb display created by input-guard.sh. */
typedef Bool (*poll_fn)(Display *, XEvent *);
static Display *d, *control;
static poll_fn poll_wine;
static int raw_press, raw_release, raw_motion, core_press, core_release;
static Bool any(Display *display, XEvent *event, XPointer arg)
{
    (void)display;
    (void)event;
    (void)arg;
    return True;
}
static void drain(int native)
{
    XEvent e;
    XSync(control, False);
    XSync(d, False);
    raw_press = raw_release = raw_motion = core_press = core_release = 0;
    while (native ? XCheckIfEvent(d, &e, any, NULL) : poll_wine(d, &e)) {
        if (e.type == ButtonPress) core_press++;
        if (e.type == ButtonRelease) core_release++;
        if (e.type == GenericEvent && XGetEventData(d, &e.xcookie)) {
            if (e.xcookie.evtype == XI_RawButtonPress) raw_press++;
            if (e.xcookie.evtype == XI_RawButtonRelease) raw_release++;
            if (e.xcookie.evtype == XI_RawMotion) raw_motion++;
            XFreeEventData(d, &e.xcookie);
        }
    }
}
static void move(int x, int y)
{
    XTestFakeMotionEvent(control, DefaultScreen(control), x, y, CurrentTime);
    drain(0);
}
static void button(unsigned int b, int down, int expected, int native)
{
    XTestFakeButtonEvent(control, b, down, CurrentTime);
    drain(native);
    assert((down ? raw_press : raw_release) == expected);
    assert((down ? core_press : core_release) == 1);
}
int main(int argc, char **argv)
{
    assert(argc == 2);
    void *driver = dlopen(argv[1], RTLD_NOW);
    assert(driver);
    poll_wine = dlsym(driver, "poll_wine");
    assert(poll_wine);
    d = XOpenDisplay(NULL);
    control = XOpenDisplay(NULL);
    assert(d && control);
    int major = 2, minor = 2;
    assert(XIQueryVersion(d, &major, &minor) == Success);
    Window root = DefaultRootWindow(d);
    for (int i = 0; i < 3; i++) {
        Window w = XCreateSimpleWindow(d, root, 10 + i * 200, 10, 180, 180, 0, 0, 0xffffff);
        XClassHint hint = {.res_name = "test", .res_class = i ? "other-app" : "kakaotalk.exe"};
        XSetClassHint(d, w, &hint);
        XSelectInput(d, w, ButtonPressMask | ButtonReleaseMask | PointerMotionMask);
        XMapWindow(d, w);
    }
    unsigned char bits[XIMaskLen(XI_LASTEVENT)] = {0};
    XISetMask(bits, XI_RawMotion);
    XISetMask(bits, XI_RawButtonPress);
    XISetMask(bits, XI_RawButtonRelease);
    XIEventMask mask = {.deviceid = XIAllMasterDevices, .mask_len = sizeof bits, .mask = bits};
    assert(XISelectEvents(d, root, &mask, 1) == Success);
    XSync(d, False);
    drain(0);
    move(50, 50);
    button(1, True, 1, 0);
    button(1, False, 1, 0);
    move(50, 50);
    for (unsigned int b = 2; b <= 9; b++) {
        button(b, True, 1, 0);
        button(b, False, 1, 0);
    }
    puts("PASS: direct Wine left/right/middle/side clicks and wheels preserve raw and core events");
    for (int i = 0; i < 20; i++) {
        move(50, 50);
        button(1, True, 1, 0);
        button(1, False, 1, 0);
        move(250, 50);
        button(1, True, 0, 0);
        button(1, False, 0, 0);
        move(450, 50);
        button(1, True, 0, 0);
        button(1, False, 0, 0);
        for (unsigned int b = 2; b <= 9; b++) {
            button(b, True, 0, 0);
            button(b, False, 0, 0);
        }
    }
    puts("PASS: 20 Wine -> A -> B click/wheel cycles block background raw input");
    move(50, 50);
    button(1, True, 1, 0);
    move(450, 50);
    assert(raw_motion > 0);
    button(1, False, 1, 0);
    button(1, True, 0, 0);
    button(1, False, 0, 0);
    puts("PASS: drag retains motion and release; next outside click is blocked");
    button(1, True, 1, 1);
    button(1, False, 1, 1);
    puts("PASS: non-Wine Xlib caller receives unchanged raw input");
    XCloseDisplay(control);
    XCloseDisplay(d);
    dlclose(driver);
    return 0;
}
