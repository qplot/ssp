/* $Id: mysite_links.js,v 1.2 2008/02/06 00:38:47 agentken Exp $ */

/**
 * Modified from Suckerfish Dropdowns, www.htmldog.com
 * Suckerfish by Patrick Griffiths and Dan Webb.
 *
 * IE fix.
 */
mysiteHover = function() {
  var linksReady = '';
  var sfEls = '';
  linksReady = document.getElementById("mysite-links");
  if (linksReady) {
    sfEld = document.getElementById("mysite-links").getElementsByTagName("LI");
    if (sfEls) {
      for (var i=0; i<sfEls.length; i++) {
        sfEls[i].onmouseover=function() {
          this.className+=" mysite-hover";
        }
        sfEls[i].onmouseout=function() {
          this.className=this.className.replace(new RegExp(" mysite-hover\\b"), "");
        }
      }
    }
  }
}
if (window.attachEvent) window.attachEvent("onload", mysiteHover);
