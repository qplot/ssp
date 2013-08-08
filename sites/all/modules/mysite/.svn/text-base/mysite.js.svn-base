// $Id: mysite.js,v 1.14 2008/04/01 00:55:56 agentken Exp $

/**
 * @file
 * JavaScript functions for MySite.  Includes collapsible <div> tags and
 * Interface-compatible functions for drag-and-drop sorting.
 */

/**
 * Because <fieldset> is only valid HTML for <form> elements
 * we rewrite Drupal's collpase.js functions to work with special <div>
 * elements in MySite.
 */

/**
 * Toggle the visibility of a div using smooth animations
 */
Drupal.toggleDiv = function(div) {
  if ($(div).is('.collapsed')) {
    var content = $('> div', div).hide();
    $(div).removeClass('collapsed');
    content.slideDown( {
    duration: 300, // THE FIX
      complete: function() {
        // Make sure we open to height auto
        $(this).css('height', 'auto');
        Drupal.collapseScrollIntoView(this.parentNode);
        this.parentNode.animating = false;
      },
      step: function() {
         // Scroll the div into view
        Drupal.collapseScrollIntoView(this.parentNode);
      }
    });
    if (typeof Drupal.textareaAttach != 'undefined') {
      // Initialize resizable textareas that are now revealed
      Drupal.textareaAttach(null, div);
    }
  }
  else {
    var content = $('> div', div).slideUp('medium', function() {
      $(this.parentNode).addClass('collapsed');
      this.parentNode.animating = false;
    });
  }
}

/**
 * Scroll a given div into view as much as possible.
 */
Drupal.collapseScrollIntoView = function (node) {
  var h = self.innerHeight || document.documentElement.clientHeight || $('body')[0].clientHeight || 0;
  var offset = self.pageYOffset || document.documentElement.scrollTop || $('body')[0].scrollTop || 0;
  var pos = Drupal.absolutePosition(node);
  var fudge = 55;
  if (pos.y + node.offsetHeight + fudge > h + offset) {
    if (node.offsetHeight > h) {
      window.scrollTo(0, pos.y);
    } else {
      window.scrollTo(0, pos.y + node.offsetHeight - h + fudge);
    }
  }
}

// Global Killswitch
if (Drupal.jsEnabled) {
  $(document).ready(function() {
    $('div.collapsible > span.mysite-header').each(function() {
      var div = $(this.parentNode);
      // Expand if there are errors inside
      if ($('input.error, textarea.error, select.error', div).size() > 0) {
        div.removeClass('collapsed');
      }

      // Turn the span into a clickable link and wrap the contents of the div
      // in a div for easier animation
      var text = this.innerHTML;
      $(this).empty().append($('<a href="#" title="Click to expand/collapse. Click and drag to sort.">'+ text +'</a>').click(function() {
        var div = $(this).parents('div:first')[0];
        // Don't animate multiple times
        if (!div.animating) {
          div.animating = true;
          Drupal.toggleDiv(div);
        }
        return false;
      })).after($('<div class="mysite-wrapper"></div>').append(div.children(':not(span.mysite-header)')));
    });
  });
}

/**
 * jQuery Interface functions for MySite
 */
$(document).ready(
  function () {
    $('div.mysite-sortable').Sortable(
      {
        handle: 'span.mysite-header',
        accept : 'sortable-item',
        onchange : function (obj) {
                     serial = $.SortSerialize(obj.id);
                     mysite_ajax_call(serial.hash);
                   },
        fx: 200,
        axis: 'float',
        tolerance: 'intersect',
        activeclass: 'mysite-active',
        hoverclass: 'mysite-hover',
        helperclass: 'mysite-helper',
        ghosting: true,
        opacity: 0.75
      }
    )
  }
);

/**
 * The ajax callback function to save changes
 */
function mysite_ajax_call(serial) {
  var serial = serial.replace(/&/g, ':');
  var serial = serial.replace(/=m/g, '=');
  var data = serial.replace(/mysite-sort/g, '');
  var url = document.URL;
  var request = url.replace(/mysite.+/, 'ajax/mysite-sort/') + data;
  $.get(request);
  mysite_message();
}

/**
 * Trigger a display message to the user.
 * Translators: edit the $string variable.
 */
function mysite_message() {
  $string = 'Changes saved.';
  // clear existing messages
  $("div.messages").hide();
  $("div.mysite-ajax").empty();
  // insert and remove the message
  $("div.mysite-ajax").fadeIn(100, function(){
    $("div.mysite-ajax").append('<div class="messages">' + $string + '</div>');
  });
  $("div.mysite-ajax").fadeOut(5000, function(){
    $("div.mysite-ajax").empty();
  });
}
