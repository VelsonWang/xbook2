#ifndef __GUISRV_WINDOW_H__
#define __GUISRV_WINDOW_H__

#include <layer/layer.h>

#include <sys/list.h>

#define GUIW_TITLE_LEN    128   /* 标题字符串长度 */

#define GUIW_NO_TITLE       0x01    /* 没有标题 */
#define GUIW_BTN_MINIM      0x02    /* 最小化按钮 */
#define GUIW_BTN_MAXIM      0x04    /* 最大化按钮 */
#define GUIW_BTN_CLOSE      0x08    /* 关闭按钮 */

#define GUIW_BTN_MASK       (GUIW_BTN_MINIM | GUIW_BTN_MAXIM | GUIW_BTN_CLOSE)

#define GUIW_TITLE_HEIGHT    24     /* 窗口标题高度 */



typedef struct _gui_window {
    int x, y;                   /* 窗口的位置 */
    int width, height;          /* 窗口的宽高 */
    int x_off, y_off;           /* 窗口数据的偏移 */
    int attr;                   /* 窗口的属性 */
    char title[GUIW_TITLE_LEN]; /* 窗口标题 */
    layer_t *layer;             /* 窗口对应的图层 */
    list_t list;                /* 窗口链表 */
    list_t child_list;          /* 子窗口链表 */
    struct _gui_window *parent; /* 父窗口指针 */
    GUI_COLOR title_bar_color;  /* 标题栏颜色 */
    GUI_COLOR title_color;      /* 标题颜色 */
    
} gui_window_t;

extern gui_window_t *window_current;

#define current_window window_current

int init_gui_window();

gui_window_t *gui_create_window(
    char *title,
    int x,
    int y,
    int width,
    int height,
    int attr,
    gui_window_t *parent
);
int gui_destroy_window(gui_window_t *win);

int gui_window_update(
    gui_window_t *win,
    int left,
    int top,
    int right,
    int buttom
);

void gui_window_switch(gui_window_t *window);
void gui_window_focus(gui_window_t *window);

#endif  /* __GUISRV_WINDOW_H__ */
