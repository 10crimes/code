// ==UserScript==
// @name           StackExchange Tweaks
// @namespace      SET
// @description    Minor visual tweaks to StackExchange (remove the new sidebar from Q/A pages, for classic look)
// @version        1.0.6
// @license        MIT
// @include        https://stackoverflow.com/*
// @include        https://superuser.com/*
// @include        https://serverfault.com/*
// @include        https://*.stackexchange.com/*
// @grant          GM_addStyle
// ==/UserScript==

// Swap the positions of the notifications block and the username/stats block
//var secondaryNav = document.querySelector('.secondary-nav')
//secondaryNav.parentNode.insertBefore(secondaryNav, secondaryNav.parentNode.firstChild)
var profileElementInner = document.querySelector('.my-profile');
if (profileElementInner) {
    var profileElementContainer = profileElementInner.parentNode;
    profileElementContainer.parentNode.appendChild(profileElementContainer);
} else {
    console.warn("Could not find .my-profile element");
}

// Hide the sidebar when we are on question pages (to reduce visual noise)
if (document.location.pathname.match(/^\/(q|questions)\//)) {
    GM_addStyle('#left-sidebar { display: none; }   #content { border-left: none; }');
}
