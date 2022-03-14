import 'dart:collection';

import 'package:compositor_dart/compositor_dart.dart';
import 'package:compositor_dart/constants.dart';
import 'package:flutter/gestures.dart';
import 'package:flutter/material.dart';
import 'package:flutter/rendering.dart';
import 'package:flutter/services.dart';

typedef _OnWidgetSizeChange = void Function(Size? size);

class _MeasureSizeRenderObject extends RenderProxyBox {
  Size? oldSize;
  final _OnWidgetSizeChange onChange;

  _MeasureSizeRenderObject(this.onChange);

  @override
  void performLayout() {
    super.performLayout();

    Size? newSize = child?.size;
    if (oldSize == newSize) return;

    oldSize = newSize;
    WidgetsBinding.instance.addPostFrameCallback((_) {
      onChange(newSize);
    });
  }
}

class _MeasureSize extends SingleChildRenderObjectWidget {
  final _OnWidgetSizeChange onChange;

  const _MeasureSize({
    Key? key,
    required this.onChange,
    required Widget child,
  }) : super(key: key, child: child);

  @override
  RenderObject createRenderObject(BuildContext context) {
    return _MeasureSizeRenderObject(onChange);
  }
}

class SurfaceView extends StatefulWidget {
  final Surface surface;

  const SurfaceView({Key? key, required this.surface}) : super(key: key);

  @override
  State<StatefulWidget> createState() {
    return _SurfaceViewState();
  }
}

class _SurfaceViewState extends State<SurfaceView> {
  late _CompositorPlatformViewController controller;

  @override
  void initState() {
    super.initState();
    controller = _CompositorPlatformViewController(surface: widget.surface);
  }

  @override
  void didUpdateWidget(SurfaceView oldWidget) {
    super.didUpdateWidget(oldWidget);
    if (oldWidget.surface != widget.surface) {
      controller.dispose();
      controller = _CompositorPlatformViewController(surface: widget.surface);
    }
  }

  @override
  Widget build(BuildContext context) {
    return Listener(
      onPointerSignal: (event) {
        controller.dispatchPointerEvent(event);
      },
      child: Focus(
        onKeyEvent: (node, event) {
          final KeyStatus status;

          if (event is KeyDownEvent) {
            status = KeyStatus.pressed;
          } else {
            status = KeyStatus.released;
          }

          int? keycode = phyisicalToXkbMap[event.physicalKey.usbHidUsage];

          if (keycode != null) {
            compositor.platform.surfaceSendKey(
              widget.surface,
              keycode,
              status,
              event.timeStamp,
            );

            return KeyEventResult.handled;
          }

          return KeyEventResult.ignored;
        },
        child: _MeasureSize(
          onChange: (size) {
            if (size != null) {
              controller.setSize(size);
            }
          },
          child: PlatformViewSurface(
            controller: controller,
            hitTestBehavior: PlatformViewHitTestBehavior.opaque,
            gestureRecognizers: HashSet(),
          ),
        ),
      ),
    );
  }
}

class _CompositorPlatformViewController extends PlatformViewController {
  Surface surface;
  Size size = const Size(100, 100);

  _CompositorPlatformViewController({required this.surface});

  void setSize(Size size) {
    this.size = size;
    compositor.platform.surfaceToplevelSetSize(
        surface, size.width.round(), size.height.round());
  }

  @override
  Future<void> clearFocus() => compositor.platform.clearFocus(surface);

  @override
  Future<void> dispatchPointerEvent(PointerEvent event) async {
    //print("${event.toString()}");

    final int deviceKind;
    switch (event.kind) {
      case PointerDeviceKind.mouse:
        deviceKind = pointerKindMouse;
        break;
      case PointerDeviceKind.touch:
        deviceKind = pointerKindTouch;
        break;
      default:
        deviceKind = pointerKindUnknown;
        break;
    }

    final int eventType;
    Offset scrollAmount = Offset.zero;

    if (event is PointerDownEvent) {
      eventType = pointerDownEvent;
    } else if (event is PointerUpEvent) {
      eventType = pointerUpEvent;
    } else if (event is PointerHoverEvent) {
      eventType = pointerHoverEvent;
    } else if (event is PointerMoveEvent) {
      eventType = pointerMoveEvent;
    } else if (event is PointerEnterEvent) {
      eventType = pointerEnterEvent;
    } else if (event is PointerExitEvent) {
      eventType = pointerExitEvent;
    } else if (event is PointerScrollEvent) {
      eventType = pointerScrollEvent;
      scrollAmount = event.scrollDelta;
    } else {
      eventType = pointerUnknownEvent;
    }

    print(event.buttons);

    List data = [
      surface.handle,
      event.buttons,
      event.delta.dx,
      event.delta.dy,
      event.device,
      event.distance,
      event.down,
      event.embedderId,
      deviceKind,
      event.localDelta.dx,
      event.localDelta.dy,
      event.localPosition.dx,
      event.localPosition.dy,
      event.obscured,
      event.orientation,
      event.platformData,
      event.pointer,
      event.position.dx,
      event.position.dy,
      event.pressure,
      event.radiusMajor,
      event.radiusMinor,
      event.size,
      event.synthesized,
      event.tilt,
      event.timeStamp.inMicroseconds,
      eventType,
      size.width,
      size.height,
      scrollAmount.dx,
      scrollAmount.dy,
    ];

    //print("pointerevent $data");

    await compositor.platform.channel.invokeMethod(
      "surface_pointer_event",
      data,
    );
  }

  @override
  Future<void> dispose() async {
    // TODO: implement dispose
    //throw UnimplementedError();
  }

  @override
  int get viewId => surface.handle;
}

const Map<int, int> phyisicalToXkbMap = {
  0x00070029: 0x00000009, // escape
  0x0007001e: 0x0000000a, // digit1
  0x0007001f: 0x0000000b, // digit2
  0x00070020: 0x0000000c, // digit3
  0x00070021: 0x0000000d, // digit4
  0x00070022: 0x0000000e, // digit5
  0x00070023: 0x0000000f, // digit6
  0x00070024: 0x00000010, // digit7
  0x00070025: 0x00000011, // digit8
  0x00070026: 0x00000012, // digit9
  0x00070027: 0x00000013, // digit0
  0x0007002d: 0x00000014, // minus
  0x0007002e: 0x00000015, // equal
  0x0007002a: 0x00000016, // backspace
  0x0007002b: 0x00000017, // tab
  0x00070014: 0x00000018, // keyQ
  0x0007001a: 0x00000019, // keyW
  0x00070008: 0x0000001a, // keyE
  0x00070015: 0x0000001b, // keyR
  0x00070017: 0x0000001c, // keyT
  0x0007001c: 0x0000001d, // keyY
  0x00070018: 0x0000001e, // keyU
  0x0007000c: 0x0000001f, // keyI
  0x00070012: 0x00000020, // keyO
  0x00070013: 0x00000021, // keyP
  0x0007002f: 0x00000022, // bracketLeft
  0x00070030: 0x00000023, // bracketRight
  0x00070028: 0x00000024, // enter
  0x000700e0: 0x00000025, // controlLeft
  0x00070004: 0x00000026, // keyA
  0x00070016: 0x00000027, // keyS
  0x00070007: 0x00000028, // keyD
  0x00070009: 0x00000029, // keyF
  0x0007000a: 0x0000002a, // keyG
  0x0007000b: 0x0000002b, // keyH
  0x0007000d: 0x0000002c, // keyJ
  0x0007000e: 0x0000002d, // keyK
  0x0007000f: 0x0000002e, // keyL
  0x00070033: 0x0000002f, // semicolon
  0x00070034: 0x00000030, // quote
  0x00070035: 0x00000031, // backquote
  0x000700e1: 0x00000032, // shiftLeft
  0x00070031: 0x00000033, // backslash
  0x0007001d: 0x00000034, // keyZ
  0x0007001b: 0x00000035, // keyX
  0x00070006: 0x00000036, // keyC
  0x00070019: 0x00000037, // keyV
  0x00070005: 0x00000038, // keyB
  0x00070011: 0x00000039, // keyN
  0x00070010: 0x0000003a, // keyM
  0x00070036: 0x0000003b, // comma
  0x00070037: 0x0000003c, // period
  0x00070038: 0x0000003d, // slash
  0x000700e5: 0x0000003e, // shiftRight
  0x00070055: 0x0000003f, // numpadMultiply
  0x000700e2: 0x00000040, // altLeft
  0x0007002c: 0x00000041, // space
  0x00070039: 0x00000042, // capsLock
  0x0007003a: 0x00000043, // f1
  0x0007003b: 0x00000044, // f2
  0x0007003c: 0x00000045, // f3
  0x0007003d: 0x00000046, // f4
  0x0007003e: 0x00000047, // f5
  0x0007003f: 0x00000048, // f6
  0x00070040: 0x00000049, // f7
  0x00070041: 0x0000004a, // f8
  0x00070042: 0x0000004b, // f9
  0x00070043: 0x0000004c, // f10
  0x00070053: 0x0000004d, // numLock
  0x00070047: 0x0000004e, // scrollLock
  0x0007005f: 0x0000004f, // numpad7
  0x00070060: 0x00000050, // numpad8
  0x00070061: 0x00000051, // numpad9
  0x00070056: 0x00000052, // numpadSubtract
  0x0007005c: 0x00000053, // numpad4
  0x0007005d: 0x00000054, // numpad5
  0x0007005e: 0x00000055, // numpad6
  0x00070057: 0x00000056, // numpadAdd
  0x00070059: 0x00000057, // numpad1
  0x0007005a: 0x00000058, // numpad2
  0x0007005b: 0x00000059, // numpad3
  0x00070062: 0x0000005a, // numpad0
  0x00070063: 0x0000005b, // numpadDecimal
  0x00070094: 0x0000005d, // lang5
  0x00070064: 0x0000005e, // intlBackslash
  0x00070044: 0x0000005f, // f11
  0x00070045: 0x00000060, // f12
  0x00070087: 0x00000061, // intlRo
  0x00070092: 0x00000062, // lang3
  0x00070093: 0x00000063, // lang4
  0x0007008a: 0x00000064, // convert
  0x00070088: 0x00000065, // kanaMode
  0x0007008b: 0x00000066, // nonConvert
  0x00070058: 0x00000068, // numpadEnter
  0x000700e4: 0x00000069, // controlRight
  0x00070054: 0x0000006a, // numpadDivide
  0x00070046: 0x0000006b, // printScreen
  0x000700e6: 0x0000006c, // altRight
  0x0007004a: 0x0000006e, // home
  0x00070052: 0x0000006f, // arrowUp
  0x0007004b: 0x00000070, // pageUp
  0x00070050: 0x00000071, // arrowLeft
  0x0007004f: 0x00000072, // arrowRight
  0x0007004d: 0x00000073, // end
  0x00070051: 0x00000074, // arrowDown
  0x0007004e: 0x00000075, // pageDown
  0x00070049: 0x00000076, // insert
  0x0007004c: 0x00000077, // delete
  0x0007007f: 0x00000079, // audioVolumeMute
  0x00070081: 0x0000007a, // audioVolumeDown
  0x00070080: 0x0000007b, // audioVolumeUp
  0x00070066: 0x0000007c, // power
  0x00070067: 0x0000007d, // numpadEqual
  0x000700d7: 0x0000007e, // numpadSignChange
  0x00070048: 0x0000007f, // pause
  0x000c029f: 0x00000080, // showAllWindows
  0x00070085: 0x00000081, // numpadComma
  0x00070090: 0x00000082, // lang1
  0x00070091: 0x00000083, // lang2
  0x00070089: 0x00000084, // intlYen
  0x000700e3: 0x00000085, // metaLeft
  0x000700e7: 0x00000086, // metaRight
  0x00070065: 0x00000087, // contextMenu
  0x000c0226: 0x00000088, // browserStop
  0x00070079: 0x00000089, // again
  0x0007007a: 0x0000008b, // undo
  0x00070077: 0x0000008c, // select
  0x0007007c: 0x0000008d, // copy
  0x00070074: 0x0000008e, // open
  0x0007007d: 0x0000008f, // paste
  0x0007007e: 0x00000090, // find
  0x0007007b: 0x00000091, // cut
  0x00070075: 0x00000092, // help
  0x000c0192: 0x00000094, // launchApp2
  0x00010082: 0x00000096, // sleep
  0x00010083: 0x00000097, // wakeUp
  0x000c0194: 0x00000098, // launchApp1
  0x000c0196: 0x0000009e, // launchInternetBrowser
  0x000c019e: 0x000000a0, // lockScreen
  0x000c018a: 0x000000a3, // launchMail
  0x000c022a: 0x000000a4, // browserFavorites
  0x000c0224: 0x000000a6, // browserBack
  0x000c0225: 0x000000a7, // browserForward
  0x000c00b8: 0x000000a9, // eject
  0x000c00b5: 0x000000ab, // mediaTrackNext
  0x000c00cd: 0x000000ac, // mediaPlayPause
  0x000c00b6: 0x000000ad, // mediaTrackPrevious
  0x000c00b7: 0x000000ae, // mediaStop
  0x000c00b2: 0x000000af, // mediaRecord
  0x000c00b4: 0x000000b0, // mediaRewind
  0x000c008c: 0x000000b1, // launchPhone
  0x000c0183: 0x000000b3, // mediaSelect
  0x000c0223: 0x000000b4, // browserHome
  0x000c0227: 0x000000b5, // browserRefresh
  0x000c0094: 0x000000b6, // exit
  0x000700b6: 0x000000bb, // numpadParenLeft
  0x000700b7: 0x000000bc, // numpadParenRight
  0x000c0201: 0x000000bd, // newKey
  0x000c0279: 0x000000be, // redo
  0x00070068: 0x000000bf, // f13
  0x00070069: 0x000000c0, // f14
  0x0007006a: 0x000000c1, // f15
  0x0007006b: 0x000000c2, // f16
  0x0007006c: 0x000000c3, // f17
  0x0007006d: 0x000000c4, // f18
  0x0007006e: 0x000000c5, // f19
  0x0007006f: 0x000000c6, // f20
  0x00070070: 0x000000c7, // f21
  0x00070071: 0x000000c8, // f22
  0x00070072: 0x000000c9, // f23
  0x00070073: 0x000000ca, // f24
  0x000c00b1: 0x000000d1, // mediaPause
  0x000c0203: 0x000000d6, // close
  0x000c00b0: 0x000000d7, // mediaPlay
  0x000c00b3: 0x000000d8, // mediaFastForward
  0x000c00e5: 0x000000d9, // bassBoost
  0x000c0208: 0x000000da, // print
  0x000c0221: 0x000000e1, // browserSearch
  0x000c0070: 0x000000e8, // brightnessDown
  0x000c006f: 0x000000e9, // brightnessUp
  0x000100b5: 0x000000eb, // displayToggleIntExt
  0x000c007a: 0x000000ed, // kbdIllumDown
  0x000c0079: 0x000000ee, // kbdIllumUp
  0x000c028c: 0x000000ef, // mailSend
  0x000c0289: 0x000000f0, // mailReply
  0x000c028b: 0x000000f1, // mailForward
  0x000c0207: 0x000000f2, // save
  0x000c01a7: 0x000000f3, // launchDocuments
  0x000c0075: 0x000000fc, // brightnessAuto
  0x000c0060: 0x0000016e, // info
  0x000c008d: 0x00000172, // programGuide
  0x000c0061: 0x0000017a, // closedCaptionToggle
  0x000c0232: 0x0000017c, // zoomToggle
  0x000c01ae: 0x0000017e, // launchKeyboardLayout
  0x000c01b7: 0x00000190, // launchAudioBrowser
  0x000c018e: 0x00000195, // launchCalendar
  0x000c0083: 0x0000019d, // mediaLast
  0x000c009c: 0x000001a2, // channelUp
  0x000c009d: 0x000001a3, // channelDown
  0x000c022d: 0x000001aa, // zoomIn
  0x000c022e: 0x000001ab, // zoomOut
  0x000c0184: 0x000001ad, // launchWordProcessor
  0x000c0186: 0x000001af, // launchSpreadsheet
  0x000c018d: 0x000001b5, // launchContacts
  0x000c0072: 0x000001b7, // brightnessToggle
  0x000c01ab: 0x000001b8, // spellCheck
  0x000c019c: 0x000001b9, // logOff
  0x000c019f: 0x0000024b, // launchControlPanel
  0x000c01a2: 0x0000024c, // selectTask
  0x000c01b1: 0x0000024d, // launchScreenSaver
  0x000c00cf: 0x0000024e, // speechInputToggle
  0x000c01cb: 0x0000024f, // launchAssistant
  0x000c029d: 0x00000250, // keyboardLayoutSelect
  0x000c0073: 0x00000258, // brightnessMinimum
  0x000c0074: 0x00000259, // brightnessMaximum
  0x00000017: 0x00000281, // privacyScreenToggle
};
