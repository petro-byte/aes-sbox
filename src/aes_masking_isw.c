#include "sw/device/examples/demos.h"
#include "sw/device/lib/arch/device.h"
#include "sw/device/lib/dif/dif_uart.h"
#include "sw/device/lib/pinmux.h"
#include "sw/device/lib/runtime/hart.h"
#include "sw/device/lib/runtime/log.h"
#include "sw/device/lib/runtime/print.h"
#include "sw/device/lib/testing/check.h"
#include "sw/device/lib/testing/test_status.h"
#include "sw/device/lib/runtime/ibex.h"
#include "hw/top_earlgrey/sw/autogen/top_earlgrey.h"  // Generated.

#include "aes_masking_isw.h"
#include "aes_const.h"

// Global variables
static dif_uart_t uart;              // UART object


// Main program
int main(int argc, char **argv)
{
    // Setup UART
    CHECK(dif_uart_init(
            (dif_uart_params_t){
                .base_addr = mmio_region_from_addr(TOP_EARLGREY_UART_BASE_ADDR),
            },
            &uart) == kDifUartOk);
    CHECK(dif_uart_configure(&uart, (dif_uart_config_t){
                                      .baudrate = kUartBaudrate,
                                      .clk_freq_hz = kClockFreqPeripheralHz,
                                      .parity_enable = kDifUartToggleDisabled,
                                      .parity = kDifUartParityEven,
                                  }) == kDifUartConfigOk);
    base_uart_stdout(&uart);

    
    pinmux_init();

    validate_sec_mult();

    // AES S-Box
    uint8_t aes_sbox[256];

    // shares and masking order d
    uint8_t d = MASKING_ORDER;
    uint8_t shares[d+1];
    uint8_t ret = 0;
    
    // Debug message
    LOG_INFO("Computing ISW-masked AES S-box for d = %d ...\n", d);

    // Compute S-Box
    for(uint16_t x = 0; x <= 255; x++)
    {
      // Create d+1 shares
      share(x, shares, d);
      
      // Compute S-box using ISW masking
      sec_sbox(shares, d);
      
      // Reconstruct output
      aes_sbox[x] = unshare(shares, d);
      
      LOG_INFO("0x%2X ", aes_sbox[x]);

      if (aes_sbox[x] != aes_sbox_orig[x]) {
        LOG_INFO(" FAILED for input %d!\n", x);
        ret = 1;
        break;
      }
                                          
    }

    if (ret==0)
      LOG_INFO("\nSUCCESS!\n");

    return ret;

    return 0;

}

void reset_share(uint8_t *shares, uint8_t d)
{
    for(uint8_t i = 0; i < d+1; i++)
    {
        shares[i] = 0;
    }
}

void share(uint8_t x, uint8_t *shares, uint8_t d)
{
    for(uint8_t i = 0; i < d+1; i++)
    {
        shares[i] = 0;
    }
    shares[0] = x;
}

void refresh_masks(uint8_t *shares, uint8_t d, struct rng_state *state) {
    uint8_t t;
    for(uint8_t i = 1; i <= d; i++) {
        t = mersenne_twister(state);
        shares[0] = shares[0] ^ t;
        shares[i] = shares[i] ^ t;
    }
}

uint8_t unshare(uint8_t *shares, uint8_t d)
{
    // Reconstruct output
    uint8_t res = 0;
    for (uint8_t i = 0; i <= d; i++)
    {
        res = res ^ shares[i];
    }
    return res;
}

void sec_sbox(uint8_t *shares, uint8_t d)
{
    struct rng_state state;
    state.val = 42;

    // Compute inverse
    // uint8_t y[d+1];
    sec_inv(shares, shares, d, &state);

    // Compute secure AT
    for(uint8_t i = 0; i <= d; i++) {
        shares[i] = affine_transform(shares[i]);
        shares[0] ^= 0x63;
    }
    shares[0] ^= 0x63;
}

uint8_t affine_transform(uint8_t x) {
    return x ^ left_rot(x, 1) ^ left_rot(x, 2) ^ left_rot(x, 3) ^ left_rot(x, 4) ^ 0x63;
}

uint8_t left_rot(uint8_t x, uint8_t n) {
    return (x << n) | (x >> (8 - n));
}

void validate_sec_mult()
{
    struct rng_state state;
    state.val = 42;
    uint16_t i;
    uint8_t res;
    uint8_t d = MASKING_ORDER;
    uint8_t shares[d+1], res_shares[d+1];
    uint32_t t1, t2;
    uint32_t timings[256];
    uint32_t avg, min, max, var;
    for(i = 0; i < 256; i++) {
        share(i, shares, d);
        share(0, res_shares, d);
        t1 = (uint32_t) ibex_mcycle_read();
        sec_inv(shares, res_shares, d, &state);
        t2 = (uint32_t) ibex_mcycle_read();
        res = unshare(res_shares, d);
        LOG_INFO("%i", gf_mult(res, i));
        timings[i] = t2 - t1;
    }

    min = timings[0];
    for(i = 1; i < 256; i++) {
        if(timings[i] < min) {
            min = timings[i];
        }
    }

    max = timings[0];
    for(i = 1; i < 256; i++) {
        if(timings[i] > max) {
            max = timings[i];
        }
    }

    avg = 0;
    for(i = 0; i < 256; i++) {
        avg += (timings[i] / 256);
    }

    var = 0;
    for(i = 0; i < 256; i++) {
        var += ( ( (timings[i] - avg) * (timings[i] - avg) ) / 256 );
    }

    LOG_INFO("Minimum Cycles: %u", min);
    LOG_INFO("Maximum Cycles: %u", max);
    LOG_INFO("Average Cycles: %u", avg);
    LOG_INFO("Variance: %u", var);

}

void sec_mult(uint8_t *a, uint8_t *b, uint8_t *c, uint8_t d, struct rng_state *state)
{
    uint8_t r[d+1][d+1];
    uint8_t p1, p2;
    for(uint8_t i = 0; i <= d; i++) {
        for(uint8_t j = i+1; j <= d; j++) {
            r[i][j] = mersenne_twister(state);
            p1 = gf_mult(a[i], b[j]);
            p2 = gf_mult(a[j], b[i]);
            r[j][i] = (r[i][j] ^ p1) ^ (p2);
        }
    }
    for(uint8_t i = 0; i <= d; i++) {
        c[i] = gf_mult(a[i], b[i]);
        for(uint8_t j = 0; j <= d; j++) {
            if(i != j) {
                c[i] = c[i] ^ r[i][j];
            }
        }
    }
}

void sec_inv(uint8_t *x, uint8_t *y, uint8_t d, struct rng_state *state)
{
    uint8_t z[d+1];
    uint8_t w[d+1];
    uint8_t i;
    for(i = 0; i <= d; i++) {
        z[i] = gf_pow(x[i], 2);
    }
    refresh_masks(z, d, state);
    sec_mult(z, x, y, d, state);
    for(i = 0; i <= d; i++) {
        w[i] = gf_pow(y[i], 4);
    }
    refresh_masks(w, d, state);
    sec_mult(y, w, y, d, state);
    for(i = 0; i <= d; i++) {
        y[i] = gf_pow(y[i], 16);
    }
    sec_mult(y, w, y, d, state);
    sec_mult(y, z, y, d, state);
}


// RNGs

uint8_t xorshift8(struct rng_state *state)
{
    uint8_t x = state->val;
    x ^= x << 7;
    x ^= x >> 3;
    x ^= x << 2;
    return state->val = x;
}

uint8_t lcg(struct rng_state *state)
{
    uint8_t x = state->val;
    x = (uint8_t) (x * 157 + 205);
    return state->val = x;
}

void mersenne_twister_init(struct rng_state *state)
{
    const uint32_t mult = 1812433253ul;
    uint32_t seed = 3961ul;
    int i;
    for (i = 0; i < N; i++)
    {
        state->Y[i] = seed;
        seed = mult * (seed ^ (seed >> 30)) + (i+1);
    }
}

void mersenne_twister_update(struct rng_state *state)
{
    static const uint32_t  A[2] = { 0, 0x9908B0DF };
    int i = 0;
    for (; i < N - M; i++) {
        state->Y[i] = state->Y[i+(M)] ^ (((state->Y[i] & 0x80000000) | (state->Y[i+1] & 0x7FFFFFFF)) >> 1) ^ A[state->Y[i+1] & 1];
    }   
    for (; i < N-1; i++) {
        state->Y[i] = state->Y[i+(M-N)] ^ (((state->Y[i] & 0x80000000) | (state->Y[i+1] & 0x7FFFFFFF)) >> 1) ^ A[state->Y[i+1] & 1];
    }
    state->Y[N-1] = state->Y[M-1] ^ (((state->Y[N-1] & 0x80000000) | (state->Y[0] & 0x7FFFFFFF)) >> 1) ^ A[state->Y[0] & 1];
}

uint8_t mersenne_twister(struct rng_state *state)
{
    uint32_t res;
    static uint32_t index = N + 1;
    if(index >= N) {
        if(index > N) {
            mersenne_twister_init(state);
        }
        mersenne_twister_update(state);
        index = 0;
    }
    res = state->Y[index++];
    res ^= (res >> 11);
    res ^= (res <<  7) & 0x9D2C5680;
    res ^= (res << 15) & 0xEFC60000;
    res ^= (res >> 18);
    return (uint8_t) (res % 256);
}


// Standard Arithmetic

uint8_t gf_mult(uint8_t a, uint8_t b)
{
    uint8_t ap = LOG[a];
    uint8_t bp = LOG[b];
    uint8_t s, c;
    s = (ap + bp) % 256;
    c = (ap + bp) >> 8;
    uint8_t r = EXP[s + c];
    bool ma;
    ma = ap - 255;
    bool mb;
    mb = bp - 255;
    return r * ma * mb;
}

uint8_t gf_pow(uint8_t a, uint8_t p)
{
    if(p == 0) return 0;
    uint8_t i;
    uint8_t r = a;
    for(i = 1; i < p; i++) {
        r = gf_mult(r, a);
    }
    return r;
}