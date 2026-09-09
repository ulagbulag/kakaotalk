/* SPDX-License-Identifier: Unlicense */
/* Mimic the call site in Wine: the helper scopes itself to the driver module. */
#include <X11/Xlib.h>
static Bool any(Display *d, XEvent *e, XPointer p)
{
    (void)d;
    (void)e;
    (void)p;
    return True;
}
Bool poll_wine(Display *d, XEvent *e) { return XCheckIfEvent(d, e, any, 0); }
