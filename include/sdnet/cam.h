/////////////////////////////////////////////////////////////////////////////
//
// Copyright 2018 Xilinx, Inc. All rights reserved.
// This file contains confidential and proprietary information of Xilinx, Inc.
// and is protected under U.S. and international copyright and other
// intellectual property laws.
//
// DISCLAIMER
// This disclaimer is not a license and does not grant any
// rights to the materials distributed herewith. Except as
// otherwise provided in a valid license issued to you by
// Xilinx, and to the maximum extent permitted by applicable
// law: (1) THESE MATERIALS ARE MADE AVAILABLE "AS IS" AND
// WITH ALL FAULTS, AND XILINX HEREBY DISCLAIMS ALL WARRANTIES
// AND CONDITIONS, EXPRESS, IMPLIED, OR STATUTORY, INCLUDING
// BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, NON-
// INFRINGEMENT, OR FITNESS FOR ANY PARTICULAR PURPOSE; and
// (2) Xilinx shall not be liable (whether in contract or tort,
// including negligence, or under any other theory of
// liability) for any loss or damage of any kind or nature
// related to, arising under or in connection with these
// materials, including for any direct, or any indirect,
// special, incidental, or consequential loss or damage
// (including loss of data, profits, goodwill, or any type of
// loss or damage suffered as a result of any action brought
// by a third party) even if such damage or loss was
// reasonably foreseeable or Xilinx had been advised of the
// possibility of the same.
//
// CRITICAL APPLICATIONS
// Xilinx products are not designed or intended to be fail-
// safe, or for use in any application requiring fail-safe
// performance, such as life-support or safety devices or
// systems, Class III medical devices, nuclear facilities,
// applications related to the deployment of airbags, or any
// other applications that could lead to death, personal
// injury, or severe property or environmental damage
// (individually and collectively, "Critical
// Applications"). Customer assumes the sole risk and
// liability of any use of Xilinx products in Critical
// Applications, subject only to applicable laws and
// regulations governing limitations on product liability.
//
// THIS COPYRIGHT NOTICE AND DISCLAIMER MUST BE RETAINED AS
// PART OF THIS FILE AT ALL TIMES.
//


/**
 *
 * Public cam argument configuration and error codes
 *
 * @file cam.h
 *
 *
 * @addtogroup camarg Configuration Parameters and Error Codes
 * @{
 */

#ifndef _CAM_H_
#define _CAM_H_


#include <stdint.h>
#include <stdbool.h>


#ifdef __cplusplus
extern "C" {
#endif


/**
 * Error numbers. These are the return codes from the bcam, tcam and stcam API functions.
 * If zero is returned, the function call was successful
 */
enum cam_errnos {
    CAM_OK                                 = 0,
    /* Non-fatal errors */
    CAM_ERROR_FULL                         = 1,
    CAM_ERROR_NO_OPEN                      = 2,
    CAM_ERROR_INVALID_ARG                  = 3,
    CAM_ERROR_WRONG_KEY_WIDTH              = 5,
    CAM_ERROR_TOO_MANY_INSTANCES           = 6,
    CAM_ERROR_WRONG_BIT_FIELD_MASK         = 7,
    CAM_ERROR_WRONG_CONST_FIELD_MASK       = 8,
    CAM_ERROR_WRONG_UNUSED_FIELD_MASK      = 9,
    CAM_ERROR_INVALID_TERNARY_FIELD_LEN    = 10,
    CAM_ERROR_WRONG_PRIO_WIDTH             = 11,
    CAM_ERROR_WRONG_MAX                    = 12,
    CAM_ERROR_DUPLICATE_FOUND              = 13,
    CAM_ERROR_WRONG_PREFIX                 = 14,
    CAM_ERROR_WRONG_PREFIX_MASK            = 15,
    CAM_ERROR_WRONG_RANGE                  = 16,
    CAM_ERROR_WRONG_RANGE_MASK             = 17,
    CAM_ERROR_KEY_NOT_FOUND                = 18,
    CAM_ERROR_WRONG_MIN                    = 19,
    CAM_ERROR_WRONG_PRIO                   = 20,
    CAM_ERROR_WRONG_LIST_LENGTH            = 21,
    CAM_ERROR_WRONG_NUMBER_OF_SLOTS        = 22,
    CAM_ERROR_INVALID_MEM_TYPE             = 23,
    CAM_ERROR_TOO_HIGH_FREQUENCY           = 24,
    CAM_ERROR_WRONG_TERNARY_MASK           = 25,
    CAM_ERROR_MASKED_KEY_BIT_IS_SET        = 26,
    CAM_ERROR_INVALID_MODE                 = 28,
    CAM_ERROR_WRONG_RESPONSE_WIDTH         = 29,
    CAM_ERROR_FORMAT_SYNTAX                = 30, /* Syntax error in format */
    CAM_ERROR_TOO_MANY_FIELDS              = 31, /* Too many fields */
    CAM_ERROR_TOO_MANY_RANGES              = 32, /* Too many ranges */
    CAM_ERROR_INVALID_RANGE_LEN            = 33, /* Invalid range len */
    CAM_ERROR_INVALID_RANGE_START          = 34, /* Range start + len exceeds key len */
    CAM_ERROR_INVALID_PREFIX_LEN           = 35, /* Invalid prefix len */
    CAM_ERROR_INVALID_PREFIX_START         = 36, /* Prefix start + len exceeds key len */
    CAM_ERROR_INVALID_PREFIX_KEY           = 37, /* Prefix key longer than prefix len */
    CAM_ERROR_INVALID_BIT_FIELD_LEN        = 38, /* Invalid bit len */
    CAM_ERROR_INVALID_BIT_FIELD_START      = 39, /* Bit start + len exceeds key len */
    CAM_ERROR_INVALID_CONST_FIELD_LEN      = 40, /* Invalid const len */
    CAM_ERROR_INVALID_CONST_FIELD_START    = 41, /* Const start + len exceeds key len */
    CAM_ERROR_INVALID_UNUSED_FIELD_LEN     = 42, /* Invalid unused len */
    CAM_ERROR_INVALID_UNUSED_FIELD_START   = 43, /* Unused start + len exceeds key len */
    CAM_ERROR_MAX_KEY_LEN_EXCEED           = 44, /* Sum of all field lengths exceeds key len */
    CAM_ERROR_INVALID_PRIO_AND_INDEX_WIDTH = 45, /* Prio width is too narrow or to wide */
    CAM_ERROR_TOO_MANY_UNITS               = 46, /* Too many logical or physical units */

    /* Error during tcam_insert. The entry has not been inserted since
     HW resources are partially depleted. It
     is possible to continue using the instance, inserting entries using already
     allocated resources, updating entries, get entries or delete entries. */
    CAM_ERROR_NO_MASK                      = 47,
    CAM_ERROR_INVALID_MEMORY_WIDTH         = 48, /* The memory structure defined by the
                                                    combined key and response width is
                                                    too wide */

    CAM_ERROR_UNSUPPORTED_COMMAND          = 49, /* Unsupported Computational BCAM command */
    CAM_ERROR_ENVIRONMENT                  = 50, /* Computational BCAM environment error at transport layer or IP non responsive */
    CAM_ERROR_UNSUPPORTED_CAM_TYPE         = 51, /* API request for an unsupported cam type */
    CAM_ERROR_NULL_POINTER                 = 52, /* Null pointer */
    CAM_ERROR_TOO_MANY_PCS                 = 53, /* Too many pseudo-channels (PCs) are used */
    CAM_ERROR_CONFIGURATION                = 54, /* Computational BCAM configurations do not match sw setttings */
    CAM_ERROR_ENVIRONMENT_FSMBUSY          = 55, /* Computational BCAM FSM reporting busy */
    CAM_ERROR_ENVIRONMENT_POLLED_OUT       = 56, /* Computational BCAM has polled stutus register beyond the max allowed */

    /* Fatal error codes: CAM_ERROR_FATAL_BASE_START <= error code < CAM_ERROR_FATAL_BASE_END */
    /* The instance is not working anymore */
    CAM_ERROR_FATAL_BASE_START             = 100,
    CAM_ERROR_FATAL_BASE_END               = 199,
    CAM_ERROR_NOMEM                        = CAM_ERROR_FATAL_BASE_END,
    CAM_ERROR_MALLOC_FAILED                = CAM_ERROR_FATAL_BASE_END - 1,

};


/**
 * Debug flags to enable debug printouts and hardware verification.
 */
#define CAM_DEBUG_NONE           0x00000000  /* Quiet, no error messages */
#define CAM_DEBUG_NO_ERROR_MSG   0x00000001  /* Disable software error messages printout */
#define CAM_DEBUG_ARGS           0x00000002  /* Software echoes input arguments */
#define CAM_DEBUG_VERIFY_WR      0x00000004  /* Software immediately reads back written data from hardware
                                                for verification. Mismatching data is reported with error
                                                message. */
#define CAM_DEBUG_VERIFY_SHADOW  0x00000008  /* 'get_by_response' and 'get_by_key' operations read data from
                                               hardware and verifies software shadow data */
#define CAM_DEBUG_VERBOSE_VERIFY 0x00000010  /* Enables printout of all verify operations, including correct
                                               compare (applies to CAM_DEBUG_VERIFY_WR, CAM_DEBUG_VERIFY_SHADOW,
                                               and CAM_DEBUG_VERIFY_CONFIG) */
#define CAM_DEBUG_SKIP_MEM_INIT  0x00000020  /* Skip memory initialization, useful to speedup Verilog
                                               simulation */
#define CAM_DEBUG_CONFIG         0x00000040  /* Both software and Verilog hardware prints
                                               configuration parameters */
#define CAM_DEBUG_VERIFY_CONFIG  0x00000080  /* Configuration parameters are read from hardware for
                                               verification */
#define CAM_DEBUG_KEY_MASKING    0x00000100  /* Enable error check for key value '1' in masked (wildcarded)
                                               bit position. */
#define CAM_DEBUG_STATS          0x00000200  /* Enable statistics printout in the 'bcam_destroy' and
                                               'stcam_destroy' functions. */
#define CAM_DEBUG_HW_WR          0x00010000  /* Verilog hardware prints write operations */
#define CAM_DEBUG_HW_RD          0x00020000  /* Verilog hardware prints read operations */
#define CAM_DEBUG_HW_LOOKUP      0x00040000  /* Verilog hardware prints lookup operations */


/**
 * Option flags to control API behavior.
 */
#define CAM_OPTION_NONE     0x00000000 /* Default */
#define CAM_OPTION_NARROW   0x00000001 /* All proceeding API calls use narrow key and response width. The flag needs to be cleared
                                         to resume using native key and response width. This option has only affect in mixed mode.  */


typedef struct cam_arg_t cam_arg_t;

/**
 * Creates cam_arg which is used to pass arguments during the creation of a bcam, tcam or stcam instance.
 * If the function was successful, 0 is returned. If the function fails an error code is returned.
 *
 * @param[out]  out_cam_arg   The newly created cam_arg. If successful memory was allocated.
 *                            If unsuccessful, no memory was allocated and out_cam_arg is left unchanged.
 *
 * @return                    Returns 0 for success, otherwise the error code.
 *
 */
int cam_arg_create(cam_arg_t **out_cam_arg);


/**
 * Destroy cam_arg. The memory allocated by cam_arg_create is released.
 *
 * @param[in]  cam_arg   Pointer to cam_arg.
 *
 */
void cam_arg_destroy(cam_arg_t *cam_arg);


/**
 * The function is only used for tcam. This function shall not be called for bcam or stcam.
 * The format string is used to specify the location and size of range fields, since
 * ternary matching hardware is replaced by range matching hardware for these fields. Furthermore the format string
 * is used to determine key width and to guide the tcam API software. The key width can not be more than 991 bits.
 * In addition, 2 x key width + response width + priority width + 1 can not exceed 1472/1984 (BRAM/URAM) bits.
 * Sets the format string of cam_arg. If the cam_arg_set_format is unsuccessful, the format is not set.
 * The default format of cam_arg is "/0".
 *
 * @param[in]   cam_arg Pointer to cam_arg
 * @param[in]   format  String describing the format of all entries
 *                      of a tcam instance. An entry consists of
 *                      several fields. Fields are separated by ':'.
 *                      The string is read left to right. The first
 *                      field will correspond to the least significant bits
 *                      of the key. The key is specified as little-endian.
 *                      Each field is specified as: \<field len\>
 *                      \<field type\>. The following field types and
 *                      lengths are supported. There are no alignment
 *                      restrictions for the fields.
 *                      - r - Range, most significant byte of field is
 *                          in most significant byte of key.
 *                          The allowed length of the field is 1 to 16 bits.
 *                          Maximum 8 ranges per format
 *                      - p - Prefix, most significant byte of field is in
 *                          most significant byte of key.
 *                         The allowed length of the field is 1 to 128 bits.
 *                      - b - bit field.
 *                          The allowed length of the field is 1 to 128 bits.
 *                      - t - Ternary field.
 *                          The allowed length of the field is 2 to 16 bits.
 *                      - c - Bit field with mask always set.
 *                          The allowed length of the field is 1 to 128 bits.
 *                      - u - Unused bit field with mask always cleared.
 *                          The allowed length of the field is 1 to 128 bits.
 *
 *                      For example, the format "16r:16r:64p:64p:48b:48b"
 *                      contains 2 x 16 bit range fields for port numbers,
 *                      2 x 64 bit prefix fields for IPv6 addresses and
 *                      2 x 48 bit fields for MAC addresses. The 16r field is
 *                      located in the 16 least significant bits of the key
 *
 * @return              Returns 0 for success, otherwise the error code.
 *
 */
int cam_arg_set_format(cam_arg_t *cam_arg, char *format);


/**
 * Sets the instance context pointer of cam_arg. The context is user-defined and will always be passed
 * when the following functions are invoked:
 * hw write, hw read, info print and error print.
 * An example of usage of context could be for addressing multiple cam instances.
 * Each instance would be given a unique base address using cam_arg_set_inst_ctx.
 * The hw read/write functions would then add the base address from the context before accessing the
 * hardware.
 *
 * @param[in]  cam_arg              Pointer to cam_arg.
 * @param[in]  inst_ctx             Pointer to the instance context.
 *
 */
void cam_arg_set_inst_ctx(cam_arg_t *cam_arg, void *inst_ctx);


/**
 * Sets the hw write function of cam_arg. The hw write function will be called by the
 * cam instance to write to the HW
 *
 * @param[in]  cam_arg              Pointer to cam_arg.
 * @param[in]  hw_write_function    Pointer to the hw write function. This function is provided by the user
 *
 */
void cam_arg_set_hw_write_function(cam_arg_t *cam_arg, void (*hw_write_function)(void*, uint32_t, uint32_t));


/**
 * Sets the hw read function of cam_arg. The hw read function will be called by the
 * cam instance to read from the HW
 *
 * @param[in]  cam_arg              Pointer to cam_arg.
 * @param[in]  hw_read_function     Pointer to the hw read function. This function is provided by the user
 *
 */
void cam_arg_set_hw_read_function(cam_arg_t *cam_arg, void (*hw_read_function)(void*, uint32_t, uint32_t*));


/**
 * Sets the supported number of entries to cam_arg. This is the number of entries the cam instance supports.
 *
 * @param[in]  cam_arg              Pointer to cam_arg.
 * @param[in]  num_entries          The number of supported entries the instance can store before it becomes full
 *
 */
void cam_arg_set_num_entries(cam_arg_t *cam_arg, uint32_t num_entries);


/**
* Determine if the CAM is either SW CAM or Computation CAM
*
* @param[in]  cam_arg              Pointer to cam_arg.
* @param[in]  hw_assist            If true COMP CAM hardware will be used. If false SW CAM is used.
*
*/
void cam_arg_set_hw_assist(struct cam_arg_t *cam_arg, bool hw_assist);

/**
 * Sets the lookup rate of the instance to cam_arg. This is the supported lookup rate of the instance.
 * In order to save resources it is important not to set the lookup rate higher than required.
 *
 * @param[in]  cam_arg              Pointer to cam_arg.
 * @param[in]  lookup_rate          The lookup rate of the instance expressed in Mlookups/s
 *
 */
void cam_arg_set_engine_lookup_rate(struct cam_arg_t *cam_arg, float lookup_rate);


/**
 * This function should not be used for bcam using HBM.
 * Sets the frequency of the lookup interface to cam_arg.
 * This is the clock frequency of the lookup request and response interfaces.
 * The frequency of the lookup interface must be at least as high as the lookup rate.
 * Sometimes a higher frequency is used, for example:
 * Lookup interface frequency == Lookup rate       (A lookup can be issued every clock cycle)
 * Lookup interface frequency == 2 * Lookup rate   (A lookup can be issued every second cycle)
 *
 * @param[in]  cam_arg              Pointer to cam_arg.
 * @param[in]  freq                 The frequency of the instance expressed in MHz
 *
 */
void cam_arg_set_lookup_interface_freq(struct cam_arg_t *cam_arg, float freq);


/**
 * This function should not be used for bcam using HBM.
 * Sets the RAM clock frequency of the instance to cam_arg.
 *
 * The RAM clock defines the clock frequency of the memories and the internal datapath.
 * A high RAM clock frequency enables time division of the hardware resources, leading to significant
 * savings.
 * The amount of time division possible is calculated as:
 * RAM clock frequency / lookup rate, rounded downwards to the nearest power of two.
 *
 * Example:
 * Lookup rate = 150, Lookup interface frequency = 330
 * RAM frequency = 600 -> Time division = 600 / 150 = 4
 *
 * @param[in]  cam_arg              Pointer to cam_arg.
 * @param[in]  freq                 The RAM clock frequency expressed in MHz.
 *
 */
void cam_arg_set_ram_freq(struct cam_arg_t *cam_arg, float freq);


/**
 * Sets the response width of the instance to cam_arg. This is the supported width of response.
 *
 * @param[in]  cam_arg              Pointer to cam_arg.
 * @param[in]  response_width       The response width expressed in number of bits (1-1024).
 *                                  The following limitations apply:
 *                                  For bcam,   key width + response width + 1 cannot exceed 1024/1536/1024 (hbm/bram/uram) bits.
 *                                  For stcam,  key width + response width + priority width + 1 cannot exceed 1536/2048 (bram/uram) bits.
 *                                  For tcam, 2*key width + response width + priority width + 1 cannot exceed 1472/1984 (bram/uram) bits.
 *
 */
void cam_arg_set_response_width(struct cam_arg_t *cam_arg, uint16_t response_width);


/**
 * Forces a narrower priority width of the instance to cam_arg. This function is not applicable
 * for the bcam instance. The default value is a priority width
 * corresponding to the number of entries. Each entry should have its own priority value. In some scenarios a
 * narrower priority width is sufficient.
 *
 * @param[in]  cam_arg              Pointer to cam_arg.
 * @param[in]  prio_width           The prio width is expressed in number of bits (0-32).
 *                                  The priority is used to determine the winning match when multiple matches occurs.
 *                                  To represent the priority value a certain priority width is required.
 *                                  The following limitations apply:
 *                                  For stcam,  key width + response width + priority width + 1 cannot exceed 1536/2048 (bram/uram) bits.
 *                                  For tcam, 2*key width + response width + priority width + 1 cannot exceed 1472/1984 (bram/uram) bits.
 *
 */
void cam_arg_force_prio_width(struct cam_arg_t *cam_arg, uint8_t prio_width);


/**
 * This function is only used for bcam and stcam. For tcam the format string should be used instead.
 *
 * @param[in]  cam_arg              Pointer to cam_arg.
 * @param[in]  key_width            The key width expressed in number of bits (10-992).
 *                                  The following limitations apply:
 *                                  For bcam,   key width + response width + 1 cannot exceed 1024/1536/1024 (hbm/bram/uram) bits.
 *                                  For stcam,  key width + response width + priority width + 1 cannot exceed 1536/2048 (bram/uram) bits.
 *
 */
void cam_arg_set_key_width(struct cam_arg_t *cam_arg, uint16_t key_width);


/**
 * This function is only used for stcam.
 * An stcam is dimensioned to support a specific number of unique masks and a specific number of entries. The dimensioning is based
 * on a worst case scenario. The stcam memory management algorithm allocates memory dynamically for each unique mask. All entries can use
 * the same mask or entries can use different masks. The memory dimensioning guarantees that the specified number of masks and
 * the specified number of entries will fit in the memory simultaneously.
 *
 * @param[in]  cam_arg              Pointer to cam_arg.
 * @param[in]  num_masks            The number of unique masks.
 *
 */
void cam_arg_set_num_masks(struct cam_arg_t *cam_arg, uint8_t num_masks);


/**
 * This function is not used for bcam using HBM.
 * Overrides the automatic memory dimensioning. A cam consists of a number of memory units. The automatic
 * memory dimensioning calculates the number of required memory units to store all entries and masks.
 * This function is used to override the automatic dimensioning and forces the number of memory units to a specified
 * value. This function can be used for optimization in certain cases when the automatic
 * dimensioning provides too much headroom.
 *
 * @param[in]  cam_arg              Pointer to cam_arg.
 * @param[in]  num_units            The number of memory units (1-256).
 *
 */
void cam_arg_force_num_units(struct cam_arg_t *cam_arg, uint16_t num_units);


/**
 * This function is not used for a bcam using HBM.
 * Forces the instance to use either URAM or BRAM. Based on the cam_arg parameters an optimal solution in terms of
 * hardware resources is calculated. This solution is an instance using either BRAM or URAM. In some scenarios the
 * usage of a certain memory type might be constrained and then the solution can be forced to use a certain memory type
 * even though it is not the optimal solution.
 *
 * @param[in]  cam_arg              Pointer to cam_arg.
 * @param[in]  uram                 If true uram is forced to be used. If false bram is forced to be used.
 *
 */
void cam_arg_force_mem_type(struct cam_arg_t *cam_arg, bool uram);


/**
 * This function is only used for a bcam using HBM. If this function is called, the bcam uses HBM instead of URAM/BRAM.
 *
 * @param[in]  cam_arg              Pointer to cam_arg.
 *
 */
void cam_arg_force_hbm(struct cam_arg_t *cam_arg);

/**
 * This function is only used for a bcam using HBM. The default size is set for a 4GB/stack, i.e. size = 512K
 * This function should only be called if the stack size is different from 4GB/stack.
 *
 * @param[in]  cam_arg              Pointer to cam_arg.
 * @param[in]  size                 4GB = 512K
 *
 */
void cam_arg_set_dram_size(struct cam_arg_t *cam_arg, uint32_t size);

/**
 * This function is only used if mixed mode is set, see cam_arg_set_mixed_mode.
 * The function sets the key width of the narrow entry. The key width of the
 * wide entry is specified using cam_arg_set_key_width.
 *
 * @param[in]  cam_arg                  Pointer to cam_arg.
 * @param[in]  narrow_key_width         The key width for the narrow entry
 *
 */
void cam_arg_set_narrow_key_width(struct cam_arg_t *cam_arg, uint16_t narrow_key_width);

/**
 * This function can only be used for a bcam using hbm. If mixed mode is set, wide and narrow entries are efficiently stored together
 * in the same memory. The wide entry is 1024 or 512 bits wide. The narrow entry is half the width of the wide entry.
 * key_width + response_width + 2 is rounded upwards to 512 or 1024 bits for the wide entry . narrow_key_width + response_width + 2 is
 * rounded upwards to 256 or 512 bits for the narrow entry. Both conditions must be fulfilled:
 * 1: key_width >= narrow_key_width and
 * 2: narrow entry width = wide entry width / 2 .
 * See cam_arg_set_narrow_key_width.
 *
 * @param[in]  cam_arg              Pointer to cam_arg.
 * @param[in]  mixed                True = Mixed mode is enabled
 *                                  False  = Mixed mode is disabled
 */
void cam_arg_set_mixed_mode(struct cam_arg_t *cam_arg, bool mixed);


/**
 * Sets the flow control configuration.
 *
 * @param[in]  cam_arg              Pointer to cam_arg.
 * @param[in]  flow_control         False = Flow control off (fixed lookup rate & latency)
 *                                  True  = Flow control on (variable lookup rate & latency)
 *
 */
void cam_arg_set_flow_control(struct cam_arg_t *cam_arg, bool flow_control);


/**
 * Sets the default response. When a key is looked up in the data plane and there is a no match,
 * the default response is output together with a no match indication. If the default response is not set
 * all bits are cleared as default. The width of the default response is specified by the
 * cam_arg_set_response_width function. cam_arg_set_response_width must be called prior to this function
 * otherwise the number of bits t copy from the input argument is unknown and zero bytes will be copied.
 *
 * @param[in]  cam_arg              Pointer to cam_arg.
 * @param[in]  default_response     The default response
 *
 */
void cam_arg_set_default_response(struct cam_arg_t *cam_arg, const uint8_t *default_response);


/**
 * This function is only used if mixed mode is set, see cam_arg_set_mixed_mode. Sets the narrow default response.
 * When a narrow key is looked up in the data plane and there is a no match,
 * the narrow default response is output together with a no match indication. If the narrow default response is not set
 * all bits are cleared as default. The width of the narrow default response is specified by the
 * cam_arg_set_response_width function. cam_arg_set_response_width must be called prior to this function
 * otherwise the number of bits to copy from the input argument is unknown and zero bytes will be copied.
 *
 * @param[in]  cam_arg                      Pointer to cam_arg.
 * @param[in]  narrow_default_response      The narrow default response
 *
 */
void cam_arg_set_narrow_default_response(struct cam_arg_t *cam_arg, const uint8_t *narrow_default_response);


/**
 * Sets the debug flags of cam_arg. The debug flags control how much information is displayed
 * for information, warning and error purposes.
 *
 * @param[in]  cam_arg              Pointer to cam_arg.
 * @param[in]  debug_flags          Bit mask value, every bit corresponds to a class of debug messages.
 *
 */
void cam_arg_set_debug_flags(cam_arg_t *cam_arg, uint32_t debug_flags);


/**
 * Get the debug flags of cam_arg. The debug flags control how much information is displayed
 * for information, warning and error purposes.
 *
 * @param[in]  cam_arg              Pointer to cam_arg.
 * @return     debug_flags          Bit mask value, every bit corresponds to a class of debug messages.
 *
 */
uint32_t cam_arg_get_debug_flags(cam_arg_t *cam_arg);


/**
 * Sets the error print function of cam_arg. The error print function will be called by the
 * cam instance to print error messages. The default error print function is printing to stderr.
 *
 * @param[in]  cam_arg              Pointer to cam_arg.
 * @param[in]  error_print_function Pointer to the error print function. This function is provided by the user
 *
 */
void cam_arg_set_error_print_function(struct cam_arg_t *cam_arg, void (*error_print_function)(void *, char*));


/**
 * Sets the info print function of cam_arg. The info print function will be called by the
 * cam instance to print informational messages. The default info print function is printing to stdout.
 *
 * @param[in]  cam_arg              Pointer to cam_arg.
 * @param[in]  info_print_function  Pointer to the info print function. This function is provided by the user
 *
 */
void cam_arg_set_info_print_function(struct cam_arg_t *cam_arg, void (*info_print_function)(void *, char*));


/**
 * This function is only for debug purposes and returns the error code as a string.
 *
 * @param[in]  error_code           error code to print. The error code is returned from the API functions
 * @return     error_string         The error code as text.
 *
 */
char *cam_error_string(int error_code);


/**
 * This function is only for debug purposes and prints the error code on stderr.
 *
 * @param[in]  error_code           error code to print. The error code is returned from the API functions
 *
 */
void cam_print_error(int error_code);
/** @}*/

#ifdef __cplusplus
}
#endif

#endif
