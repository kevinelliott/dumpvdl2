/*
 * Generated by asn1c-0.9.28 (http://lionet.info/asn1c)
 * From ASN.1 module "PMCPDLCMessageSetVersion1"
 * 	found in "atn-cpdlc.asn1"
 * 	`asn1c -fcompound-names -fincludes-quoted -gen-PER`
 */

#ifndef	_WaypointSpeedLevel_H_
#define	_WaypointSpeedLevel_H_


#include "asn_application.h"

/* Including external dependencies */
#include "Position.h"
#include "constr_SEQUENCE.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Forward declarations */
struct Speed;
struct ATWLevelSequence;

/* WaypointSpeedLevel */
typedef struct WaypointSpeedLevel {
	Position_t	 position;
	struct Speed	*speed	/* OPTIONAL */;
	struct ATWLevelSequence	*aTWLevels	/* OPTIONAL */;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} WaypointSpeedLevel_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_WaypointSpeedLevel;

#ifdef __cplusplus
}
#endif

/* Referred external types */
#include "Speed.h"
#include "ATWLevelSequence.h"

#endif	/* _WaypointSpeedLevel_H_ */
#include "asn_internal.h"
