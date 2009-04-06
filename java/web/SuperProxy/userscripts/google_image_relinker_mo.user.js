// ==UserScript==
// @name          Google Image Relinker Mod
// @namespace     http://userscripts.org/users/9022
// @description   http://userscripts.org/scripts/show/5059
// @version       0.5
// @date          2009-03-20
// @copyright     2006-2009, thorbenhauer
// @license       GPL 2 or later
// @include       http://images.google.tld/images?*
//                for Opera (which doesn't understand tld):
// @include       http://images.google.com/images?*
// @include       http://images.google.de/images?*
// ==/UserScript==
//
// -----------------------------------------------------------------------------
//
// Based on Google Image Relinker user script by Patrick Cavit
// http://userscripts.org/users/187
// Script location: http://userscripts.org/scripts/show/792 
//
// Copyright Notice by Patrick Cavit, pcavit@gmail.com:
// Copy, use, modify, spread as you see fit. Massive thanks go out to
// Eric Hamiter, this code is just a quick modification of his extension at
// http://roachfiend.com/
//
// With Modifications inspired by
// FurYy http://userscripts.org/users/1618
// Juhani Naskali http://userscripts.org/users/8345
// ekbworldwide http://userscripts.org/users/39581
//
// -----------------------------------------------------------------------------
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
// -----------------------------------------------------------------------------

(function () { // function wrapper for Opera

/* sites for which the blogger.com and blogspot.com content-disposition HTTP
   response header workaround is enabled - use regular expressions here */
var sites = new Array(
//  /.*/, // CATCH ALL rule
  /^http:\/\/(\d\.)?bp\d?\.blog(spot|ger)\.com\//,
  /^http:\/\/img(\d)?\.blogs\.yahoo\.co\.jp\//
);
    
/* blogger.com and blogspot.com content-disposition HTTP response header
   workaround */
var q = document.evaluate("//input[@name = 'q']/@value", document, null,
  XPathResult.FIRST_ORDERED_NODE_TYPE, null).singleNodeValue;
if (q != null) {
  q = q.value;
  if (q.indexOf(":girm:") == 0) {
    var s = q.split(":girm:");
    var url = s[1].replace(/link:/, '');
    var context = s[2].replace(/context:/, '');
    document.body = document.createElement("body");
    document.body.innerHTML = "<a href=\"" + context +
      "\" style=\"color: white\" title=\"Click to see the full site\">" +
      "<img src=\"" + url + "\"></a>";
    document.getElementsByTagName("head")[0].innerHTML = "";
    return;    
  }
}
    
createLinks();
window.addEventListener("resize", createLinks, true);

function createLinks() {
  var googLinks = document.evaluate("//td/a[contains(@href, " +
    "'/imgres?imgurl=')][contains(@href, '&imgrefurl=')]", document, null,
    XPathResult.ORDERED_NODE_SNAPSHOT_TYPE, null);
  var googFonts = document.evaluate("//td/font/span[@class = 'a']|" +
    "//font[contains(@color, '#008000')]", document, null,
    XPathResult.ORDERED_NODE_SNAPSHOT_TYPE, null);
  var link, gmatch, font, newDiv, newLink, contextLink, imgLink, domain;       
  for (var i = 0; i < googLinks.snapshotLength; i++) {
    link = googLinks.snapshotItem(i);
    font = googFonts.snapshotItem(i);
    gmatch = link.href.match(/&imgrefurl=(.*?)(&start=|&h=|&usg=)/);
    contextLink = decodeURIComponent(gmatch[1]);
    domain = contextLink.match(/:\/\/([^\/]+)\//)[1];
    font.innerHTML = "<a href=\"" + contextLink + "\">" + domain + "</a>";
    if (font.parentNode.getAttribute("framedView") == null) {
      newDiv = document.createElement("div");
      newDiv.setAttribute("style", "margin-top: 2px; font-size: x-small;");
      newLink = document.createElement("a");
      font.parentNode.setAttribute("framedView", "set");
      newLink.href = link.href;
      newLink.innerHTML = "Framed View";
      newDiv.appendChild(newLink);
      font.parentNode.appendChild(newDiv);
    }
    gmatch = link.href.match(/\/imgres\?imgurl=(.*?)&imgrefurl=/);
    imgLink = decodeURIComponent(gmatch[1]);
    /* blogger.com and blogspot.com content-disposition HTTP response header
       workaround */
    for (var j = 0; j < sites.length; j++) {
      if (imgLink.search(sites[j]) > -1) {
        imgLink = "http://images.google.com/images?gbv=1&q=" +
          ":girm:link:" + imgLink + ":girm:context:" +
        decodeURIComponent(contextLink);
        break;
      }
    }
    link.href = imgLink;
  }
}

})(); // function wrapper for Opera