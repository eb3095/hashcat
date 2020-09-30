/**
 * Author......: See docs/credits.txt
 * License.....: MIT
 */

#define NEW_SIMD_CODE

#ifdef KERNEL_STATIC
#include "inc_vendor.h"
#include "inc_types.h"
#include "inc_platform.cl"
#include "inc_common.cl"
#include "inc_simd.cl"
#include "inc_hash_sha512.cl"
#endif

KERNEL_FQ void m22200_mxx (KERN_ATTR_VECTOR ())
{
  /**
   * modifier
   */

  const u64 lid = get_local_id (0);
  const u64 gid = get_global_id (0);

  if (gid >= gid_max) return;

  /**
   * base
   */

  u32x z[32] = { 0 };

  const u32 pw_len = pws[gid].pw_len;

  u32x w[64] = { 0 };

  for (u32 i = 0, idx = 0; i < pw_len; i += 4, idx += 1)
  {
    w[idx] = pws[gid].i[idx];
  }

  sha512_ctx_t ctx0;

  sha512_init (&ctx0);

  sha512_update_global (&ctx0, salt_bufs[SALT_POS].salt_buf, salt_bufs[SALT_POS].salt_len);

  /**
   * loop
   */

  u32x w0l = w[0];

  for (u32 il_pos = 0; il_pos < il_cnt; il_pos += VECT_SIZE)
  {
    const u32x w0r = words_buf_r[il_pos / VECT_SIZE];

    const u32x w0 = w0l | w0r;

    w[0] = w0;

    sha512_ctx_vector_t ctx;

    sha512_init_vector_from_scalar (&ctx, &ctx0);

    sha512_update_vector (&ctx, w, pw_len);

    sha512_update_vector (&ctx, z, 1);

    sha512_final_vector (&ctx);

    const u32x r0 = l32_from_64 (ctx.h[7]);
    const u32x r1 = h32_from_64 (ctx.h[7]);
    const u32x r2 = l32_from_64 (ctx.h[3]);
    const u32x r3 = h32_from_64 (ctx.h[3]);

    COMPARE_M_SIMD (r0, r1, r2, r3);
  }
}

KERNEL_FQ void m22200_sxx (KERN_ATTR_VECTOR ())
{
  /**
   * modifier
   */

  const u64 lid = get_local_id (0);
  const u64 gid = get_global_id (0);

  if (gid >= gid_max) return;

  /**
   * digest
   */

  const u32 search[4] =
  {
    digests_buf[DIGESTS_OFFSET].digest_buf[DGST_R0],
    digests_buf[DIGESTS_OFFSET].digest_buf[DGST_R1],
    digests_buf[DIGESTS_OFFSET].digest_buf[DGST_R2],
    digests_buf[DIGESTS_OFFSET].digest_buf[DGST_R3]
  };

  /**
   * base
   */

  u32x z[32] = { 0 };

  const u32 pw_len = pws[gid].pw_len;

  u32x w[64] = { 0 };

  for (u32 i = 0, idx = 0; i < pw_len; i += 4, idx += 1)
  {
    w[idx] = pws[gid].i[idx];
  }

  sha512_ctx_t ctx0;

  sha512_init (&ctx0);

  sha512_update_global (&ctx0, salt_bufs[SALT_POS].salt_buf, salt_bufs[SALT_POS].salt_len);

  /**
   * loop
   */

  u32x w0l = w[0];

  for (u32 il_pos = 0; il_pos < il_cnt; il_pos += VECT_SIZE)
  {
    const u32x w0r = words_buf_r[il_pos / VECT_SIZE];

    const u32x w0 = w0l | w0r;

    w[0] = w0;

    sha512_ctx_vector_t ctx;

    sha512_init_vector_from_scalar (&ctx, &ctx0);

    sha512_update_vector (&ctx, w, pw_len);

    sha512_update_vector (&ctx, z, 1);

    sha512_final_vector (&ctx);

    const u32x r0 = l32_from_64 (ctx.h[7]);
    const u32x r1 = h32_from_64 (ctx.h[7]);
    const u32x r2 = l32_from_64 (ctx.h[3]);
    const u32x r3 = h32_from_64 (ctx.h[3]);

    COMPARE_S_SIMD (r0, r1, r2, r3);
  }
}
