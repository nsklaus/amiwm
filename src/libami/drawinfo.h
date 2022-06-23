#ifndef DRAWINFO_H
#include <X11/Xlib.h>
#include <X11/Xmd.h>

#define DRI_VERSION     (2)

struct DrawInfo
{
    CARD16        dri_Version;    /* will be  DRI_VERSION                 */
    CARD16        dri_NumPens;    /* guaranteed to be >= 9                */
    unsigned long *dri_Pens;      /* pointer to pen array                 */
 
    XFontSet     dri_FontSet;     /* screen default font                  */
    CARD16       dri_Depth;       /* (initial) depth of screen bitmap     */
 
    struct {      /* from DisplayInfo database for initial display mode   */
        CARD16   X;
        CARD16   Y;
    } dri_Resolution;
 
    BITS32      dri_Flags;        /* defined below                        */ 
    Pixmap      dri_CheckMark;    /* pointer to scaled checkmark image    */
    Pixmap      dri_AmigaKey;     /* pointer to scaled Amiga-key image    */
    CARD32      dri_Ascent;
    CARD32      dri_Descent;
    CARD32      dri_MaxBoundsWidth;
    Atom        dri_FontSetAtom;
    CARD32      dri_Reserved;  /* avoid recompilation ;^)           */

}; typedef struct DrawInfo DrawInfo;

#define DETAILPEN        (0x0000)     /* compatible Intuition rendering pens */
#define BLOCKPEN         (0x0001)     /* compatible Intuition rendering pens */
#define TEXTPEN          (0x0002)     /* text on background                  */
#define SHINEPEN         (0x0003)     /* bright edge on 3D objects           */
#define SHADOWPEN        (0x0004)     /* dark edge on 3D objects             */
#define FILLPEN          (0x0005)     /* active-window/selected-gadget fill  */
#define FILLTEXTPEN      (0x0006)     /* text over FILLPEN                   */
#define BACKGROUNDPEN    (0x0007)     /* always color 0                      */
#define HIGHLIGHTTEXTPEN (0x0008)     /* special color text, on background   */

#define BARDETAILPEN     (0x0009)     /* text/detail in screen-bar/menus */
#define BARBLOCKPEN      (0x000A)     /* screen-bar/menus fill */
#define BARTRIMPEN       (0x000B)     /* trim under screen-bar */ 
#define NUMDRIPENS       (0x000C)

#define DRAWINFO_H

extern void term_dri(struct DrawInfo *, Display *, Colormap);
extern void init_dri(struct DrawInfo *, Display *, Window, Colormap, int);

#endif
