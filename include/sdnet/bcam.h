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
 * Public API of the BCAM.
 *
 * @file bcam.h
 *
 *
 * @addtogroup bcam BCAM Public API
 * @{
 */

#ifndef _BCAM_H_
#define _BCAM_H_

#include "sdnet/cam.h"

#ifdef __cplusplus
extern "C" {
#endif


// Forward declaration of the bcam struct.
struct bcam;



/**
 * Creates a bcam instance. Returns 0 if successful, otherwise an error code. A shadow of the HW instance
 * is created.
 *
 * @param[in] cam_arg   Arguments used to create the bcam instance. cam_arg is copied to the
 *                      bcam instance, and can be destroyed immediately after this call.
 *
 * @param[out] out_bcam Returns the newly created bcam instance, memory is allocated using malloc.
 *                      If bcam_create returns an error the instance is not created.
 *                      In this case no memory is allocated, and out_bcam is left unchanged.
 *
 * @return              Returns 0 for success, otherwise the error code.
 */
int bcam_create(const cam_arg_t *cam_arg, struct bcam **out_bcam);



/**
 * Inserts an entry to the bcam instance. If the entry is found, the function fails by returning
 * error code CAM_ERROR_DUPLICATE_FOUND.
 * If the entry is not found, then it is inserted in the bcam instance.
 * Returns 0 if successful, otherwise an error code.
 *
 * @param[in] bcam      Pointer to the bcam instance
 *
 * @param[in] key       The key. The key width (in bits) is implicitly specified by the format string.
 *                      The key width specifies how many bits are copied and stored.
 *                      Any contents outside the scope of the defined key width is ignored.
 *                      The key is assumed to be in little-endian format.
 *                      key[0] bit 0 is the least significant bit of the key.
 *                      key[1] bit 0 is bit 8 of the key and so on.
 *
 * @param[in] response  The response of the entry.
 *                      The defined response width specifies how many bits are copied and stored.
 *                      The response is in little-endian format.
 *
 * @return              Returns 0 for success, otherwise the error code.
 *
 */
int bcam_insert(struct bcam *bcam, const uint8_t key[], const uint8_t response[]);



/**
 * Updates an entry in the bcam instance. If the entry is found, the response is updated.
 * If the entry is not found, the function fails with error code CAM_ERROR_KEY_NOT_FOUND.
 * Returns 0 if successful, otherwise an error code.
 *
 * @param[in] bcam      Pointer to the bcam instance
 *
 * @param[in] key       The key. The key width (in bits) is implicitly specified by the format string.
 *                      The key width specifies how many bits of the key to use.
 *                      Any contents outside the scope of the defined key width is ignored.
 *                      The key is assumed to be in little-endian format.
 *                      key[0] bit 0 is the least significant bit of the key.
 *                      key[1] bit 0 is bit 8 of the key and so on.
 *
 * @param[in] response  The response of the entry.
 *                      The defined response width specifies how many bits are copied and stored.
 *                      The response is in little-endian format.
 *
 * @return              Returns 0 for success, otherwise the error code.
 *
 */
int bcam_update(struct bcam *bcam, const uint8_t key[], const uint8_t response[]);



/**
 * Gets an entry with matching response value. If an entry is found, the function returns 0.
 * If the entry is not found, the function fails with error code CAM_ERROR_KEY_NOT_FOUND.
 *
 * The pos argument enables multiple matching entries to be found. The first time the function is
 * called pos should be set to zero. To find all entries matching the response, the function is
 * called multiple times, each call returns one matching entry.
 * For every consecutive call the returned pos value must be used again as input argument.
 * When CAM_ERROR_KEY_NOT_FOUND is returned no more entries matching the response could be found.
 *
 * The response_mask is used to limit the search to subfields of the response. If the complete,
 * exact response value is searched, then all response_mask bits should be set to one.
 * By setting the response_mask bits to all zeros, any response value will match.
 * This can be used for getting all entries without prior knowledge of the response values.
 *
 * The get function does not read from the hardware. It reads from the shadow.
 * bcam_get_by_response uses linear search.
 *
 * @param[in] bcam              Pointer to bcam instance
 *
 * @param[in] response          The response to search for.
 *                              The response is in little-endian format.
 *
 * @param[in] response_mask     The response and response_mask are ANDed and then
 *                              searched for. The stored response is also ANDed
 *                              with the response_mask before compare takes place.
 *                              The response_mask is in little-endian format.
 *
 * @param[in,out] pos           Position in the database. Used for
 *                              consecutive get operations, set to 0 for
 *                              first get.
 *
 * @param[out] out_key          The read key. Key width bits are copied to the key.
 *                              The key is in little-endian format.
 *
 * @return                      Returns 0 for success, otherwise the error code.
 *
 */
int bcam_get_by_response(struct bcam *bcam, const uint8_t response[], const uint8_t response_mask[], uint32_t *pos,
                         uint8_t out_key[]);



/**
 * For bcam, get_by_key and lookup have identical behavior. See bcam_lookup.
 */
int bcam_get_by_key(struct bcam *bcam, const uint8_t key[], uint8_t response[]);



/**
 * Delete an entry from the bcam instance. If the entry is found, the function returns 0.
 * If an entry is not found, the function fails with error code CAM_ERROR_KEY_NOT_FOUND.
 *
 * @param[in] bcam      Pointer to bcam instance
 *
 * @param[in] key       The key to delete. The key width (in bits) is implicitly specified by the
 *                      format string. The key width specifies how many bits are copied and stored.
 *                      Any contents outside the scope of the defined key width is ignored.
 *                      The key is assumed to be in little-endian format.
 *                      key[0] bit 0 is the least significant bit of the key.
 *                      key[1] bit 0 is bit 8 of the key and so on.
 *
 * @return              Returns 0 for success, otherwise the error code.
 *
 */
int bcam_delete(struct bcam *bcam, const uint8_t key[]);



/**
 * Looks up an entry in the bcam instance.
 * This function provides the same response as if a lookup had been performed in hardware.
 * If no match is found in the database, the response value contains the default response
 * is returned and the return code is CAM_ERROR_KEY_NOT_FOUND.
 * If an entry is found in the data base the function returns 0.
 *
 * @param[in] bcam      Pointer to bcam instance
 *
 * @param[in] key       The key to lookup. The key width (in bits) is implicitly specified by the
 *                      key width. Any contents outside the scope of the defined key width is ignored.
 *                      The key is assumed to be in little-endian format.
 *                      key[0] bit 0 is the least significant bit of the key.
 *                      key[1] bit 0 is bit 8 of the key and so on.
 *
 * @param[out] response The response from the lookup.
 *
 * @return              Returns 0 for success, otherwise the error code.
 *
 */
int bcam_lookup(struct bcam *bcam, const uint8_t key[], uint8_t response[]);



/**
 * Reads and clears the ECC counters of the bcam instance. When the instance is created the ECC counters
 * are automatically cleared. This function is not supported for HBM.
 *
 * @param[in] bcam                          Pointer to bcam instance
 *
 * @param[out] corrected_single_bit_errors  The number of single bit errors the hardware scrubbing
 *                                          process has detected and corrected permanently . Single bit errors
 *                                          might have been detected by hardware lookups and then corrected
 *                                          dynamically for the lookups. These errors are not counted.
 *                                          The scrubbing process runs in the background and corrects
 *                                          errors permanently. The corrected_single_bit_errors counter
 *                                          is only incremented after the error is corrected permanently. The
 *                                          counter is cleared after it is read.
 *
 * @param[out] detected_double_bit_errors   The number of detected double-bit errors. Double-bit
 *                                          errors are fatal errors and the database is corrupt. The hardware
 *                                          cannot correct the error without help from software. The instance
 *                                          generates an interrupt every time a double bit error is detected
 *                                          either by a lookup or scrubbing. Lookups detecting double-bit errors
 *                                          signals double bit error and it is advised to discard such lookups.
 *                                          The detected_double_bit_errors counter is only incremented by the
 *                                          scrubbing process. The counter is cleared after it is read.
 *
 * @return                                  Returns 0 for success, otherwise the error code.
 *
 */
int bcam_read_and_clear_ecc_counters(struct bcam *bcam, uint32_t *corrected_single_bit_errors,
                                     uint32_t *detected_double_bit_errors);



/**
 * Reads and clears the ECC address registers of the bcam instance. The ECC address registers are
 * automatically cleared when the bcam instance is created.
 *
 * @param[in] bcam                               Pointer to bcam instance
 *
 * @param[out] failing_address_single_bit_error  The address of the first single bit error detected and corrected
 *                                               by the hardware scrubbing process since the last read of this address. Additional
 *                                               errors might have been detected during a hardware lookup and then corrected
 *                                               dynamically, but this register only reflects the
 *                                               errors detected by hardware scrubbing. Single bit errors are not reported for HBM.
 *
 * @param[out] failing_address_double_bit_error  The address of the first double-bit error detected by the
 *                                               hardware scrubbing process since the last read of this address.
 *
 * @return                                       Returns 0 for success, otherwise the error code.
 *
 */
int bcam_read_and_clear_ecc_addresses(struct bcam *bcam, uint32_t *failing_address_single_bit_error,
                                      uint32_t *failing_address_double_bit_error);



/**
 * Sets the debug flag. The debug flags control how much information is displayed
 * for information, warning and error purposes.
 *
 * @param[in] bcam          Pointer to bcam instance
 * @param[in] debug_flags   Bit mask value, every bit corresponds to a class of debug messages.
 *
 * @return                  Returns 0 for success, otherwise the error code.
 *
 */
int bcam_set_debug_flags(struct bcam *bcam, uint32_t debug_flags);



/**
 * Get the debug flags. The debug flags control how much information is displayed
 * for information, warning and error purposes.
 *
 * @param[in] bcam          Pointer to bcam instance
 * @return    debug_flags   Bit mask value, every bit corresponds to a class of debug messages.
 *
 */
uint32_t bcam_get_debug_flags(const struct bcam *bcam);



/**
 * Sets the option flags. The option flags control the behavior of the API.
 * This function is not supported for all modes. If not supported
 * CAM_ERROR_UNSUPPORTED_COMMAND is returned.
 *
 * @param[in]   bcam            Pointer to bcam instance
 * @param[in]   option_flags    Bit mask value, every bit corresponds to an option.
 *                              Multiple options can be enabled simultaneously.
 *
 * @return                      Returns 0 for success, otherwise the error code.
 *
 */
int bcam_set_option_flags(struct bcam *bcam, uint32_t option_flags);



/**
 * Gets the option flags. The option flags control the behavior of the API.
 * This function is not supported for all modes. If not supported
 * CAM_ERROR_UNSUPPORTED_COMMAND is returned.
 *
 * @param[in]   bcam              Pointer to bcam instance
 * @param[out]  option_flags      Bit mask value, every bit corresponds to an option.
 *                                Multiple options can be enabled simultaneously.
 *
 * @return                        Returns 0 for success, otherwise the error code.
 *
 */
int bcam_get_option_flags(const struct bcam *bcam, uint32_t *option_flags);



/**
 * Destroy bcam instance. All memory allocated by bcam_create is released.
 *
 * @param[in]   bcam    Pointer to bcam instance
 */
int bcam_destroy(struct bcam *bcam);



/**
 * Rewrite all memory contents to recover from ECC double-bit error.
 *
 * @param[in]   bcam    Pointer to bcam instance.
 */
int bcam_rewrite(const struct bcam *bcam);
/** @}*/

#ifdef __cplusplus
}
#endif

#endif
