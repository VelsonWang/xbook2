#ifndef __GUISRV_ENVIRONMENT_MOUSE_H__
#define __GUISRV_ENVIRONMENT_MOUSE_H__

#include <window/window.h>

/* 鼠标光标状态 */
typedef enum mouse_cursor_state {
    MOUSE_CURSOR_NORMAL = 0,        /* 普通 */
    MOUSE_CURSOR_HOLD,              /* 抓取 */
    MOUSE_CURSOR_HELP,              /* 帮助 */
    MOUSE_CURSOR_BACKGROUND,        /* 后台运行 */
    MOUSE_CURSOR_BUSY,              /* 繁忙 */
    MOUSE_CURSOR_ACCURATE,          /* 精确选择 */
    MOUSE_CURSOR_TEXT,              /* 文本选择 */
    MOUSE_CURSOR_UNUSABLE,          /* 不可用 */
    MOUSE_CURSOR_VRESIZE,           /* 垂直调整大小 */
    MOUSE_CURSOR_HRESIZE,           /* 水平调整大小 */
    MOUSE_CURSOR_DRESIZE1,          /* 对角线调整大小1 */
    MOUSE_CURSOR_DRESIZE2,          /* 对角线调整大小2 */
    MOUSE_CURSOR_MOVE,              /* 移动 */
    MOUSE_CURSOR_LINK,              /* 链接选择 */
} mouse_cursor_state_t;

/* 鼠标光标大小，32*32 */
#define MOUSE_CURSOR_WIDTH   32
#define MOUSE_CURSOR_HEIGHT  32

typedef struct _env_mouse {
    mouse_cursor_state_t state;     /* 鼠标的状态 */
    bool state_changed;             /* 状态是否已经改变了 */
    int x, y;
    int local_x, local_y;           /* 鼠标在窗口中的偏移 */
    gui_window_t *hold_window;      /* 抓住的窗口 */
    layer_t *layer;
    void (*left_btn_down)(void);
    void (*left_btn_up)(void);
    void (*right_btn_down)(void);
    void (*right_btn_up)(void);
    void (*middle_btn_down)(void);
    void (*middle_btn_up)(void);
    
} env_mouse_t;

extern env_mouse_t env_mouse;

#define mouse_enter_state(mouse) (mouse).state_changed = false


void env_mouse_move();

int init_env_mouse();
void env_mouse_draw(mouse_cursor_state_t state);
void env_mouse_set_state(mouse_cursor_state_t state);

#endif  /* __GUISRV_ENVIRONMENT_MOUSE_H__ */
