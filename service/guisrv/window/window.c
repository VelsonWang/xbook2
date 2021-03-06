#include <window/window.h>
#include <window/draw.h>
#include <layer/draw.h>
#include <drivers/screen.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <guisrv.h>

#include <environment/desktop.h>
#include <environment/mouse.h>

/* 当前活动的窗口 */
gui_window_t *window_current;

gui_window_t *gui_create_window(
    char *title,
    int x,
    int y,
    int width,
    int height,
    int attr,
    gui_window_t *parent
) {
    if (width <= 0 || height <= 0)
        return NULL;
    int w, h;   /* window width, height */
    int y_off = 0;
    /* 自动添加关闭按钮 */
    attr |= GUIW_BTN_CLOSE;

    /* 根据属性设置窗口内容 */
    if (attr & GUIW_NO_TITLE) { /* 没有标题 */
        attr &= ~GUIW_BTN_MASK; /* 清除按钮控制 */
        
        /* 窗口=窗体 */
        w = width;
        h = height;
        
    } else {    
        /* 窗口=窗体+标题 */
        w = width;
        h = height + GUIW_TITLE_HEIGHT;
        y_off = GUIW_TITLE_HEIGHT;
    }

    /* alloc a new layer first */
    layer_t *layer = create_layer(w, h);
    if (layer == NULL) {
        return NULL;
    }
    
    /* 创建一个窗口 */
    gui_window_t *win = sbrk(0);
    if (win == (void *) -1) {
        destroy_layer(layer);
        return NULL;
    }

    if (sbrk(sizeof(gui_window_t)) == (void *) -1) {
        destroy_layer(layer);
        return NULL;
    }

    /* 设置窗口结构 */
    win->x = x;
    win->y = y;
    win->x_off = 0;
    win->y_off = y_off;
    win->width = width;
    win->height = height;
    win->attr = attr;
    win->layer = layer;
    win->parent = parent;
    win->title_color = COLOR_RED;
    win->title_bar_color = COLOR_RGB(128, 128, 128);
    
    layer->extension = (void *) win;

    memset(win->title, 0, GUIW_TITLE_LEN);
    if (title) {    /* 有标题才复制 */    
        strcpy(win->title, title);
    }
    init_list(&win->list);
    init_list(&win->child_list);
    
    /* 有父窗口就追加到父窗口的子窗口链表中 */
    if (parent) {
        list_add_tail(&win->list, &parent->child_list);
    }

    /* 绘制窗口本体 */
    layer_draw_rect_fill(win->layer, win->x_off, win->y_off, win->layer->width, win->height, COLOR_WHITE);

    layer_set_xy(layer, x, y);
    
    layer_set_z(layer, layer_topest->z);    /* 位于顶层图层下面 */

    gui_window_focus(win);

    return win;
}

int gui_destroy_window(gui_window_t *win)
{
    if (!win)
        return -1;
    layer_t *layer = win->layer;
    /* 隐藏图层 */
    layer_set_z(layer, -1);
    /* 关闭所有子窗口 */
    gui_window_t *child, *next;
    list_for_each_owner_safe (child, next, &win->child_list, list) {
        gui_destroy_window(child);  /* 递归销毁子窗口 */
    }

    /* 脱离父窗口 */
    if (win->parent) {
        list_del_init(&win->list);
        win->parent = NULL;
    }

    /* 释放窗口占用的空间 */

    /* 释放图层 */
    if (destroy_layer(layer))
        return -1;
    
    /* 获取顶层窗口 */
    

    return 0;
}

int gui_window_update(gui_window_t *win, int left, int top, int right, int buttom)
{
    layer_refresh(win->layer, win->x_off + left, win->y_off + top, 
        win->x_off + right, win->y_off + buttom);
    return 0;
}

void gui_window_title_switch(gui_window_t *window, bool on)
{
    if (on) {
        window->title_bar_color = COLOR_RGB(192, 192, 192);
        window->title_color = COLOR_RGB(255, 255, 255);
    } else {
        window->title_bar_color = COLOR_RGB(128, 128, 128);
        window->title_color = COLOR_RGB(200, 200, 200);
    }
    /* 绘制标题栏背景 */
    layer_draw_rect_fill(window->layer, 0, 0, window->layer->width, GUIW_TITLE_HEIGHT,
        window->title_bar_color);
    layer_draw_text(window->layer, 24, 4, window->title, window->title_color);
    layer_refresh(window->layer, 0, 0, window->layer->width, GUIW_TITLE_HEIGHT);
}

/**
 * gui_window_switch - 切换图形窗口
 * @window: 窗口
 * 
 */
void gui_window_focus(gui_window_t *window)
{
    if (window == current_window) {
        return;
    }
    if (window) {   /* 切换到指定窗口 */
        /* 修改聚焦窗口 */
        
        /* 移除聚焦 */
        if (current_window && !(current_window->attr & GUIW_NO_TITLE)) {  /* 有标题，就修改标题栏颜色 */
            /* 修改标题栏颜色 */
            gui_window_title_switch(current_window, false);
        }

        /* 绑定聚焦 */
        if (!(window->attr & GUIW_NO_TITLE)) {  /* 有标题，就修改标题栏颜色 */
            /* 修改标题栏颜色 */
            gui_window_title_switch(window, true);
        }
        current_window = window;
    }
}

/**
 * gui_window_switch - 切换图形窗口
 * @window: 窗口
 * 
 */
void gui_window_switch(gui_window_t *window)
{
    if (window == current_window) {
        return;
    }
    if (window) {   /* 切换到指定窗口 */
        /* 修改窗口的高度为最高 */
        if (window->layer) {
            /* 切换到鼠标下面 */
            layer_set_z(window->layer, layer_topest->z - 1);
            
            gui_window_focus(window);
        }
    }
}

int init_gui_window()
{
    window_current = NULL;
    
    if (init_env_desktop()) {
        printf("[desktop ] %s: init desktop environment failed!\n", SRV_NAME);
        return -1;
    }
    
    gui_window_draw_text(env_desktop.window, 0,0,"abcdefghijklmnopqrstuvwxyz0123456789",
        COLOR_RED);

    gui_window_draw_text_ex(env_desktop.window, 0,16,"abcdefghijklmnopqrstuvwxyz0123456789",
        COLOR_RED, gui_get_font("simsun"));
    
    gui_window_update(env_desktop.window, 0,0, 400, 16*2);

    /* 创建测试窗口 */
    gui_window_t *win = gui_create_window("xbook2", 20, 20, 320, 240, 0, NULL);
    if (win == NULL) {
        printf("create window failed!\n");
        return -1;
    }
    
    gui_window_put_point(win, 10, 10, COLOR_BLUE);
    gui_window_put_point(win, 15, 13, COLOR_RED);
    gui_window_put_point(win, 13, 15, COLOR_GREEN);

    gui_window_update(win, 10, 10, 15, 15);

    gui_window_draw_line(win, 20, 20, 100, 150, COLOR_YELLOW);
    
    gui_window_draw_rect(win, 20, 20, 100, 150, COLOR_BLUE);
    gui_window_draw_rect_fill(win, 100, 20, 100, 150, COLOR_GREEN);
    
    gui_window_update(win, 20, 20, 300, 200);

    win = gui_create_window("xbook3", 500, 100, 240, 360, 0, NULL);
    if (win == NULL) {
        printf("create window failed!\n");
        return -1;
    }
    
    gui_window_put_point(win, 10, 10, COLOR_BLUE);
    gui_window_put_point(win, 15, 13, COLOR_RED);
    gui_window_put_point(win, 13, 15, COLOR_GREEN);

    gui_window_update(win, 10, 10, 15, 15);

    gui_window_draw_line(win, 20, 20, 100, 150, COLOR_YELLOW);
    
    gui_window_draw_rect(win, 20, 20, 100, 150, COLOR_BLUE);
    gui_window_draw_rect_fill(win, 100, 20, 100, 150, COLOR_GREEN);
    
    gui_window_draw_rect_fill(win, 100, 20, 100, 150, COLOR_GREEN);
    
    gui_window_draw_text(win, 100, 200, "hello, world!", COLOR_RED);

    gui_window_draw_text_ex(win, 100, 220, "hello, world!", COLOR_RED, gui_get_font("Simsun"));
    
    gui_window_update(win, 20, 20, 300, 300);
    
    //sleep(3);   /* 休眠1s */

    //gui_destroy_window(win);
    return 0;
}
