/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * The origin of this IDL file is
 * http://dev.w3.org/csswg/css-font-loading/#FontFaceSet-interface
 *
 * Copyright © 2014 W3C® (MIT, ERCIM, Keio, Beihang), All Rights Reserved. W3C
 * liability, trademark and document use rules apply.
 */

// To implement FontFaceSet's iterator until we can use setlike.
dictionary FontFaceSetIteratorResult
{
  required any value;
  required boolean done;
};

// To implement FontFaceSet's iterator until we can use setlike.
[NoInterfaceObject,
 Exposed=Window]
interface FontFaceSetIterator {
  [Throws] FontFaceSetIteratorResult next();
};

callback FontFaceSetForEachCallback = void (FontFace value, FontFace key, FontFaceSet set);

enum FontFaceSetLoadStatus { "loading", "loaded" };

[Pref="layout.css.font-loading-api.enabled",
 Exposed=Window]
interface FontFaceSet : EventTarget {
  // Bug 1072762 is for the FontFaceSet constructor.
  // constructor(sequence<FontFace> initialFaces);

  // Emulate setlike behavior until we can use that directly.
  readonly attribute unsigned long size;
  [Throws] void add(FontFace font);
  boolean has(FontFace font);
  boolean delete(FontFace font);
  void clear();
  [NewObject] FontFaceSetIterator entries();
  // Iterator keys();
  [NewObject, Alias=keys, Alias="@@iterator"] FontFaceSetIterator values();
  [Throws] void forEach(FontFaceSetForEachCallback cb, optional any thisArg);

  // -- events for when loading state changes
  attribute EventHandler onloading;
  attribute EventHandler onloadingdone;
  attribute EventHandler onloadingerror;

  // check and start loads if appropriate
  // and fulfill promise when all loads complete
  [NewObject] Promise<sequence<FontFace>> load(UTF8String font, optional DOMString text = " ");

  // return whether all fonts in the fontlist are loaded
  // (does not initiate load if not available)
  [Throws] boolean check(UTF8String font, optional DOMString text = " ");

  // async notification that font loading and layout operations are done
  [Throws] readonly attribute Promise<void> ready;

  // loading state, "loading" while one or more fonts loading, "loaded" otherwise
  readonly attribute FontFaceSetLoadStatus status;
};
