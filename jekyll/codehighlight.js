/*
 * @licstart  The following is the entire license notice for the
 * JavaScript code in this tag.
 *
 * Copyright (C) 2012-2013 Free Software Foundation, Inc.
 *
 * The JavaScript code in this tag is free software: you can
 * redistribute it and/or modify it under the terms of the GNU
 * General Public License (GNU GPL) as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version.  The code is distributed WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU GPL for more details.
 *
 * As additional permission under GNU GPL version 3 section 7, you
 * may distribute non-source (e.g., minimized or compacted) forms of
 * that code without the copy of the GNU GPL normally required by
 * section 4, provided you include this license notice and a URL
 * through which recipients can access the Corresponding Source.
 *
 *
 * @licend  The above is the entire license notice
 * for the JavaScript code in this tag.
*/

function CodeHighlightOn(elem, id) {
    var target = document.getElementById(id);
    if (null != target) {
        elem.cacheClassElem = elem.className;
        elem.cacheClassTarget = target.className;
        target.className = "code-highlighted";
        elem.className = "code-highlighted";
    }
}

function CodeHighlightOff(elem, id) {
    var target = document.getElementById(id);

    if (elem.cacheClassElem) {
        elem.className = elem.cacheClassElem;
    }

    if (elem.cacheClassTarget) {
        target.className = elem.cacheClassTarget;
    }
}
