#include <funkyvm/cpu.h>
#include <funkyvm/funkyvm.h>
#include "../funky_system.h"

static fs_display_t *display = NULL;

//   camera([x, y]) -- set camera position
//   clip([x, y, w, h]) -- set screen clipping region
//   cursor(x, y) -- set cursor and CR/LF margin position
//   fget(n, [f]) -- get values of sprite flags
//   flip() -- flip screen back buffer (30fps)
//   fset(n, [f], v) -- set values of sprite flags
//   pal(c0, c1, [p]) -- switch colour 0 to colour 1; p = 0 = draw palette; p = 1 = screen palette
//   palt(col, t) -- set transparency for colour to t (bool)
//   pget(x, y) -- get pixel colour
//   print(str, [x, y, [col]]) -- print string
//   pset(x, y, [col]) -- set pixel colour
//   rectfill(x0, y0, x1, y1, [col]) -- draw filled rectangle
//   sget(x, y) -- get spritesheet pixel colour
//   spr(n, x, y, [w, h], [flip_x], [flip_y]) -- draw sprite
//   sset(x, y, [col]) -- set spritesheet pixel colour
//   sspr(sx, sy, sw, sh, dx, dy, [dw, dh], [flip_x], [flip_y]) -- draw texture from spritesheet


static void rect(CPU_State *state) {
    SDL_Rect rect = {
            STACK_VALUE(state, -3)->int_value,
            STACK_VALUE(state, -2)->int_value,
            STACK_VALUE(state, -1)->int_value,
            STACK_VALUE(state, -0)->int_value
    };
    SDL_RenderDrawRect(display->renderer, &rect);
}

static void fillRect(CPU_State *state) {
    SDL_Rect fillRect = {
            STACK_VALUE(state, -3)->int_value,
            STACK_VALUE(state, -2)->int_value,
            STACK_VALUE(state, -1)->int_value,
            STACK_VALUE(state, -0)->int_value
    };
    SDL_RenderFillRect(display->renderer, &fillRect);
}

static void setColor(CPU_State *state) {
    SDL_SetRenderDrawColor(display->renderer,
                           (Uint8)STACK_VALUE(state, -3)->int_value,
                           (Uint8)STACK_VALUE(state, -2)->int_value,
                           (Uint8)STACK_VALUE(state, -1)->int_value,
                           (Uint8)STACK_VALUE(state, -0)->int_value
    );
}

static void cls(CPU_State *state) {
    SDL_RenderClear(display->renderer);
}

void drawcircle(int x0, int y0, int radius) {
    int x = radius-1;
    int y = 0;
    int dx = 1;
    int dy = 1;
    int err = dx - (radius << 1);

    while (x >= y) {
        SDL_RenderDrawPoint(display->renderer, x0 + x, y0 + y);
        SDL_RenderDrawPoint(display->renderer, x0 + y, y0 + x);
        SDL_RenderDrawPoint(display->renderer, x0 - y, y0 + x);
        SDL_RenderDrawPoint(display->renderer, x0 - x, y0 + y);
        SDL_RenderDrawPoint(display->renderer, x0 - x, y0 - y);
        SDL_RenderDrawPoint(display->renderer, x0 - y, y0 - x);
        SDL_RenderDrawPoint(display->renderer, x0 + y, y0 - x);
        SDL_RenderDrawPoint(display->renderer, x0 + x, y0 - y);

        if (err <= 0) {
            y++;
            err += dy;
            dy += 2;
        }

        if (err > 0) {
            x--;
            dx += 2;
            err += dx - (radius << 1);
        }
    }
}

void fillcircle(int x, int y, int radius) {
    for (int w = 0; w < radius * 2; w++) {
        for (int h = 0; h < radius * 2; h++) {
            int dx = radius - w; // horizontal offset
            int dy = radius - h; // vertical offset
            if ((dx*dx + dy*dy) <= (radius * radius) - 1) {
                SDL_RenderDrawPoint(display->renderer, x + dx, y + dy);
            }
        }
    }
}

static void circle(CPU_State *state) {
    drawcircle(
            STACK_VALUE(state, -2)->int_value,
            STACK_VALUE(state, -1)->int_value,
            STACK_VALUE(state, -0)->int_value
    );
}

static void fillCircle(CPU_State *state) {
    fillcircle(
            STACK_VALUE(state, -2)->int_value,
            STACK_VALUE(state, -1)->int_value,
            STACK_VALUE(state, -0)->int_value
    );
}

#define sgn(x) ((x<0)?-1:((x>0)?1:0)) /* macro to return the sign of a number */

static void line(CPU_State *state) {
    int x1 = STACK_VALUE(state, -3)->int_value - 1;
    int y1 = STACK_VALUE(state, -2)->int_value;
    int x2 = STACK_VALUE(state, -1)->int_value - 1;
    int y2 = STACK_VALUE(state, -0)->int_value;

    int i,dx,dy,sdx,sdy,dxabs,dyabs,x,y,px,py;

    dx=x2-x1;      /* the horizontal distance of the line */
    dy=y2-y1;      /* the vertical distance of the line */
    dxabs=abs(dx);
    dyabs=abs(dy);
    sdx=sgn(dx);
    sdy=sgn(dy);
    x=dyabs>>1;
    y=dxabs>>1;
    px=x1;
    py=y1;

    if (dxabs>=dyabs) /* the line is more horizontal than vertical */
    {
        for(i=0;i<dxabs;i++)
        {
            y+=dyabs;
            if (y>=dxabs)
            {
                y-=dxabs;
                py+=sdy;
            }
            px+=sdx;
            SDL_RenderDrawPoint(display->renderer, px, py);
        }
    }
    else /* the line is more vertical than horizontal */
    {
        for(i=0;i<dyabs;i++)
        {
            x+=dxabs;
            if (x>=dyabs)
            {
                x-=dyabs;
                px+=sdx;
            }
            py+=sdy;
            SDL_RenderDrawPoint(display->renderer, px, py);
        }
    }
}

void register_bindings_draw(CPU_State *state) {
    display = get_display();
    register_syscall(state, "rect", rect);
    register_syscall(state, "fillRect", fillRect);
    register_syscall(state, "setColor", setColor);
    register_syscall(state, "cls", cls);
    register_syscall(state, "circle", circle);
    register_syscall(state, "fillCircle", fillCircle);
    register_syscall(state, "line", line);
}
