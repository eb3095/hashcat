/**
 * Author......: See docs/credits.txt
 * License.....: MIT
 */

#include <inttypes.h>
#include "common.h"
#include "types.h"
#include "modules.h"
#include "bitops.h"
#include "convert.h"
#include "shared.h"

static const u32   ATTACK_EXEC    = ATTACK_EXEC_OUTSIDE_KERNEL; 
static const u32   DGST_POS0      = 0;
static const u32   DGST_POS1      = 1;
static const u32   DGST_POS2      = 2;
static const u32   DGST_POS3      = 3;
static const u32   DGST_SIZE      = DGST_SIZE_4_8;
static const u32   HASH_CATEGORY  = HASH_CATEGORY_PASSWORD_MANAGER;
static const char *HASH_NAME      = "BestCrypt v4 Volume Encryption";
static const u64   KERN_TYPE      = 23901;
static const u32   OPTI_TYPE      = OPTI_TYPE_ZERO_BYTE;
static const u64   OPTS_TYPE      = OPTS_TYPE_PT_GENERATE_LE
                                  | OPTS_TYPE_ST_HEX;
static const u32   SALT_TYPE      = SALT_TYPE_EMBEDDED;
static const char *ST_PASS        = NULL; // the self-test is disabled, because the original scrypt settings would create a too long startup time
static const char *ST_HASH        = "$bcve$4$08$450886364ae094fb94b4a787e4fc2484b85f305082f63b1a$3a6ead2a1b806e51ba72efe66efde5a96ec4f4966a2155e1dbbb5eeaa55249a6430d648451ad5409d1813bedc98b55c8cd896adce3bca3d5e1d843ef9dd3a8b30ac23dcd92668113c7d9caf444acbdd5e60eaa18d36975089b24cf8459ffce00";

u32         module_attack_exec    (MAYBE_UNUSED const hashconfig_t *hashconfig, MAYBE_UNUSED const user_options_t *user_options, MAYBE_UNUSED const user_options_extra_t *user_options_extra) { return ATTACK_EXEC;     }
u32         module_dgst_pos0      (MAYBE_UNUSED const hashconfig_t *hashconfig, MAYBE_UNUSED const user_options_t *user_options, MAYBE_UNUSED const user_options_extra_t *user_options_extra) { return DGST_POS0;       }
u32         module_dgst_pos1      (MAYBE_UNUSED const hashconfig_t *hashconfig, MAYBE_UNUSED const user_options_t *user_options, MAYBE_UNUSED const user_options_extra_t *user_options_extra) { return DGST_POS1;       }
u32         module_dgst_pos2      (MAYBE_UNUSED const hashconfig_t *hashconfig, MAYBE_UNUSED const user_options_t *user_options, MAYBE_UNUSED const user_options_extra_t *user_options_extra) { return DGST_POS2;       }
u32         module_dgst_pos3      (MAYBE_UNUSED const hashconfig_t *hashconfig, MAYBE_UNUSED const user_options_t *user_options, MAYBE_UNUSED const user_options_extra_t *user_options_extra) { return DGST_POS3;       }
u32         module_dgst_size      (MAYBE_UNUSED const hashconfig_t *hashconfig, MAYBE_UNUSED const user_options_t *user_options, MAYBE_UNUSED const user_options_extra_t *user_options_extra) { return DGST_SIZE;       }
u32         module_hash_category  (MAYBE_UNUSED const hashconfig_t *hashconfig, MAYBE_UNUSED const user_options_t *user_options, MAYBE_UNUSED const user_options_extra_t *user_options_extra) { return HASH_CATEGORY;   }
const char *module_hash_name      (MAYBE_UNUSED const hashconfig_t *hashconfig, MAYBE_UNUSED const user_options_t *user_options, MAYBE_UNUSED const user_options_extra_t *user_options_extra) { return HASH_NAME;       }
u64         module_kern_type      (MAYBE_UNUSED const hashconfig_t *hashconfig, MAYBE_UNUSED const user_options_t *user_options, MAYBE_UNUSED const user_options_extra_t *user_options_extra) { return KERN_TYPE;       }
u32         module_opti_type      (MAYBE_UNUSED const hashconfig_t *hashconfig, MAYBE_UNUSED const user_options_t *user_options, MAYBE_UNUSED const user_options_extra_t *user_options_extra) { return OPTI_TYPE;       }
u64         module_opts_type      (MAYBE_UNUSED const hashconfig_t *hashconfig, MAYBE_UNUSED const user_options_t *user_options, MAYBE_UNUSED const user_options_extra_t *user_options_extra) { return OPTS_TYPE;       }
u32         module_salt_type      (MAYBE_UNUSED const hashconfig_t *hashconfig, MAYBE_UNUSED const user_options_t *user_options, MAYBE_UNUSED const user_options_extra_t *user_options_extra) { return SALT_TYPE;       }
const char *module_st_hash        (MAYBE_UNUSED const hashconfig_t *hashconfig, MAYBE_UNUSED const user_options_t *user_options, MAYBE_UNUSED const user_options_extra_t *user_options_extra) { return ST_HASH;         }
const char *module_st_pass        (MAYBE_UNUSED const hashconfig_t *hashconfig, MAYBE_UNUSED const user_options_t *user_options, MAYBE_UNUSED const user_options_extra_t *user_options_extra) { return ST_PASS;         }

typedef struct bestcrypt_scrypt
{
  u32 salt_buf[24];
  u32 ciphertext[96];

} bestcrypt_scrypt_t;

// limit scrypt accel otherwise we hurt ourself when calculating the scrypt tmto
// 16 is actually a bit low, we may need to change this depending on user response

static const char *SIGNATURE_BESTCRYPT_SCRYPT = "$bcve$";
static const u32   SCRYPT_MAX_ACCEL          = 16;
static const u32   SCRYPT_MAX_THREADS        = 1;

u32 module_kernel_accel_min (MAYBE_UNUSED const hashconfig_t *hashconfig, MAYBE_UNUSED const user_options_t *user_options, MAYBE_UNUSED const user_options_extra_t *user_options_extra)
{
  const u32 kernel_accel_min = 1;

  return kernel_accel_min;
}

u32 module_kernel_accel_max (MAYBE_UNUSED const hashconfig_t *hashconfig, MAYBE_UNUSED const user_options_t *user_options, MAYBE_UNUSED const user_options_extra_t *user_options_extra)
{
  const u32 kernel_accel_max = (user_options->kernel_accel_chgd == true) ? user_options->kernel_accel : SCRYPT_MAX_ACCEL;

  return kernel_accel_max;
}

u32 module_kernel_loops_min (MAYBE_UNUSED const hashconfig_t *hashconfig, MAYBE_UNUSED const user_options_t *user_options, MAYBE_UNUSED const user_options_extra_t *user_options_extra)
{
  const u32 kernel_loops_min = 1;

  return kernel_loops_min;
}

u32 module_kernel_loops_max (MAYBE_UNUSED const hashconfig_t *hashconfig, MAYBE_UNUSED const user_options_t *user_options, MAYBE_UNUSED const user_options_extra_t *user_options_extra)
{
  const u32 kernel_loops_max = 1;

  return kernel_loops_max;
}

u32 module_kernel_threads_min (MAYBE_UNUSED const hashconfig_t *hashconfig, MAYBE_UNUSED const user_options_t *user_options, MAYBE_UNUSED const user_options_extra_t *user_options_extra)
{
  const u32 kernel_threads_min = 1;

  return kernel_threads_min;
}

u32 module_kernel_threads_max (MAYBE_UNUSED const hashconfig_t *hashconfig, MAYBE_UNUSED const user_options_t *user_options, MAYBE_UNUSED const user_options_extra_t *user_options_extra)
{
  // limit scrypt accel otherwise we hurt ourself when calculating the scrypt tmto
  // 16 is actually a bit low, we may need to change this depending on user response

  const u32 kernel_threads_max = (user_options->kernel_threads_chgd == true) ? user_options->kernel_threads : SCRYPT_MAX_THREADS;

  return kernel_threads_max;
}

u64 module_esalt_size (MAYBE_UNUSED const hashconfig_t *hashconfig, MAYBE_UNUSED const user_options_t *user_options, MAYBE_UNUSED const user_options_extra_t *user_options_extra)
{
  const u64 esalt_size = (const u64) sizeof (bestcrypt_scrypt_t);

  return esalt_size;
}

u32 module_pw_max (MAYBE_UNUSED const hashconfig_t *hashconfig, MAYBE_UNUSED const user_options_t *user_options, MAYBE_UNUSED const user_options_extra_t *user_options_extra)
{
  // this overrides the reductions of PW_MAX in case optimized kernel is selected
  // IOW, even in optimized kernel mode it support length 256

  const u32 pw_max = PW_MAX;

  return pw_max;
}

u64 module_extra_buffer_size (MAYBE_UNUSED const hashconfig_t *hashconfig, MAYBE_UNUSED const user_options_t *user_options, MAYBE_UNUSED const user_options_extra_t *user_options_extra, MAYBE_UNUSED const hashes_t *hashes, MAYBE_UNUSED const hc_device_param_t *device_param)
{
  // we need to set the self-test hash settings to pass the self-test
  // the decoder for the self-test is called after this function

  const u32 scrypt_N = (hashes->salts_buf[0].scrypt_N) ? hashes->salts_buf[0].scrypt_N : 32768;
  const u32 scrypt_r = (hashes->salts_buf[0].scrypt_r) ? hashes->salts_buf[0].scrypt_r : 16;
  //const u32 scrypt_p = (hashes->salts_buf[0].scrypt_p) ? hashes->salts_buf[0].scrypt_p : 1;

  const u64 kernel_power_max = (u64)(device_param->device_processors * hashconfig->kernel_threads_max * hashconfig->kernel_accel_max);

  u32 tmto_start = 1;
  u32 tmto_stop  = 6;

  if (user_options->scrypt_tmto)
  {
    tmto_start = user_options->scrypt_tmto;
    tmto_stop  = user_options->scrypt_tmto;
  }

  // size_pws

  const u64 size_pws = kernel_power_max * sizeof (pw_t);

  const u64 size_pws_amp = size_pws;

  // size_pws_comp

  const u64 size_pws_comp = kernel_power_max * (sizeof (u32) * 64);

  // size_pws_idx

  const u64 size_pws_idx = (kernel_power_max + 1) * sizeof (pw_idx_t);

  // size_tmps

  const u64 size_tmps = kernel_power_max * hashconfig->tmp_size;

  // size_hooks

  const u64 size_hooks = kernel_power_max * hashconfig->hook_size;

  const u64 scrypt_extra_space
    = device_param->size_bfs
    + device_param->size_combs
    + device_param->size_digests
    + device_param->size_esalts
    + device_param->size_markov_css
    + device_param->size_plains
    + device_param->size_results
    + device_param->size_root_css
    + device_param->size_rules
    + device_param->size_rules_c
    + device_param->size_salts
    + device_param->size_shown
    + device_param->size_tm
    + device_param->size_st_digests
    + device_param->size_st_salts
    + device_param->size_st_esalts
    + size_pws
    + size_pws_amp
    + size_pws_comp
    + size_pws_idx
    + size_tmps
    + size_hooks;

  bool not_enough_memory = true;

  u64 size_scrypt = 0;

  u32 tmto;

  for (tmto = tmto_start; tmto <= tmto_stop; tmto++)
  {
    size_scrypt = (128 * scrypt_r) * scrypt_N;

    size_scrypt /= 1u << tmto;

    size_scrypt *= kernel_power_max;

    if ((size_scrypt / 4) > device_param->device_maxmem_alloc) continue;

    if ((size_scrypt + scrypt_extra_space) > device_param->device_available_mem) continue;

    not_enough_memory = false;

    break;
  }

  if (not_enough_memory == true) return -1;

  return size_scrypt;
}

u64 module_tmp_size (MAYBE_UNUSED const hashconfig_t *hashconfig, MAYBE_UNUSED const user_options_t *user_options, MAYBE_UNUSED const user_options_extra_t *user_options_extra)
{
  const u64 tmp_size = 0; // we'll add some later

  return tmp_size;
}

u64 module_extra_tmp_size (MAYBE_UNUSED const hashconfig_t *hashconfig, MAYBE_UNUSED const user_options_t *user_options, MAYBE_UNUSED const user_options_extra_t *user_options_extra, MAYBE_UNUSED const hashes_t *hashes)
{
  // we need to set the self-test hash settings to pass the self-test
  // the decoder for the self-test is called after this function

  const u32 scrypt_N = (hashes->salts_buf[0].scrypt_N) ? hashes->salts_buf[0].scrypt_N : 32768;
  const u32 scrypt_r = (hashes->salts_buf[0].scrypt_r) ? hashes->salts_buf[0].scrypt_r : 16;
  const u32 scrypt_p = (hashes->salts_buf[0].scrypt_p) ? hashes->salts_buf[0].scrypt_p : 1;

  // we need to check that all hashes have the same scrypt settings

  for (u32 i = 1; i < hashes->salts_cnt; i++)
  {
    if ((hashes->salts_buf[i].scrypt_N != scrypt_N)
     || (hashes->salts_buf[i].scrypt_r != scrypt_r)
     || (hashes->salts_buf[i].scrypt_p != scrypt_p))
    {
      return -1;
    }
  }

  const u64 tmp_size = (u64)(128 * scrypt_r * scrypt_p);

  return tmp_size;
}

bool module_unstable_warning (MAYBE_UNUSED const hashconfig_t *hashconfig, MAYBE_UNUSED const user_options_t *user_options, MAYBE_UNUSED const user_options_extra_t *user_options_extra, MAYBE_UNUSED const hc_device_param_t *device_param)
{
  if (device_param->opencl_platform_vendor_id == VENDOR_ID_APPLE)
  {
    // Invalid extra buffer size.
    if ((device_param->opencl_device_vendor_id == VENDOR_ID_INTEL_SDK) && (device_param->opencl_device_type & CL_DEVICE_TYPE_GPU))
    {
      return true;
    }
  }

  // amdgpu-pro-18.50-708488-ubuntu-18.04: Segmentation fault
  if ((device_param->opencl_device_vendor_id == VENDOR_ID_AMD) && (device_param->has_vperm == false))
  {
    return true;
  }

  return false;
}

bool module_jit_cache_disable (MAYBE_UNUSED const hashconfig_t *hashconfig, MAYBE_UNUSED const user_options_t *user_options, MAYBE_UNUSED const user_options_extra_t *user_options_extra, MAYBE_UNUSED const hashes_t *hashes, MAYBE_UNUSED const hc_device_param_t *device_param)
{
  return true;
}

char *module_jit_build_options (MAYBE_UNUSED const hashconfig_t *hashconfig, MAYBE_UNUSED const user_options_t *user_options, MAYBE_UNUSED const user_options_extra_t *user_options_extra, MAYBE_UNUSED const hashes_t *hashes, MAYBE_UNUSED const hc_device_param_t *device_param)
{
  const u32 scrypt_N = (hashes->salts_buf[0].scrypt_N) ? hashes->salts_buf[0].scrypt_N : 32768;
  const u32 scrypt_r = (hashes->salts_buf[0].scrypt_r) ? hashes->salts_buf[0].scrypt_r : 16;
  const u32 scrypt_p = (hashes->salts_buf[0].scrypt_p) ? hashes->salts_buf[0].scrypt_p : 1;

  const u64 extra_buffer_size = device_param->extra_buffer_size;

  const u64 kernel_power_max = (u64)(device_param->device_processors * hashconfig->kernel_threads_max * hashconfig->kernel_accel_max);

  const u64 size_scrypt = (u64)(128 * scrypt_r * scrypt_N);

  const u64 scrypt_tmto_final = (kernel_power_max * size_scrypt) / extra_buffer_size;

  const u64 tmp_size = (u64)(128 * scrypt_r * scrypt_p);

  char *jit_build_options = NULL;

  hc_asprintf (&jit_build_options, "-DSCRYPT_N=%u -DSCRYPT_R=%u -DSCRYPT_P=%u -DSCRYPT_TMTO=%" PRIu64 " -DSCRYPT_TMP_ELEM=%" PRIu64,
    hashes->salts_buf[0].scrypt_N,
    hashes->salts_buf[0].scrypt_r,
    hashes->salts_buf[0].scrypt_p,
    scrypt_tmto_final,
    tmp_size / 16);

  return jit_build_options;
}

int module_hash_decode (MAYBE_UNUSED const hashconfig_t *hashconfig, MAYBE_UNUSED void *digest_buf, MAYBE_UNUSED salt_t *salt, MAYBE_UNUSED void *esalt_buf, MAYBE_UNUSED void *hook_salt_buf, MAYBE_UNUSED hashinfo_t *hash_info, const char *line_buf, MAYBE_UNUSED const int line_len)
{

  u32 *digest = (u32 *) digest_buf;

  bestcrypt_scrypt_t *bestcrypt_scrypt = (bestcrypt_scrypt_t *) esalt_buf;

  token_t token;

  token.token_cnt  = 5;

  token.signatures_cnt    = 1;
  token.signatures_buf[0] = SIGNATURE_BESTCRYPT_SCRYPT;

  token.len[0]     = 6;
  token.attr[0]    = TOKEN_ATTR_FIXED_LENGTH
                   | TOKEN_ATTR_VERIFY_SIGNATURE;

  token.len_min[1] = 1;
  token.len_max[1] = 1;
  token.sep[1]     = '$';
  token.attr[1]    = TOKEN_ATTR_VERIFY_LENGTH
                   | TOKEN_ATTR_VERIFY_DIGIT;

  token.len_min[2] = 2;
  token.len_max[2] = 2;
  token.sep[2]     = '$';
  token.attr[2]    = TOKEN_ATTR_VERIFY_LENGTH
                   | TOKEN_ATTR_VERIFY_HEX;

  token.len_min[3] = 48;
  token.len_max[3] = 48;
  token.sep[3]     = '$';
  token.attr[3]    = TOKEN_ATTR_VERIFY_LENGTH
                   | TOKEN_ATTR_VERIFY_HEX;

  token.len[4]     = 192;
  token.attr[4]    = TOKEN_ATTR_FIXED_LENGTH
                   | TOKEN_ATTR_VERIFY_HEX;

  const int rc_tokenizer = input_tokenizer ((const u8 *) line_buf, line_len, &token);

  if (rc_tokenizer != PARSER_OK) return (rc_tokenizer);

  const u8 *format_type_pos = token.buf[1];

  if (format_type_pos[0] != '4') return (PARSER_SALT_VALUE);

  const u8 *crypto_type_pos = token.buf[2];

  if (crypto_type_pos[0] != '0') return (PARSER_SALT_VALUE);
  if (crypto_type_pos[1] != '8') return (PARSER_SALT_VALUE);

  // scrypt settings
  const u32 scrypt_N = 32768;
  const u32 scrypt_r = 16;
  const u32 scrypt_p = 1;

  salt->scrypt_N = scrypt_N;
  salt->scrypt_r = scrypt_r;
  salt->scrypt_p = scrypt_p;

  // salt

  const u8 *salt_pos = token.buf[3];
  const int salt_len = token.len[3];

  const bool parse_rc = generic_salt_decode (hashconfig, salt_pos, salt_len, (u8 *) salt->salt_buf, (int *) &salt->salt_len);

  if (parse_rc == false) return (PARSER_SALT_LENGTH);

  bestcrypt_scrypt->salt_buf[0] = salt->salt_buf[0];
  bestcrypt_scrypt->salt_buf[1] = salt->salt_buf[1];

  bestcrypt_scrypt->salt_buf[0] = byte_swap_32 (salt->salt_buf[0]);
  bestcrypt_scrypt->salt_buf[1] = byte_swap_32 (salt->salt_buf[1]);

  salt->salt_iter = 1;

  // ciphertext

  const u8 *ciphertext_pos = token.buf[4];
  const int ciphertext_len = token.len[4];

  hex_decode (ciphertext_pos, ciphertext_len, (u8 *) bestcrypt_scrypt->ciphertext);


  digest[0] = bestcrypt_scrypt->ciphertext[16];
  digest[1] = bestcrypt_scrypt->ciphertext[17];
  digest[2] = bestcrypt_scrypt->ciphertext[18];
  digest[3] = bestcrypt_scrypt->ciphertext[19];

  return (PARSER_OK);
}

int module_hash_encode (MAYBE_UNUSED const hashconfig_t *hashconfig, MAYBE_UNUSED const void *digest_buf, MAYBE_UNUSED const salt_t *salt, MAYBE_UNUSED const void *esalt_buf, MAYBE_UNUSED const void *hook_salt_buf, MAYBE_UNUSED const hashinfo_t *hash_info, char *line_buf, MAYBE_UNUSED const int line_size)
{

  char tmp_salt[SALT_MAX * 2];

  const int salt_len = generic_salt_encode (hashconfig, (const u8 *) salt->salt_buf, (const int) salt->salt_len, (u8 *) tmp_salt);

  tmp_salt[salt_len] = 0;

  bestcrypt_scrypt_t *bestcrypt_scrypt = (bestcrypt_scrypt_t *) esalt_buf;

  u8 data_hex[193] = { 0 };

  hex_encode ((u8 *) bestcrypt_scrypt->ciphertext, 96, data_hex);

  const int line_len = snprintf (line_buf, line_size, "%s4$08$%s$%s",
    SIGNATURE_BESTCRYPT_SCRYPT,
    (char *) tmp_salt,
    data_hex
  ); 

  return line_len;
}

void module_init (module_ctx_t *module_ctx)
{
  module_ctx->module_context_size             = MODULE_CONTEXT_SIZE_CURRENT;
  module_ctx->module_interface_version        = MODULE_INTERFACE_VERSION_CURRENT;

  module_ctx->module_attack_exec              = module_attack_exec;
  module_ctx->module_benchmark_esalt          = MODULE_DEFAULT;
  module_ctx->module_benchmark_hook_salt      = MODULE_DEFAULT;
  module_ctx->module_benchmark_mask           = MODULE_DEFAULT;
  module_ctx->module_benchmark_salt           = MODULE_DEFAULT;
  module_ctx->module_build_plain_postprocess  = MODULE_DEFAULT;
  module_ctx->module_deep_comp_kernel         = MODULE_DEFAULT;
  module_ctx->module_dgst_pos0                = module_dgst_pos0;
  module_ctx->module_dgst_pos1                = module_dgst_pos1;
  module_ctx->module_dgst_pos2                = module_dgst_pos2;
  module_ctx->module_dgst_pos3                = module_dgst_pos3;
  module_ctx->module_dgst_size                = module_dgst_size;
  module_ctx->module_dictstat_disable         = MODULE_DEFAULT;
  module_ctx->module_esalt_size               = module_esalt_size;
  module_ctx->module_extra_buffer_size        = module_extra_buffer_size;
  module_ctx->module_extra_tmp_size           = module_extra_tmp_size;
  module_ctx->module_forced_outfile_format    = MODULE_DEFAULT;
  module_ctx->module_hash_binary_count        = MODULE_DEFAULT;
  module_ctx->module_hash_binary_parse        = MODULE_DEFAULT;
  module_ctx->module_hash_binary_save         = MODULE_DEFAULT;
  module_ctx->module_hash_decode_potfile      = MODULE_DEFAULT;
  module_ctx->module_hash_decode_zero_hash    = MODULE_DEFAULT;
  module_ctx->module_hash_decode              = module_hash_decode;
  module_ctx->module_hash_encode_status       = MODULE_DEFAULT;
  module_ctx->module_hash_encode_potfile      = MODULE_DEFAULT;
  module_ctx->module_hash_encode              = module_hash_encode;
  module_ctx->module_hash_init_selftest       = MODULE_DEFAULT;
  module_ctx->module_hash_mode                = MODULE_DEFAULT;
  module_ctx->module_hash_category            = module_hash_category;
  module_ctx->module_hash_name                = module_hash_name;
  module_ctx->module_hashes_count_min         = MODULE_DEFAULT;
  module_ctx->module_hashes_count_max         = MODULE_DEFAULT;
  module_ctx->module_hlfmt_disable            = MODULE_DEFAULT;
  module_ctx->module_hook12                   = MODULE_DEFAULT;
  module_ctx->module_hook23                   = MODULE_DEFAULT;
  module_ctx->module_hook_salt_size           = MODULE_DEFAULT;
  module_ctx->module_hook_size                = MODULE_DEFAULT;
  module_ctx->module_jit_build_options        = module_jit_build_options;
  module_ctx->module_jit_cache_disable        = module_jit_cache_disable;
  module_ctx->module_kernel_accel_max         = module_kernel_accel_max;
  module_ctx->module_kernel_accel_min         = module_kernel_accel_min;
  module_ctx->module_kernel_loops_max         = module_kernel_loops_max;
  module_ctx->module_kernel_loops_min         = module_kernel_loops_min;
  module_ctx->module_kernel_threads_max       = module_kernel_threads_max;
  module_ctx->module_kernel_threads_min       = module_kernel_threads_min;
  module_ctx->module_kern_type                = module_kern_type;
  module_ctx->module_kern_type_dynamic        = MODULE_DEFAULT;
  module_ctx->module_opti_type                = module_opti_type;
  module_ctx->module_opts_type                = module_opts_type;
  module_ctx->module_outfile_check_disable    = MODULE_DEFAULT;
  module_ctx->module_outfile_check_nocomp     = MODULE_DEFAULT;
  module_ctx->module_potfile_custom_check     = MODULE_DEFAULT;
  module_ctx->module_potfile_disable          = MODULE_DEFAULT;
  module_ctx->module_potfile_keep_all_hashes  = MODULE_DEFAULT;
  module_ctx->module_pwdump_column            = MODULE_DEFAULT;
  module_ctx->module_pw_max                   = module_pw_max;
  module_ctx->module_pw_min                   = MODULE_DEFAULT;
  module_ctx->module_salt_max                 = MODULE_DEFAULT;
  module_ctx->module_salt_min                 = MODULE_DEFAULT;
  module_ctx->module_salt_type                = module_salt_type;
  module_ctx->module_separator                = MODULE_DEFAULT;
  module_ctx->module_st_hash                  = module_st_hash;
  module_ctx->module_st_pass                  = module_st_pass;
  module_ctx->module_tmp_size                 = module_tmp_size;
  module_ctx->module_unstable_warning         = module_unstable_warning;
  module_ctx->module_warmup_disable           = MODULE_DEFAULT;
}
