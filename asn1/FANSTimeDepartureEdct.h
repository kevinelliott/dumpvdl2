/*
 * Generated by asn1c-0.9.28 (http://lionet.info/asn1c)
 * From ASN.1 module "FANSACTwoWayDataLinkCommunications"
 * 	found in "fans-cpdlc.asn1"
 * 	`asn1c -fcompound-names -gen-PER`
 */

#ifndef	_FANSTimeDepartureEdct_H_
#define	_FANSTimeDepartureEdct_H_


#include <asn_application.h>

/* Including external dependencies */
#include "FANSTime.h"

#ifdef __cplusplus
extern "C" {
#endif

/* FANSTimeDepartureEdct */
typedef FANSTime_t	 FANSTimeDepartureEdct_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_FANSTimeDepartureEdct;
asn_struct_free_f FANSTimeDepartureEdct_free;
asn_struct_print_f FANSTimeDepartureEdct_print;
asn_constr_check_f FANSTimeDepartureEdct_constraint;
ber_type_decoder_f FANSTimeDepartureEdct_decode_ber;
der_type_encoder_f FANSTimeDepartureEdct_encode_der;
xer_type_decoder_f FANSTimeDepartureEdct_decode_xer;
xer_type_encoder_f FANSTimeDepartureEdct_encode_xer;
per_type_decoder_f FANSTimeDepartureEdct_decode_uper;
per_type_encoder_f FANSTimeDepartureEdct_encode_uper;

#ifdef __cplusplus
}
#endif

#endif	/* _FANSTimeDepartureEdct_H_ */
#include <asn_internal.h>