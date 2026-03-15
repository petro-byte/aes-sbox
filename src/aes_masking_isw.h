#define MASKING_ORDER 3
#define N 624
#define M 397

struct rng_state {
    uint8_t val;
    uint32_t Y[N];
};

void validate_sec_mult();

// S-Box and Affine Transform
void sec_sbox(uint8_t *shares, uint8_t d);
uint8_t affine_transform(uint8_t x);
uint8_t left_rot(uint8_t x, uint8_t n);

// Masking
void share(uint8_t x, uint8_t *shares, uint8_t d);
void reset_share(uint8_t *shares, uint8_t d);
void refresh_masks(uint8_t *shares, uint8_t d, struct rng_state *state);
uint8_t unshare(uint8_t *shares, uint8_t d);

// Secure Arithmetic
void sec_mult(uint8_t *a, uint8_t *b, uint8_t *c, uint8_t d, struct rng_state *state);
void sec_inv(uint8_t *x, uint8_t *y, uint8_t d, struct rng_state *state);

// RNGs
uint8_t xorshift8(struct rng_state *state);
uint8_t lcg(struct rng_state *state);
void mersenne_twister_init(struct rng_state *state);
void mersenne_twister_update(struct rng_state *state);
uint8_t mersenne_twister(struct rng_state *state);

// Branch-Free Arithmetic
uint8_t gf_mult(uint8_t a, uint8_t b);
uint8_t gf_pow(uint8_t a, uint8_t p);
