// Copyright 2013 The Flutter Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef KEYBOARD_MAP_H_
#define KEYBOARD_MAP_H_

#include <xkbcommon/xkbcommon.h>

#define WLR_USE_UNSTABLE
#include <wlr/types/wlr_keyboard.h>

#ifdef __cplusplus
extern "C" {
#endif

/* inline uint64_t gpointer_to_uint64(gpointer pointer) {
  return pointer == nullptr ? 0 : reinterpret_cast<uint64_t>(pointer);
}

inline gpointer uint64_to_gpointer(uint64_t number) {
  return reinterpret_cast<gpointer>(number);
} */

//void initialize_modifier_bit_to_checked_keys(GHashTable* table);

//void initialize_lock_bit_to_checked_keys(GHashTable* table);

// Mask for the 32-bit value portion of the key code.
extern const uint64_t kValueMask;

// The plane value for keys which have a Unicode representation.
extern const uint64_t kUnicodePlane;

// The plane value for the private keys defined by the GTK embedding.
extern const uint64_t kGtkPlane;

uint64_t apply_id_plane(uint64_t logical_id, uint64_t plane);

uint64_t event_to_physical_key(struct wlr_event_keyboard_key *event);

uint64_t event_to_logical_key(struct wlr_event_keyboard_key *event, struct xkb_state *state);

uint32_t event_to_unicode(struct wlr_event_keyboard_key *event, struct xkb_state *state);

uint64_t event_to_timestamp(struct wlr_event_keyboard_key *event);

#ifdef __cplusplus
}
#endif

#endif  // KEYBOARD_MAP_H_