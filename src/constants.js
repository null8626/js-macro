"use strict";

module.exports.windowStyles = Object.entries({
    border: 0x00800000,
    caption: 0x00C00000,
    child: 0x40000000,
    clipChildren: 0x02000000,
    clipSiblings: 0x04000000,
    disabled: 0x08000000,
    dialog: 0x00400000,
    horizontalScroll: 0x00100000,
    maximized: 0x01000000,
    maximizeButton: 0x00010000,
    minimized: 0x20000000,
    minimizeButton: 0x00020000,
    overlapped: 0x00000000,
    popup: 0x80000000,
    sysMenu: 0x00080000,
    tiled: 0x00000000,
    visible: 0x10000000,
    virtualScroll: 0x00200000
});

module.exports.extendedWindowStyles = Object.entries({
    acceptFiles: 0x00000010,
    appWindow: 0x00040000,
    clientEdge: 0x00000200,
    composited: 0x02000000,
    contextHelp: 0x00000400,
    controlParent: 0x00010000,
    dialogModalFrame: 0x00000001,
    layered: 0x00080000,
    layoutRT: 0x00400000,
    left: 0x00000000,
    leftScrollBar: 0x00004000,
    mdiChild: 0x00000040,
    noActivate: 0x08000000,
    noInheritLayout: 0x00100000,
    noParentNotify: 0x00000004,
    noRedirectionBitmap: 0x00200000,
    right: 0x00001000,
    RTLreading: 0x00002000,
    staticEdge: 0x00020000,
    toolWindow: 0x00000080,
    topMost: 0x00000008
});