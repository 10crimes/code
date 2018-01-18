// ==UserScript==
// @name          theguardian
// @namespace     http://userstyles.org
// @description	  Make the guardian great again!
// @author        coxmichael
// @homepage      https://userstyles.org/styles/154282
// @include       http://www.theguardian.com/*
// @include       https://www.theguardian.com/*
// @include       http://*.www.theguardian.com/*
// @include       https://*.www.theguardian.com/*
// @do-not-run-at document-start
// @version       0.20180116142410-joey00
// ==/UserScript==
(function() {var css = [
	".new-header {",
	"    background: #005689;",
	"}",
	"",
	".pillar-link.pillar-link,",
	".top-bar__item {",
	"    color:#fff;",
	"}",
	"",
	".pillar-link.pillar-link:after {",
	"    border-color:#fff;",
	"}",
	"",
	".top-bar__item--cta--circle:before {",
	"    background:#4bc6df;",
	"}",
	"",
	".new-header--news .top-bar__item--cta:hover .top-bar__item--cta--circle:before, ",
	".new-header--news .top-bar__item--cta:focus .top-bar__item--cta--circle:before {",
	"    background:#4bc6df;",
	"}",
	"",
	".top-bar__item--cta--text,",
	".rich-link:not(.rich-link--paidfor) .inline-arrow-in-circle-garnett__svg, ",
	".rich-link:not(.rich-link--paidfor).rich-link--pillar-news .inline-arrow-in-circle-garnett__svg,",
	".rich-link:not(.rich-link--paidfor) .rich-link__byline, ",
	".rich-link:not(.rich-link--paidfor) .rich-link__kicker, ",
	".rich-link:not(.rich-link--paidfor) .rich-link__read-more-text, ",
	".rich-link:not(.rich-link--paidfor).rich-link--pillar-news .rich-link__byline, ",
	".rich-link:not(.rich-link--paidfor).rich-link--pillar-news .rich-link__kicker, ",
	".rich-link:not(.rich-link--paidfor).rich-link--pillar-news .rich-link__read-more-text {",
	"    color:#114b6d;",
	"}",
	"",
	".inline-the-guardian-logo__svg path {",
	"    fill:#fff;",
	"}",
	"",
	".veggie-burger {",
	"    background:#4bc6df;",
	"}",
	"",
	"/*  text / headlines */",
	"",
	".fc-today .fc-today__sub,",
	".fc-item--pillar-news .fc-item__kicker,",
	".fc-container__header__title,",
	".fc-sublink--pillar-news .fc-sublink__kicker,",
	".content--pillar-news .byline, .content--pillar-news .content--media .content__headline, .content--pillar-news .old-article-message, .content--pillar-news .pullquote-cite, .content--pillar-news .content__section-label__link, .content--pillar-news a, .paid-content .byline, .paid-content .content--media .content__headline, .paid-content .old-article-message, .paid-content .pullquote-cite, .paid-content .content__section-label__link, .paid-content a,",
	".content--pillar-news .button--secondary, .content--pillar-news .drop-cap, .content--pillar-news .element-pullquote p, .content--pillar-news.content--type-feature .content__headline, .content--pillar-news.content--type-review .content__headline, .content--pillar-news .u-underline, .paid-content .button--secondary, .paid-content .drop-cap, .paid-content .element-pullquote p, .paid-content.content--type-feature .content__headline, .paid-content.content--type-review .content__headline, .paid-content .u-underline {",
	"    color:#005689;",
	"}",
	"",
	".fc-item--pillar-news.fc-item--type-feature .fc-item__headline {",
	"    color:#7b7b7b;",
	"}",
	"",
	".fc-container__header__title {",
	"    padding-top:1em;",
	"    padding-bottom:1em;",
	"}",
	"",
	".fc-item--pillar-news .fc-item__container:before,",
	".rich-link:not(.rich-link--paidfor) .rich-link__avatar__media, .rich-link:not(.rich-link--paidfor) .rich-link__container::before, .rich-link:not(.rich-link--paidfor).rich-link--pillar-news .rich-link__avatar__media, .rich-link:not(.rich-link--paidfor).rich-link--pillar-news .rich-link__container::before {",
	"    background-color:#005689;",
	"}",
	"",
	".fc-item--pillar-news .vjs-big-play-button .vjs-control-text:before,",
	".fc-item--pillar-news .youtube-media-atom__play-button {",
	"    background-color:#005689;",
	"}",
	"",
	".new-header--news .subnav__item--parent:after {",
	"    border-left-color: #005689;",
	"}",
	"",
	"/* rule */",
	"",
	".new-header:not(.new-header--slim):after {",
	"    height: 1px;",
	"}",
	"",
	".content--liveblog .content__meta-container, .content--media .content__meta-container, .content--type-analysis .content__meta-container, .content--type-article .content__meta-container, .content--type-comment .content__meta-container, .content--type-feature .content__meta-container, .content--type-guardianview .content__meta-container, .content--type-immersive .content__meta-container, .content--type-interview .content__meta-container, .content--type-live .content__meta-container, .content--type-matchreport .content__meta-container, .content--type-media .content__meta-container, .content--type-review .content__meta-container, .content--type-quiz .content__meta-container,",
	".fc-item--type-comment .fc-item__container>.fc-item__meta {",
	"    background-image:none;",
	"}",
	"",
	"/*  opinion */",
	"",
	".fc-item--pillar-arts .fc-item__kicker {",
	"    color: #FF9800;",
	"}",
	"",
	".fc-item--pillar-arts .fc-item__container:before {",
	"    background-color:#FF9800;",
	"}",
	"",
	"/* most viewed */",
	".headline-list__link .inline-numbers,",
	".rich-link:not(.rich-link--paidfor) .inline-arrow-in-circle-garnett__svg, .rich-link:not(.rich-link--paidfor).rich-link--pillar-news .inline-arrow-in-circle-garnett__svg {",
	"    fill:#005689;",
	"}",
	"",
	"/* footer */",
	".footer__back-to-top-container {",
	"    background: #005689;",
	"    ",
	"}",
	"",
	"",
	".back-to-top__text {",
	"    color:#fff;",
	"}",
	"",
	".back-to-top__icon-container {",
	"    background-color:#4bc6df;",
	"}",
	"",
	".content--pillar-news .block-share__item .inline-icon, .content--pillar-news .inline-close svg, .content--pillar-news .commentcount2__heading span, .content--pillar-news .old-article-message .old-article-message--clock svg, .content--pillar-news .social-icon svg, .content--pillar-news .social-icon__svg, .paid-content .block-share__item .inline-icon, .paid-content .inline-close svg, .paid-content .commentcount2__heading span, .paid-content .old-article-message .old-article-message--clock svg, .paid-content .social-icon svg, .paid-content .social-icon__svg {",
	"    fill: #797979;",
	"}",
	"",
	".content--liveblog .submeta, .content--media .submeta, .content--type-analysis .submeta, .content--type-article .submeta, .content--type-comment .submeta, .content--type-feature .submeta, .content--type-guardianview .submeta, .content--type-immersive .submeta, .content--type-interview .submeta, .content--type-live .submeta, .content--type-matchreport .submeta, .content--type-media .submeta, .content--type-review .submeta, .content--type-quiz .submeta {",
	"    background:none;",
	"}",
	"",
	".content--pillar-news .block-share__item:focus, .content--pillar-news .block-share__item:hover, .content--pillar-news .social-icon:focus, .content--pillar-news .social-icon:hover, .content--pillar-news .social-icon.social-icon--more:focus, .content--pillar-news .social-icon.social-icon--more:hover, .paid-content .block-share__item:focus, .paid-content .block-share__item:hover, .paid-content .social-icon:focus, .paid-content .social-icon:hover, .paid-content .social-icon.social-icon--more:focus, .paid-content .social-icon.social-icon--more:hover {",
	"    background-color: #005689;",
	"    border: 0.0625rem solid #005689;",
	"}",
	"",
	".content--pillar-news .u-underline:hover, .paid-content .u-underline:hover {",
	"    border-bottom: solid 0.0625rem #005689;",
	"}",
	"",
	".new-header.new-header--open, .menu {",
	"    background-color: #005689;",
	"}",
	"",
	"",
	".menu-item__title {",
	"    color:#fff !important;",
	"}"
].join("\n");
if (typeof GM_addStyle != "undefined") {
	GM_addStyle(css);
} else if (typeof PRO_addStyle != "undefined") {
	PRO_addStyle(css);
} else if (typeof addStyle != "undefined") {
	addStyle(css);
} else {
	var node = document.createElement("style");
	node.type = "text/css";
	node.appendChild(document.createTextNode(css));
	var heads = document.getElementsByTagName("head");
	if (heads.length > 0) {
		heads[0].appendChild(node);
	} else {
		// no head yet, stick it whereever
		document.documentElement.appendChild(node);
	}
}
})();
