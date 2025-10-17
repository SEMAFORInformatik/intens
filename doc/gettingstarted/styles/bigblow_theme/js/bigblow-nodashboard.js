// bigblow.js --- BigBlow JS file
//
// Copyright (C) 2011-2016 Fabrice Niessen. All rights reserved.
//
// This file is free software: you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation, either version 3 of
// the License, or (at your option) any later version.
//
// This file is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// Author: Fabrice Niessen <(concat "fniessen" at-sign "pirilampo.org")>
// URL: https://github.com/fniessen/org-html-themes/
// Version: 20140515.1841

$(function() {
    $('p').
        html(function(index, old) {
            return old.replace('FIXME',
                               '<span class="fixme">FIXME</span>');
    });
    $('p').
        html(function(index, old) {
            return old.replace('XXX',
                               '<span class="fixme">XXX</span>');
    });
});

// Remove leading section number
$(function() {
    $('.section-number-2').text("");
    for (var i = 3; i <= 5; i++) {
        $('.section-number-' + i).each(function() {
            $(this).text($(this).text().replace(/^[0-9]+\./g, ""));
        });
    }
});

$(function() {
    $('<div id="minitoc" class="dontprint"></div>').prependTo('body');
});

// generate contents of minitoc
function generateMiniToc(divId) {
    let headers = null;
    if(divId) {
        $('#minitoc').empty().append('<h2>In this section</h2>');
        headers = $('#' + divId).find('h3');
    }
    else {
        $('#minitoc').empty().append('<h2>In this document</h2>');
        headers = $('div#content').find(':header');
    }
    headers.each(function(i) {
            let text = $(this)
                .clone()    //clone the element
                .children() //select all the children
                .remove()   //remove all the children
                .end()  //again go back to selected element
                .text().trim();
            var level = parseInt(this.nodeName.substring(1), 10);
            let prefix = "".padStart(level-1, "  ");
            $("#minitoc").append("<a href='#" + $(this).attr("id") + "'>"
                                 + prefix + text + "</a>");
        });
    // Ensure that the target is expanded (hideShow)
    $('#minitoc a[href^="#"]').click(function() {
        var href = $(this).attr('href');
        hsExpandAnchor(href);
    });
}

// display tabs
function tabifySections() {

    // hide TOC (if present)
    $('#table-of-contents').hide();

    // grab the list of `h2' from the page
    var allSections = [];
    $('h2')
        .each(function() {
            // Remove TODO keywords and tags (contained in spans)
            var tabText = $(this).clone().find('span').remove().end()
                .text().trim();
            var tabId = $(this).parent().attr('id');
            if (tabText) {
                // - remove heading number (all leading digits)
                // - remove progress logging (between square brackets)
                // - remove leading and trailing spaces
                tabText = tabText.replace(/^\d+\s+/, '').replace(/\[[\d/%]+\]/, '').trim();

                allSections.push({
                    text: tabText,
                    id: tabId
                });
            }
        });

    // create the tab links
    var tabs = $('<ul id="tabs"></ul>');
    for (i = 0; i < allSections.length; i++) {
        var item = allSections[i];
        html = $('<li><a href="#' + item.id + '">' + item.text + '</a></li>');
        tabs.append(html);
    }

    // insert tabs menu after title (`h1'), or at the beginning of the content
    if($('.title').length !== 0) {
        $('.title').after(tabs);
    }
    else {
        $('#content').prepend(tabs);
    }
}

function selectTabAndScroll(href) {
    // At this point we assume that href is local (starts with #)
    // alert(href);

    // Find the tab to activate
    var targetTab = $(href).closest('.ui-tabs-panel');
    var targetTabId = targetTab.attr('id');
    var targetTabAriaLabel = targetTab.attr('aria-labelledby');

    var targetTabIndex = $("#content ul li")
        .index($('[aria-labelledby="' + targetTabAriaLabel + '"]'));

    // Activate target tab
    $('#content').tabs('option', 'active', targetTabIndex);

    // Rebuild minitoc
    generateMiniToc(targetTabId);

    // Set the location hash
    // document.location.hash = href;

    // Scroll to top if href was a tab
    if (href == '#' + targetTabId) {
        // alert(targetTabId);
        $.scrollTo(0);
    }
    // Scroll to href if href was not a tab
    else {
        $.scrollTo(href);
    }
}

$(document).ready(function() {
    $('#preamble').remove();
    $('#table-of-contents').remove();

    // Prepare for tabs
    tabifySections();

    // Build the tabs from the #content div
    $('#content').tabs();

    // Set default animation
    $('#content').tabs('option', 'show', true);

    // Rebuild minitoc when a tab is activated
    $('#content').tabs({
        activate: function(event, ui) {
            var divId = ui.newTab.attr('aria-controls');
            generateMiniToc(divId);
        }
    });

    // Required to get the link of the tab in URL
    $('#content ul').localScroll({
        target: '#content',
        duration: 0,
        hash: true
    });

    // Handle hash in URL
    if ($('#content') && document.location.hash) {
        hsExpandAnchor(document.location.hash);
        selectTabAndScroll(document.location.hash);
    }
    // If no hash, build the minitoc anyway for selected tab
    else {
        var divId = $('#content div[aria-expanded=true]').attr('id');
        generateMiniToc(divId);
    }

    // Handle click on internal links
    $('.ui-tabs-panel a[href^="#"]').click(function(e) {
        var href = $(this).attr('href');
        hsExpandAnchor(href);
        selectTabAndScroll(href);
        e.preventDefault();
    });

    // Initialize hideShow
    hsInit();
});

$(document).ready(function() {
    // Add copy to clipboard snippets
    $('.org-src-container').prepend('<div class="snippet-copy-to-clipboard"><span class="copy-to-clipboard-button">[copy]</span></div>');

    // Display/hide snippets on source block mouseenter/mouseleave
    $(document).on('mouseenter', '.org-src-container', function () {
        $(this).find('.snippet-copy-to-clipboard').show();
    });
    $(document).on('mouseleave', '.org-src-container', function () {
        $(this).find('.snippet-copy-to-clipboard').hide();
    });

    $('.copy-to-clipboard-button').click( function() {
        var element = $(this).parent().parent().find('.src');
        var val = element.text();
        val = val.replace(/\n/g, "\r\n");

        var $copyElement = $("<textarea>");
        $("body").append($copyElement);

        $copyElement.val(val);

        $copyElement.trigger('select');
        document.execCommand('copy');

        $copyElement.remove();

        $(this).parent().parent().find('.snippet-copy-to-clipboard').hide();
    });
});

$(function() {
    $('li > code :contains("[X]")')
        .parent()
            .addClass('checked')
        .end()
        .remove();
    $('li > code :contains("[-]")')
        .parent()
            .addClass('halfchecked')
        .end()
        .remove();
    $('li > code :contains("[ ]")')
        .parent()
            .addClass('unchecked')
        .end()
        .remove();
});

$(function() {
    $('i :contains("[#A]")')
        .replaceWith('<i><span style="color: #F67777;">[#A]</span></i>');
    $('i :contains("[#B]")')
        .replaceWith('<i><span style="color: #B6E864;">[#B]</span></i>');
    $('i :contains("[#C]")')
        .replaceWith('<i><span style="color: #C3DCFF;">[#C]</span></i>');
});

$(function() {
    $('<div id="toTop" class="dontprint"><span>^ Back to Top</span></div>').appendTo('body');

    $(window).scroll(function() {
        if ($(this).scrollTop() != 0) {
            $('#toTop').fadeIn();
        } else {
            $('#toTop').fadeOut();
        }
    });

    $('#toTop').click(function(e) {
        $('html, body').animate({scrollTop: 0}, 800);
        e.preventDefault();                   // Disable default browser behavior
    });
});

function togglePanel(e) {
    e.preventDefault();

    $("#left-panel-contents").toggleClass('active').toggle(200);
    $("#right-panel-contents").toggleClass('active').toggle(200);

    var slidePos =
        $("#left-panel-button").css("left") == "-23px"? '182px': '-23px';

    $("#left-panel-button").
        animate({"left": slidePos, "opacity": 0.9}, {duration: "200" });

    // if ($("#left-panel-contents").hasClass('active')) {
    //     hsHideTodoKeyword('done');
    // } else {
    //     hsShowTodoKeyword('done');
    // }

    // return false;
}

$(function() {
    $('<div id="left-panel-wrapper" class="dontprint"><div id="left-panel-contents" style="opacity: 0.9"></div></div>')
        .appendTo('body');

    $('<div id="right-panel-wrapper" class="dontprint"><div id="right-panel-contents" style="opacity: 0.9"></div></div>')
        .appendTo('body');

    $('#left-panel-button').click(togglePanel);
});


$(function() {
    $('.tag span').click(function(e) {
        var orgTag = $(this).text().trim();
        $('.' + orgTag).toggleClass('selected');
        $('#content .' + orgTag).parent().parent().parent()
            .toggleClass('matchtag');
        $('#right-panel-wrapper .' + orgTag).parent().parent()
            .toggleClass('matchtag');
        e.preventDefault();
    })
});

function clickPreviousTab() {
    var active = $('#content').tabs('option', 'active');
    // Avoid going back to last tab
    if (active == 0) return;

    $('#content').tabs('option', 'active', active - 1);

    // Set the location href
    var href = $('#content div[aria-expanded=true]').attr('id');
    document.location.hash = href;
    $.scrollTo(0);
}

function clickNextTab() {
    var active = $('#content').tabs('option', 'active');
    $('#content').tabs('option', 'active', active + 1);

    // Set the location href
    var href = $('#content div[aria-expanded=true]').attr('id');
    document.location.hash = href;
    $.scrollTo(0);
}

function orgDefkey(e) {
    if (!e)
        var e = window.event;
    var keycode = (e.keyCode) ? e.keyCode : e.which;
    var actualkey = String.fromCharCode(keycode);
    switch (actualkey) {
        case "?": // help (dashboard)
        case "h":
            togglePanel(e);
            break;
        case "n": // next
            clickNextTab();
            break;
        case "p": // previous
            clickPreviousTab();
            break;
        // case "b": // scroll down - should be mapped to Shift-SPC
        //     $(window).scrollTop($(window).scrollTop()-$(window).height());
        //     break;
        case "<": // scroll to top
            $(window).scrollTop(0);
            break;
        case ">": // scroll to bottom
            $(window).scrollTop($(document).height());
            break;
        case "-": // collapse all
            hsCollapseAll();
            break;
        case "+": // expand all
            hsExpandAll();
            break;
        case "r": // go to next task
            hsReviewTaskNext();
            break;
        case "R": // go to previous task
            hsReviewTaskPrev();
            break;
        case "q": // quit reviewing
            hsReviewTaskQuit();
            break;
        case "g": // refresh the page (from the server, rather than the cache)
            location.reload(true);
            break;
    }
}

document.onkeypress = orgDefkey;
