#include "stm32wrapper.h"
#include "poly.h"
#include <string.h>
#include <stdio.h>
#include "api.h"
#include "randombytes.h"
extern void toom_cook_4way_mem_asm(const uint16_t *a, const uint16_t *b, uint16_t *result);

static unsigned long long overflowcnt = 0;

void sys_tick_handler(void)
{
  ++overflowcnt;
}

static void printcycles(const char *s, unsigned long long c)
{
  char outs[32];
  send_USART_str(s);
  snprintf(outs,sizeof(outs),"%llu\n",c);
  send_USART_str(outs);
}

void random_poly(uint16_t *p, unsigned int len)
{
    randombytes((unsigned char *)p, len * sizeof(uint16_t));
    unsigned int i = 0;
    for (i = 0; i < len; i++) {
        p[i] &= SABER_Q-1;
    }
}
int main (void)
{
    clock_setup(CLOCK_BENCHMARK);
    gpio_setup();
    usart_setup(115200);
    systick_setup();
    rng_enable();

    unsigned int t0, t1;

    send_USART_str("\n===================================");
    
    uint16_t x[SABER_N];
    uint16_t y[SABER_N];
    uint16_t z[SABER_N];

    memset(x, 0, sizeof(x));
    memset(y, 0, sizeof(y));
    memset(z, 0, sizeof(z));

    random_poly(x, SABER_N);
    random_poly(y, SABER_N);

    t0 = systick_get_value();
    overflowcnt = 0;
    toom_cook_4way_mem_asm(x,y,z);
    //polymul_asm(x,y,z);
    t1 = systick_get_value();
    printcycles("cycles: ", (t0+overflowcnt*2400000llu)-t1);
    send_USART_str("###########");
    while(1);

    return 0;
}
