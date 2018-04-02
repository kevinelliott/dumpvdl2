/*
 * Generated by asn1c-0.9.28 (http://lionet.info/asn1c)
 * From ASN.1 module "PMCPDLCMessageSetVersion1"
 * 	found in "atn-cpdlc.asn1"
 * 	`asn1c -fcompound-names -gen-PER`
 */

#ifndef	_WaypointSpeedLevelSequence_H_
#define	_WaypointSpeedLevelSequence_H_


#include <asn_application.h>

/* Including external dependencies */
#include <asn_SEQUENCE_OF.h>
#include <constr_SEQUENCE_OF.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Forward declarations */
struct WaypointSpeedLevel;

/* WaypointSpeedLevelSequence */
typedef struct WaypointSpeedLevelSequence {
	A_SEQUENCE_OF(struct WaypointSpeedLevel) list;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} WaypointSpeedLevelSequence_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_WaypointSpeedLevelSequence;

#ifdef __cplusplus
}
#endif

/* Referred external types */
#include "WaypointSpeedLevel.h"

#endif	/* _WaypointSpeedLevelSequence_H_ */
#include <asn_internal.h>
