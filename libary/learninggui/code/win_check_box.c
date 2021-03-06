/*   
 *  Copyright (C) 2011- 2018 Rao Youkun(960747373@qq.com)
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 */

#include  <stdlib.h>
#include  <string.h>

#include  <lock.h>
#include  <cursor.h>

#include  <d2_rect.h>

#include  <keyboard.h>

#include  <win_tools.h>
#include  <win_callback.h>
#include  <win_invalidate.h>
#include  <win_desktop.h>
#include  <win_interface.h>
#include  <win_widget.h>
#include  <win_check_box.h>

#include  "win_default_in.h"
#include  "win_arithmetic_in.h"


#ifdef  _LG_WINDOW_
#ifdef  _LG_CHECK_BOX_WIDGET_

/* CheckBox internal callback */
static  int  in_check_box_callback(/* GUI_MESSAGE *msg */ void *msg)
{
    HWND           p = NULL;
    HDC            hdc = NULL;
    GUI_RECT       rect;
    GUI_COLOR      old_color;
    unsigned char  state = 0;
    int            key_value  = 0;
 

    if ( msg == NULL )
        return  -1;

    p = GET_TO_HWND_FROM_MSG(msg);
    if ( p == NULL )
        return  -1;

    switch ( MESSAGE_GET_ID(msg) )
    {
        case  MSG_PAINT:
            if ( in_win_is_visual(p) < 1 )
                break;

            in_win_set_current_color_group(p);

            /* paint backgroud */
            in_paint_widget_back(p);

            /* paint border */
            if ( IS_BORDER_WIDGET(&(p->common)) > 0 )
            {
                if ( IS_BORDER_3D_WIDGET(&(p->common)) > 0 )
                    in_paint_3d_down_border(p, &(p->common.win_dc.rect));
                else
                    in_paint_widget_border(p);
            }

            hdc = in_hdc_get_client(p);
            if ( hdc == NULL )
                break;

            #ifdef  _LG_CURSOR_
            in_cursor_maybe_restore_back_abs(hdc->rect.left, hdc->rect.top, hdc->rect.right, hdc->rect.bottom);
            #endif

            rect.left   = 0;
            rect.top    = 0;
            rect.right  = GUI_RECTW(&(hdc->rect))-1;
            rect.bottom = GUI_RECTH(&(hdc->rect))-1;

            old_color = in_hdc_get_back_color(hdc);

            if ( (p == lhdefa)&&(p != lhfocu) ) 
                in_hdc_set_back_color(hdc, DEFAULT_WINDOW_BCOLOR);

            if ( ((IN_GUI_CHECK_BOX *)(p->ext))->state > 0 )
                in_paint_check(hdc, rect.left, rect.top, rect.right, rect.bottom);

            in_hdc_set_back_color(hdc, old_color);
            in_hdc_release_win(p, hdc);

            p->common.invalidate_flag = 0;

            #ifdef  _LG_CURSOR_
            in_cursor_maybe_refresh( );
            #endif
 
            in_win_message_send_ext(p, MSG_PAINT_NEXT, HWND_APP_CALLBACK);
            break;

        case  MSG_MTJT_LBUTTON_UP:
            if ( ((IN_GUI_CHECK_BOX *)(p->ext))->state > 0 )
                state = 0;
            else
                state = 1;

            in_check_box_set_state(p, state);
            break;

        case  MSG_KEY_DOWN:
            key_value = MESSAGE_GET_KEY_VALUE(msg);

            /* GUI_KEY_SPACE */            
            if ( key_value != GUI_KEY_SPACE )
                break;

            if ( ((IN_GUI_CHECK_BOX *)(p->ext))->state > 0 )
                state = 0;
            else
                state = 1;

            in_check_box_set_state(p, state);
            break;

        default:
            break;
    }

    return  in_common_widget_callback(msg);
}

HWND in_check_box_create(HWND parent, void *gui_common_widget, void *gui_check_box)
{
    HWND                p = NULL;
    GUI_CHECK_BOX      *check_box     = (GUI_CHECK_BOX *)gui_check_box;
    IN_GUI_CHECK_BOX    in_check_box;
    unsigned int        size = 0;
    int                 ret;
   

    /* Adjust parent */
    if ( parent == NULL )
        parent = HWND_DESKTOP;
    if ( in_win_has(parent) < 0 )
        return  NULL;


    /* 
     * set hwnd common 
     */

    /* set hwnd common1 */
    /* ID, style, ext_style, default win_dc */
    /* Border ?? */
    /* set ltcomm and ltdc value */
    ret = in_set_hwnd_common1(parent, CHECK_BOX_WIDGET_TYPE, gui_common_widget);
    if ( ret < 1 )
        return  NULL;


    /* set hwnd common win_dc */
    ltdc.color[DISABLED_GROUP][BACK_ROLE] = CKBOX_WIN_DISABLED_BCOLOR;
    ltdc.color[DISABLED_GROUP][FORE_ROLE] = CKBOX_WIN_DISABLED_FCOLOR;

    ltdc.color[INACTIVE_GROUP][BACK_ROLE] = CKBOX_WIN_INACTIVE_BCOLOR;
    ltdc.color[INACTIVE_GROUP][FORE_ROLE] = CKBOX_WIN_INACTIVE_FCOLOR;

    ltdc.color[ACTIVE_GROUP][BACK_ROLE]   = CKBOX_WIN_ACTIVE_BCOLOR;
    ltdc.color[ACTIVE_GROUP][FORE_ROLE]   = CKBOX_WIN_ACTIVE_FCOLOR;
 
    ltcomm.win_dc = ltdc; 


    /* set hwnd common2 */
    /* set client dc */
    in_set_hwnd_common2(gui_common_widget);

    /* set hwnd common client_dc */
    ltdc.color[DISABLED_GROUP][BACK_ROLE]      = CKBOX_CLI_DISABLED_BCOLOR;
    ltdc.color[DISABLED_GROUP][FORE_ROLE]      = CKBOX_CLI_DISABLED_FCOLOR;
    ltdc.color[DISABLED_GROUP][TEXT_BACK_ROLE] = CKBOX_CLI_DISABLED_TBCOLOR;
    ltdc.color[DISABLED_GROUP][TEXT_FORE_ROLE] = CKBOX_CLI_DISABLED_TFCOLOR;

    ltdc.color[INACTIVE_GROUP][BACK_ROLE]      = CKBOX_CLI_INACTIVE_BCOLOR;
    ltdc.color[INACTIVE_GROUP][FORE_ROLE]      = CKBOX_CLI_INACTIVE_FCOLOR;
    ltdc.color[INACTIVE_GROUP][TEXT_BACK_ROLE] = CKBOX_CLI_INACTIVE_TBCOLOR;
    ltdc.color[INACTIVE_GROUP][TEXT_FORE_ROLE] = CKBOX_CLI_INACTIVE_TFCOLOR;

    ltdc.color[ACTIVE_GROUP][BACK_ROLE]        = CKBOX_CLI_ACTIVE_BCOLOR;
    ltdc.color[ACTIVE_GROUP][FORE_ROLE]        = CKBOX_CLI_ACTIVE_FCOLOR;
    ltdc.color[ACTIVE_GROUP][TEXT_BACK_ROLE]   = CKBOX_CLI_ACTIVE_TBCOLOR;
    ltdc.color[ACTIVE_GROUP][TEXT_FORE_ROLE]   = CKBOX_CLI_ACTIVE_TFCOLOR;

    ltcomm.client_dc = ltdc; 


    /* set hwnd common3 */
    /* ltcomm */
    in_set_hwnd_common3(gui_common_widget);
    ltcomm.in_callback = in_check_box_callback;


    /* malloc */
    size = sizeof(GUI_WND_HEAD)+sizeof(GUI_COMMON_WIDGET)+sizeof(IN_GUI_CHECK_BOX);
    p = in_malloc_hwnd_memory(gui_common_widget, size);
    if (p == NULL)
        return  NULL;


    /* Ext */
    memset(&in_check_box, 0, sizeof(in_check_box));
    if ( check_box != NULL )
        in_check_box.state = check_box->state;
    else
        in_check_box.state = 0;

    memcpy(p->ext, &in_check_box, sizeof(IN_GUI_CHECK_BOX));


    /* 
     * add hwnd and post message.
     */
    ret = in_deal_add_hwnd(gui_common_widget, p, 0);
    if ( ret < 1)
        return  NULL;

    return  p;
}

#ifndef  _LG_ALONE_VERSION_
HWND  check_box_create(HWND parent, void *gui_common_widget, void *gui_check_box)
{
    HWND  p = NULL;

    gui_lock( );
    p = in_check_box_create(parent, gui_common_widget, gui_check_box);
    gui_unlock( );

    return  p;
}
#endif

int  in_check_box_get_state(HWND hwnd)
{
    HWND  p     = hwnd;
    int   state = 0;


    if ( p == NULL )
        return  -1;
    if ((p->common.type) != CHECK_BOX_WIDGET_TYPE)
        return  -1;

    state = ((IN_GUI_CHECK_BOX *)(p->ext))->state;            

    return  state;
}

#ifndef  _LG_ALONE_VERSION_
int  check_box_get_state(HWND hwnd)
{
    int  ret = 0;

    gui_lock( );
    ret = in_check_box_get_state(hwnd);
    gui_unlock( );

    return  ret;
}
#endif

int  in_check_box_set_state(HWND hwnd, unsigned char state)
{
    HWND  p  = hwnd;


    if ( p == NULL )
        return  -1;
    if ((p->common.type) != CHECK_BOX_WIDGET_TYPE)
        return  -1;

    ((IN_GUI_CHECK_BOX *)(p->ext))->state = state;
    
    in_win_message_post_ext(p, MSG_PAINT, HWND_IN_CALLBACK | HWND_APP_CALLBACK);

    return  state;
}

#ifndef  _LG_ALONE_VERSION_
int  check_box_set_state(HWND hwnd, unsigned char state)
{
    int  ret = 0;

    gui_lock( );
    ret = in_check_box_set_state(hwnd, state);
    gui_unlock( );

    return  ret;
}
#endif

#endif  /* _LG_CHECK_BOX_WIDGET_ */
#endif  /* _LG_WINDOW_ */
