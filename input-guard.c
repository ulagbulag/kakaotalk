/* SPDX-License-Identifier: Unlicense */
#define _GNU_SOURCE
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/XI2.h>
#include <dlfcn.h>
#include <string.h>
#include <strings.h>

/* Wine consumes raw mouse events as well as the window-targeted core events.
 * In KakaoTalk, global raw events over other applications can act on the last
 * chat window, including SetFocus() and scrolling. Keep raw input only over
 * KakaoTalk or its Wine tray window, and during a drag started there.
 * Core events, keyboard input, and activation requests are left untouched.
 */
static _Thread_local unsigned int held_buttons;
typedef Bool (*event_predicate)(Display *, XEvent *, XPointer);

static int pointer_on_wine(Display *display, void *xlib)
{
    Bool (*query)(Display *, Window, Window *, Window *, int *, int *, int *, int *,
                  unsigned int *) = dlsym(xlib, "XQueryPointer");
    int (*class_hint)(Display *, Window, XClassHint *) = dlsym(xlib, "XGetClassHint");
    int (*free_data)(void *) = dlsym(xlib, "XFree");

    /* Leave input alone if it cannot be classified. */
    if (!query || !class_hint || !free_data) return 1;
    Window window = DefaultRootWindow(display), root, child;
    int rx, ry, wx, wy;
    unsigned int mask;

    /* Descend through window-manager frames and embedded tray containers. */
    for (int depth = 0; depth < 32; depth++) {
        if (!query(display, window, &root, &child, &rx, &ry, &wx, &wy, &mask)) return 1;
        if (!child) return 0;
        XClassHint hint = {0};
        class_hint(display, child, &hint);
        int match = hint.res_class && (!strcasecmp(hint.res_class, "kakaotalk.exe") ||
                                       !strcasecmp(hint.res_class, "explorer.exe"));
        if (hint.res_name) free_data(hint.res_name);
        if (hint.res_class) free_data(hint.res_class);
        if (match) return 1;
        window = child;
    }
    return 0;
}

Bool XCheckIfEvent(Display *display, XEvent *event, event_predicate predicate, XPointer arg)
{
    /* Resolve Xlib lazily: do not inject its dependency tree into every
     * subprocess inheriting LD_PRELOAD, including external native programs. */
    void *xlib = dlopen("libX11.so.6", RTLD_LAZY | RTLD_NOLOAD);
    if (!xlib) return False;
    Bool (*check)(Display *, XEvent *, event_predicate, XPointer) = dlsym(xlib, "XCheckIfEvent");
    Bool (*query)(Display *, const char *, int *, int *, int *) = dlsym(xlib, "XQueryExtension");
    Bool (*get_data)(Display *, XGenericEventCookie *) = dlsym(xlib, "XGetEventData");
    void (*free_data)(Display *, XGenericEventCookie *) = dlsym(xlib, "XFreeEventData");
    Dl_info caller;
    int wine = 0, opcode = -1, first_event, first_error;

    /* Native programs launched from KakaoTalk must retain their raw input. */
    if (dladdr(__builtin_return_address(0), &caller) && caller.dli_fname) {
        const char *name = strrchr(caller.dli_fname, '/');
        name = name ? name + 1 : caller.dli_fname;
        wine = !strcmp(name, "winex11.so") || !strcmp(name, "winex11.drv.so");
    }
    if (!check) {
        dlclose(xlib);
        return False;
    }
    Bool result;
    while ((result = check(display, event, predicate, arg))) {
        if (!wine || event->type != GenericEvent ||
            (event->xcookie.evtype != XI_RawMotion && event->xcookie.evtype != XI_RawButtonPress &&
             event->xcookie.evtype != XI_RawButtonRelease))
            break;
        if (!query || !get_data || !free_data ||
            !query(display, "XInputExtension", &opcode, &first_event, &first_error) ||
            event->xcookie.extension != opcode)
            break;

        /* Preserve releases and motion after a drag leaves the window. */
        if (held_buttons || pointer_on_wine(display, xlib)) {
            if (event->xcookie.evtype == XI_RawButtonPress) held_buttons++;
            if (event->xcookie.evtype == XI_RawButtonRelease && held_buttons) held_buttons--;
            break;
        }
        /* Claim and release discarded cookies. Leave delivered cookies for
         * Wine to claim once, as required by XGetEventData(). */
        if (get_data(display, &event->xcookie)) free_data(display, &event->xcookie);
    }
    dlclose(xlib);
    return result;
}
