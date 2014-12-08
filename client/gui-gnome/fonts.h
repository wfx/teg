/*	$Id: fonts.h,v 1.7 2004/02/08 20:49:05 riq Exp $	*/

// Ok, let me explain
// Pango might be excellent, but it doesn't support fonts with a fixed size in pixels!!!!
// And gnome-canvas 2 uses pango for everything
// so I can't use font sizes I want
// gnome-canvas-text has these properties:
// "size", pixel size,
// but it is not working :-(

#ifndef __TEGG_FONTS_H
#define __TEGG_FONTS_H

/* TEG Fonts */
#ifndef HELVETICA_8_FONT
/* #define HELVETICA_8_FONT  "-adobe-helvetica-medium-r-normal-*-8-*-*-*-p-*-iso8859-1" */
#define HELVETICA_8_FONT  "Helvetica 8"
#endif

#ifndef HELVETICA_10_FONT
#define HELVETICA_10_FONT_OLD  "-adobe-helvetica-medium-r-normal-*-10-*-*-*-p-*-iso8859-1"
#define HELVETICA_10_FONT  "Helvetica 10"
#endif

#ifndef HELVETICA_10_BFONT
/* #define HELVETICA_10_BFONT  "-adobe-helvetica-bold-r-normal-*-10-*-*-*-p-*-iso8859-1" */
#define HELVETICA_10_BFONT  "Helvetica Bold 10"
#endif

#ifndef HELVETICA_12_BFONT
/* #define HELVETICA_12_BFONT "-adobe-helvetica-bold-r-normal-*-12-*-*-*-p-*-iso8859-1" */
#define HELVETICA_12_BFONT  "Helvetica Bold 12"
#endif

#ifndef HELVETICA_12_FONT
/* #define HELVETICA_12_FONT  "-adobe-helvetica-medium-r-normal-*-12-*-*-*-p-*-iso8859-1" */
#define HELVETICA_12_FONT  "Helvetica 12"
#endif

#ifndef HELVETICA_14_BFONT
/* #define HELVETICA_14_BFONT "-adobe-helvetica-bold-r-normal-*-14-*-*-*-p-*-iso8859-1" */
#define HELVETICA_14_BFONT  "Helvetica Bold 14"
#endif

#ifndef HELVETICA_14_FONT
/* #define HELVETICA_14_FONT  "-adobe-helvetica-medium-r-normal-*-14-*-*-*-p-*-iso8859-1" */
#define HELVETICA_14_FONT  "Helvetica 14"
#endif

#ifndef HELVETICA_16_FONT
/* #define HELVETICA_16_FONT  "-adobe-helvetica-medium-r-normal-*-16-*-*-*-p-*-iso8859-1" */
#define HELVETICA_16_FONT  "Helvetica 16"
#endif

#ifndef HELVETICA_18_FONT
/* #define HELVETICA_18_FONT  "-adobe-helvetica-medium-r-normal-*-18-*-*-*-p-*-iso8859-1" */
#define HELVETICA_18_FONT  "Helvetica 18"
#endif

#ifndef HELVETICA_20_BFONT
/* #define HELVETICA_20_BFONT "-adobe-helvetica-bold-r-normal-*-20-*-*-*-p-*-iso8859-1" */
#define HELVETICA_20_BFONT  "Helvetica Bold 20"
#endif

#ifndef HELVETICA_20_FONT
/* #define HELVETICA_20_FONT  "-adobe-helvetica-medium-r-normal-*-20-*-*-*-p-*-iso8859-1" */
#define HELVETICA_20_FONT  "Helvetica 20"
#endif

#endif /* __TEGG_FONTS_H */
