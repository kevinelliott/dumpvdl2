/*
 * Generated by asn1c-0.9.28 (http://lionet.info/asn1c)
 * From ASN.1 module "PMCPDLCMessageSetVersion1"
 * 	found in "atn-cpdlc.asn1"
 * 	`asn1c -fcompound-names -gen-PER`
 */

#ifndef	_BlockLevel_H_
#define	_BlockLevel_H_


#include <asn_application.h>

/* Including external dependencies */
#include <asn_SEQUENCE_OF.h>
#include <constr_SEQUENCE_OF.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Forward declarations */
struct LevelType;

/* BlockLevel */
typedef struct BlockLevel {
	A_SEQUENCE_OF(struct LevelType) list;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} BlockLevel_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_BlockLevel;

#ifdef __cplusplus
}
#endif

/* Referred external types */
#include "LevelType.h"

#endif	/* _BlockLevel_H_ */
#include <asn_internal.h>
