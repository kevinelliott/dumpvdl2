/*
 *  dumpvdl2 - a VDL Mode 2 message decoder and protocol analyzer
 *
 *  Copyright (c) 2017-2019 Tomasz Lemiech <szpajder@gmail.com>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <signal.h>
#include <errno.h>
#include <libacars/libacars.h>	// LA_VERSION
#include <pthread.h>
#include "config.h"
#ifndef HAVE_PTHREAD_BARRIERS
#include "pthread_barrier.h"
#endif
#ifdef WITH_RTLSDR
#include "rtl.h"
#endif
#ifdef WITH_MIRISDR
#include "mirics.h"
#endif
#ifdef WITH_SDRPLAY
#include "sdrplay.h"
#endif
#ifdef WITH_SOAPYSDR
#include "soapysdr.h"
#endif
#include "dumpvdl2.h"
#include "avlc.h"		// parse_avlc_frames

int do_exit = 0;
uint32_t msg_filter = MSGFLT_ALL;
pthread_barrier_t demods_ready, samples_ready;
pthread_t decoder_thread;

void sighandler(int sig) {
	fprintf(stderr, "Got signal %d, exiting\n", sig);
	do_exit = 1;
#ifdef WITH_RTLSDR
	rtl_cancel();
#endif
#ifdef WITH_MIRISDR
	mirisdr_cancel();
#endif
#ifdef WITH_SDRPLAY
	sdrplay_cancel();
#endif
#ifdef WITH_SOAPYSDR
	soapysdr_cancel();
#endif
}

static void setup_signals() {
	struct sigaction sigact, pipeact;

	memset(&sigact, 0, sizeof(sigact));
	memset(&pipeact, 0, sizeof(pipeact));
	pipeact.sa_handler = SIG_IGN;
	sigact.sa_handler = &sighandler;
	sigaction(SIGPIPE, &pipeact, NULL);
	sigaction(SIGHUP, &sigact, NULL);
	sigaction(SIGINT, &sigact, NULL);
	sigaction(SIGQUIT, &sigact, NULL);
	sigaction(SIGTERM, &sigact, NULL);
}

static void setup_threads(vdl2_state_t *ctx) {
	int ret;
	if(	(ret = pthread_barrier_init(&demods_ready, NULL, ctx->num_channels+1)) != 0 ||
		(ret = pthread_barrier_init(&samples_ready, NULL, ctx->num_channels+1)) != 0) {
			errno = ret;
			perror("pthread_barrier_init failed");
			_exit(2);
	}
	if((ret = pthread_create(&decoder_thread, NULL, &parse_avlc_frames, NULL) != 0)) {
		errno = ret;
		perror("pthread_create failed");
		_exit(2);
	}
	for(int i = 0; i < ctx->num_channels; i++) {
		if((ret = pthread_create(&ctx->channels[i]->demod_thread, NULL, &process_samples, ctx->channels[i])) != 0) {
			errno = ret;
			perror("pthread_create failed");
			_exit(2);
		}
	}
}

static uint32_t calc_centerfreq(uint32_t *freq, int cnt, uint32_t source_rate) {
	uint32_t freq_min, freq_max;
	freq_min = freq_max = freq[0];
	for(int i = 0; i < cnt; i++) {
		if(freq[i] < freq_min) freq_min = freq[i];
		if(freq[i] > freq_max) freq_max = freq[i];
	}
	if(freq_max - freq_min > source_rate * 0.8f) {
		fprintf(stderr, "Error: given frequencies are too fart apart\n");
		return 0;
	}
	return freq_min + (freq_max - freq_min) / 2;
}

void process_file(vdl2_state_t *ctx, char *path, enum sample_formats sfmt) {
// -Wunused-parameter
	(void)ctx;
	FILE *f;
	uint32_t len;
	unsigned char buf[FILE_BUFSIZE];
	void (*process_buf)() = NULL;

	if((f = fopen(path, "r")) == NULL) {
		perror("fopen()");
		_exit(2);
	}
	switch(sfmt) {
	case SFMT_U8:
		process_buf_uchar_init();
		sbuf = XCALLOC(FILE_BUFSIZE / sizeof(uint8_t), sizeof(float));
		process_buf = &process_buf_uchar;
		break;
	case SFMT_S16_LE:
		sbuf = XCALLOC(FILE_BUFSIZE / sizeof(int16_t), sizeof(float));
		process_buf = &process_buf_short;
		break;
	default:
		fprintf(stderr, "Unsupported sample format\n");
		_exit(5);
	}
	do {
		len = fread(buf, 1, FILE_BUFSIZE, f);
		(*process_buf)(buf, len, NULL);
	} while(len == FILE_BUFSIZE && !do_exit);
	fclose(f);
}

void usage() {
	fprintf(stderr, "Usage:\n\n");
#ifdef WITH_RTLSDR
	fprintf(stderr, "RTL-SDR receiver:\n");
	fprintf(stderr, "\tdumpvdl2 [output_options] --rtlsdr <device_id> [rtlsdr_options] [<freq_1> [freq_2 [...]]]\n");
#endif
#ifdef WITH_MIRISDR
	fprintf(stderr, "MIRI-SDR receiver:\n");
	fprintf(stderr, "\tdumpvdl2 [output_options] --mirisdr <device_id> [mirisdr_options] [<freq_1> [freq_2 [...]]]\n");
#endif
#ifdef WITH_SDRPLAY
	fprintf(stderr, "SDRPLAY RSP receiver:\n");
	fprintf(stderr, "\tdumpvdl2 [output_options] --sdrplay <device_id> [sdrplay_options] [<freq_1> [freq_2 [...]]]\n");
#endif
#ifdef WITH_SOAPYSDR
	fprintf(stderr, "SOAPYSDR compatible receiver:\n");
	fprintf(stderr, "\tdumpvdl2 [output_options] --soapysdr <device_id> [soapysdr_options] [<freq_1> [freq_2 [...]]]\n");
#endif
	fprintf(stderr, "I/Q input from file:\n");
	fprintf(stderr, "\tdumpvdl2 [output_options] --iq-file <input_file> [file_options] [<freq_1> [freq_2 [...]]]\n");
	fprintf(stderr, "\ncommon options:\n");
	fprintf(stderr, "\t<freq_1> [freq_2 [...]]\t\tVDL2 channel frequences, in Hz (max %d simultaneous channels supported).\n", MAX_CHANNELS);
	fprintf(stderr, "\t\t\t\t\tIf omitted, will use VDL2 Common Signalling Channel (%u Hz)\n", CSC_FREQ);
	fprintf(stderr, "\noutput_options:\n");
	fprintf(stderr, "\t--output-file <output_file>\tOutput decoded frames to <output_file> (default: stdout)\n");
	fprintf(stderr, "\t--output-json <output_file>\tOutput JSON frames to <output_file> (default: stdout)\n");
	fprintf(stderr, "\t--hourly\t\t\tRotate output file hourly\n");
	fprintf(stderr, "\t--daily\t\t\t\tRotate output file daily\n");
	fprintf(stderr, "\t--utc\t\t\t\tUse UTC timestamps in output and file names\n");
	fprintf(stderr, "\t--raw-frames\t\t\tOutput AVLC payload as raw bytes\n");
	fprintf(stderr, "\t--dump-asn1\t\t\tOutput full ASN.1 structure of CM and CPDLC messages\n");
	fprintf(stderr, "\t--extended-header\t\tOutput additional fields in message header\n");
	fprintf(stderr, "\t--msg-filter <filter_spec>\tMessage types to display (default: all) (\"--msg-filter help\" for details)\n");
	fprintf(stderr, "\t--output-acars-pp <host:port>\tSend ACARS messages to Planeplotter over UDP/IP\n");
#ifdef WITH_STATSD
	fprintf(stderr, "\t--statsd <host>:<port>\t\tSend statistics to Etsy StatsD server <host>:<port> (default: disabled)\n");
#endif
#ifdef WITH_RTLSDR
	fprintf(stderr, "\nrtlsdr_options:\n");
	fprintf(stderr, "\t--rtlsdr <device_id>\t\tUse RTL device with specified ID or serial number (default: ID=0)\n");
	fprintf(stderr, "\t--gain <gain>\t\t\tSet gain (decibels)\n");
	fprintf(stderr, "\t--correction <correction>\tSet freq correction (ppm)\n");
	fprintf(stderr, "\t--centerfreq <center_frequency>\tSet center frequency in Hz (default: auto)\n");
#endif
#ifdef WITH_MIRISDR
	fprintf(stderr, "\nmirisdr_options:\n");
	fprintf(stderr, "\t--mirisdr <device_id>\t\tUse Mirics device with specified ID or serial number (default: ID=0)\n");
	fprintf(stderr, "\t--hw-type <device_type>\t\t0 - default, 1 - SDRPlay\n");
	fprintf(stderr, "\t--gain <gain>\t\t\tSet gain (in decibels, from 0 to 102 dB)\n");
	fprintf(stderr, "\t--correction <correction>\tSet freq correction (in Hertz)\n");
	fprintf(stderr, "\t--centerfreq <center_frequency>\tSet center frequency in Hz (default: auto)\n");
	fprintf(stderr, "\t--usb-mode <usb_transfer_mode>\t0 - isochronous (default), 1 - bulk\n");
#endif
#ifdef WITH_SDRPLAY
	fprintf(stderr, "\nsdrplay_options:\n");
	fprintf(stderr, "\t--sdrplay <device_id>\t\tUse SDRPlay RSP device with specified ID or serial number (default: ID=0)\n");
	fprintf(stderr, "\t--gr <gr>\t\t\tSet system gain reduction, in dB, positive (-100 = enables AGC)\n");
	fprintf(stderr, "\t--agc <set point in dBFS>\tAutomatic Gain Control set point in dBFS, negative (default: 0 = AGC off)\n");
	fprintf(stderr, "\t--correction <correction>\tSet freq correction (ppm)\n");
	fprintf(stderr, "\t--centerfreq <center_frequency>\tSet center frequency in Hz (default: auto)\n");
	fprintf(stderr, "\t--antenna <A/B>\t\t\tRSP2 antenna port selection (default: A)\n");
	fprintf(stderr, "\t--biast <0/1>\t\t\tRSP2 Bias-T control: 0 - off (default), 1 - on\n");
	fprintf(stderr, "\t--notch-filter <0/1>\t\tRSP2/1a/duo AM/FM/bcast notch filter control: 0 - off (default), 1 - on\n");
	fprintf(stderr, "\t--tuner <1/2>\t\t\tRSPduo tuner selection: (default: 1)\n");
#endif
#ifdef WITH_SOAPYSDR
	fprintf(stderr, "\nsoapysdr_options:\n");
	fprintf(stderr, "\t--soapysdr <device_id>\t\tUse SoapySDR compatible device with specified ID (default: ID=0)\n");
	fprintf(stderr, "\t--device-settings <key1=val1,key2=val2,...>\tSet device-specific parameters (default: none)\n");
	fprintf(stderr, "\t--gain <gain>\t\t\tSet gain (decibels)\n");
	fprintf(stderr, "\t--correction <correction>\tSet freq correction (ppm)\n");
	fprintf(stderr, "\t--soapy-antenna <antenna>\tSet antenna port selection (default: RX)\n");
	fprintf(stderr, "\t--soapy-gain <gain1=val1,gain2=val2,...>\tSet gain components (default: none)\n");
#endif
	fprintf(stderr, "\nfile_options:\n");
	fprintf(stderr, "\t--iq-file <input_file>\t\tRead I/Q samples from file\n");
	fprintf(stderr, "\t--centerfreq <center_frequency>\tCenter frequency of the input data, in Hz (default: 0)\n");
	fprintf(stderr, "\t--oversample <oversample_rate>\tOversampling rate for recorded data (default: %u)\n", FILE_OVERSAMPLE);
	fprintf(stderr, "\t\t\t\t\t  (sampling rate will be set to %u * oversample_rate)\n", SYMBOL_RATE * SPS);
	fprintf(stderr, "\t--sample-format <sample_format>\tInput sample format. Supported formats:\n");
	fprintf(stderr, "\t\t\t\t\t  U8\t\t8-bit unsigned (eg. recorded with rtl_sdr) (default)\n");
	fprintf(stderr, "\t\t\t\t\t  S16_LE\t16-bit signed, little-endian (eg. recorded with miri_sdr)\n");
	_exit(0);
}

const msg_filterspec_t filters[] = {
	{ "all",		MSGFLT_ALL,			"All messages" },
	{ "uplink",		MSGFLT_SRC_GND,			"Uplink messages (sourced by ground stations)" },
	{ "downlink",		MSGFLT_SRC_AIR,			"Downlink messages (sourced by aircraft)" },
	{ "avlc_s",		MSGFLT_AVLC_S,			"AVLC Supervisory frames" },
	{ "avlc_u",		MSGFLT_AVLC_U,			"AVLC Unnumbered Control frames" },
	{ "avlc_i",		MSGFLT_AVLC_I,			"AVLC Information frames" },
	{ "avlc",		MSGFLT_AVLC_S | MSGFLT_AVLC_U | MSGFLT_AVLC_I, "All AVLC frames (shorthand for \"avlc_s,avlc_u,avlc_i)\"", },
	{ "acars_nodata",	MSGFLT_ACARS_NODATA,		"ACARS frames without data (eg. empty ACKs)" },
	{ "acars_data",		MSGFLT_ACARS_DATA,		"ACARS frames with data" },
	{ "acars",		MSGFLT_ACARS_NODATA | MSGFLT_ACARS_DATA, "All ACARS frames (shorthand for \"acars_nodata,acars_data\")" },
	{ "xid_no_gsif",	MSGFLT_XID_NO_GSIF,		"XID frames other than Ground Station Information Frames" },
	{ "gsif",		MSGFLT_XID_GSIF,		"Ground Station Information Frames" },
	{ "xid",		MSGFLT_XID_NO_GSIF | MSGFLT_XID_GSIF, "All XID frames (shorthand for \"xid_no_gsif,gsif\")" },
	{ "x25_control",	MSGFLT_X25_CONTROL,		"X.25 Control packets" },
	{ "x25_data",		MSGFLT_X25_DATA,		"X.25 Data packets" },
	{ "x25",		MSGFLT_X25_CONTROL | MSGFLT_X25_DATA, "All X.25 packets (shorthand for \"x25_control,x25_data\")" },
	{ "idrp_no_keepalive",	MSGFLT_IDRP_NO_KEEPALIVE,	"IDRP PDUs other than Keepalives" },
	{ "idrp_keepalive",	MSGFLT_IDRP_KEEPALIVE,		"IDRP Keepalive PDUs" },
	{ "idrp",		MSGFLT_IDRP_NO_KEEPALIVE | MSGFLT_IDRP_KEEPALIVE, "All IDRP PDUs (shorthand for \"idrp_no_keepalive,idrp_keepalive\")" },
	{ "esis",		MSGFLT_ESIS,			"ES-IS PDUs" },
	{ "cm",			MSGFLT_CM,			"ICAO Context Management Protocol PDUs" },
	{ "cpdlc",		MSGFLT_CPDLC,			"Controller-Pilot Data Link Communication PDUs" },
	{ "adsc",		MSGFLT_ADSC,			"Automatic Dependent Surveillance - Contract messages" },
	{ 0,			0,				0 }
};

static void msg_filter_usage() {
	fprintf(stderr, "<filter_spec> is a comma-separated list of words specifying message types which should\n");
	fprintf(stderr, "be displayed. Each word may optionally be preceded by a '-' sign to negate its meaning\n");
	fprintf(stderr, "(ie. to indicate that a particular message type shall not be displayed).\n");
	fprintf(stderr, "\nSupported message types:\n\n");

	msg_filterspec_t *ptr = (msg_filterspec_t *)filters;
	while(ptr->token != NULL) {
		fprintf(stderr, "\t%s\t%s%s%s\n",
			ptr->token,
			strlen(ptr->token) < 8 ? "\t" : "",
			strlen(ptr->token) < 16 ? "\t" : "",
			ptr->description
		);
		ptr++;
	}

	fprintf(stderr, "\nWhen --msg-filter option is not used, all messages are displayed. But when it is, the\n");
	fprintf(stderr, "filter is first reset to \"none\", ie. you have to explicitly enable all message types\n");
	fprintf(stderr, "which you wish to see. Word list is parsed from left to right, so the last match wins.\n");
	fprintf(stderr, "\nRefer to FILTERING_EXAMPLES.md file for usage examples.\n");
	_exit(0);
}

static void update_filtermask(char *token, uint32_t *fmask) {
	msg_filterspec_t *ptr = (msg_filterspec_t *)filters;
	int negate = 0;
	if(token[0] == '-') {
		negate = 1;
		token++;
		if(token[0] == '\0') {
			fprintf(stderr, "Invalid filtermask: no token after '-'\n");
			_exit(1);
		}
	}
	while(ptr->token != NULL) {
		if(!strcmp(token, ptr->token)) {
			if(negate)
				*fmask &= ~ptr->value;
			else
				*fmask |= ptr->value;
			debug_print("token: %s negate: %d filtermask: 0x%x\n", token, negate, *fmask);
			return;
		}
		ptr++;
	}
	fprintf(stderr, "Invalid filter specification: %s: unknown message type\n", token);
	_exit(1);
}

static uint32_t parse_msg_filterspec(char *filterspec) {
	if(!strcmp(filterspec, "help")) {
		msg_filter_usage();
		return 0;
	}
	uint32_t fmask = MSGFLT_NONE;
	char *token = strtok(filterspec, ",");
	if(token == NULL) {
		fprintf(stderr, "Invalid filter specification\n");
		_exit(1);
	}
	update_filtermask(token, &fmask);
	while((token = strtok(NULL, ",")) != NULL)
		update_filtermask(token, &fmask);
	return fmask;
}

int main(int argc, char **argv) {
	vdl2_state_t ctx;
	uint32_t centerfreq = 0, sample_rate = 0, oversample = 0;
	uint32_t *freqs;
	int num_channels = 0;
	enum input_types input = INPUT_UNDEF;
	enum sample_formats sample_fmt = SFMT_UNDEF;
#if defined WITH_RTLSDR || defined WITH_MIRISDR || defined WITH_SDRPLAY || defined WITH_SOAPYSDR
	char *device = NULL;
	float gain = SDR_AUTO_GAIN;
	int correction = 0;
#endif
#ifdef WITH_MIRISDR
	int mirisdr_hw_flavour = 0;
	int mirisdr_usb_xfer_mode = 0;
#endif
#ifdef WITH_SDRPLAY
	char* sdrplay_antenna = "A";
	int sdrplay_biast = 0;
	int sdrplay_notch_filter = 0;
	int sdrplay_tuner = 1;
	int sdrplay_agc = 0;
	int sdrplay_gr = SDR_AUTO_GAIN;
#endif
#ifdef WITH_SOAPYSDR
	char *soapysdr_settings = NULL;
	char *soapysdr_antenna = NULL;
	char *soapysdr_gain = NULL;
#endif
	int opt;
	struct option long_opts[] = {
		{ "centerfreq",		required_argument,	NULL,	__OPT_CENTERFREQ },
		{ "daily",		no_argument,		NULL,	__OPT_DAILY },
		{ "hourly",		no_argument,		NULL, 	__OPT_HOURLY },
		{ "utc",		no_argument,		NULL,	__OPT_UTC },
		{ "raw-frames",		no_argument,		NULL,	__OPT_RAW_FRAMES },
		{ "dump-asn1",		no_argument,		NULL,	__OPT_DUMP_ASN1 },
		{ "extended-header",	no_argument,		NULL,	__OPT_EXTENDED_HEADER },
		{ "output-file",	required_argument,	NULL,	__OPT_OUTPUT_FILE },
		{ "output-format-json",	required_argument,	NULL,	__OPT_OUTPUT_FORMAT_JSON },
		{ "iq-file",		required_argument,	NULL,	__OPT_IQ_FILE },
		{ "oversample",		required_argument,	NULL,	__OPT_OVERSAMPLE },
		{ "sample-format",	required_argument,	NULL,	__OPT_SAMPLE_FORMAT },
		{ "msg-filter",		required_argument,	NULL,	__OPT_MSG_FILTER },
		{ "output-acars-pp",	required_argument,	NULL,	__OPT_OUTPUT_ACARS_PP },
#ifdef WITH_MIRISDR
		{ "mirisdr",		required_argument,	NULL,	__OPT_MIRISDR },
		{ "hw-type",		required_argument,	NULL,	__OPT_HW_TYPE },
		{ "usb-mode",		required_argument,	NULL,	__OPT_USB_MODE },
#endif
#ifdef WITH_SDRPLAY
		{ "sdrplay",		required_argument,	NULL,	__OPT_SDRPLAY },
		{ "antenna",		required_argument,	NULL,	__OPT_ANTENNA },
		{ "biast",		required_argument,	NULL,	__OPT_BIAST },
		{ "notch-filter",	required_argument,	NULL,	__OPT_NOTCH_FILTER },
		{ "agc",		required_argument,	NULL,	__OPT_AGC },
		{ "gr",			required_argument,	NULL,	__OPT_GR },
		{ "tuner",		required_argument,	NULL,	__OPT_TUNER },
#endif
#ifdef WITH_SOAPYSDR
		{ "soapysdr",		required_argument,	NULL,	__OPT_SOAPYSDR },
		{ "device-settings",	required_argument,	NULL,	__OPT_DEVICE_SETTINGS },
		{ "soapy-antenna",	required_argument,	NULL,	__OPT_SOAPY_ANTENNA },
		{ "soapy-gain",		required_argument,	NULL,	__OPT_SOAPY_GAIN },
#endif
#ifdef WITH_RTLSDR
		{ "rtlsdr",		required_argument,	NULL,	__OPT_RTLSDR },
#endif
#if defined WITH_RTLSDR || defined WITH_MIRISDR || defined WITH_SOAPYSDR
		{ "gain",		required_argument,	NULL,	__OPT_GAIN },
#endif
#if defined WITH_RTLSDR || defined WITH_MIRISDR || defined WITH_SDRPLAY || defined WITH_SOAPYSDR
		{ "correction",		required_argument,	NULL,	__OPT_CORRECTION },
#endif
#ifdef WITH_STATSD
		{ "statsd",		required_argument,	NULL,	__OPT_STATSD },
#endif
		{ "help",		no_argument,		NULL,	__OPT_HELP },
		{ 0,			0,			0,	0 }
	};

#ifdef WITH_STATSD
	char *statsd_addr = NULL;
	int statsd_enabled = 0;
#endif
	char *infile = NULL, *outfile = NULL, *pp_addr = NULL;
	int output_format_json = 0;

	fprintf(stderr, "dumpvdl2 %s (libacars %s)\n", DUMPVDL2_VERSION, LA_VERSION);
	while((opt = getopt_long(argc, argv, "", long_opts, NULL)) != -1) {
		switch(opt) {
		case __OPT_IQ_FILE:
			infile = strdup(optarg);
			input = INPUT_FILE;
			oversample = FILE_OVERSAMPLE;
			sample_fmt = SFMT_U8;
			break;
		case __OPT_SAMPLE_FORMAT:
			if(!strcmp(optarg, "U8"))
				sample_fmt = SFMT_U8;
			else if(!strcmp(optarg, "S16_LE"))
				sample_fmt = SFMT_S16_LE;
			else {
				fprintf(stderr, "Unknown sample format\n");
				_exit(1);
			}
			break;
		case __OPT_HOURLY:
			hourly = 1;
			break;
		case __OPT_DAILY:
			daily = 1;
			break;
		case __OPT_UTC:
			utc = 1;
			break;
		case __OPT_RAW_FRAMES:
			output_raw_frames = 1;
			break;
		case __OPT_DUMP_ASN1:
			dump_asn1 = 1;
			break;
		case __OPT_EXTENDED_HEADER:
			extended_header = 1;
			break;
		case __OPT_CENTERFREQ:
			centerfreq = strtoul(optarg, NULL, 10);
			break;
#ifdef WITH_MIRISDR
		case __OPT_MIRISDR:
			device = optarg;
			input = INPUT_MIRISDR;
			oversample = MIRISDR_OVERSAMPLE;
			break;
		case __OPT_HW_TYPE:
			mirisdr_hw_flavour = atoi(optarg);
			break;
		case __OPT_USB_MODE:
			mirisdr_usb_xfer_mode = atoi(optarg);
			break;
#endif
#ifdef WITH_SDRPLAY
		case __OPT_SDRPLAY:
			device = optarg;
			input = INPUT_SDRPLAY;
			oversample = SDRPLAY_OVERSAMPLE;
			break;
		case __OPT_ANTENNA:
			sdrplay_antenna = strdup(optarg);
			break;
		case __OPT_BIAST:
			sdrplay_biast = atoi(optarg);
			break;
		case __OPT_NOTCH_FILTER:
			sdrplay_notch_filter = atoi(optarg);
			break;
		case __OPT_AGC:
			sdrplay_agc = atoi(optarg);
			break;
		case __OPT_GR:
			sdrplay_gr = atoi(optarg);
			break;
		case __OPT_TUNER:
			sdrplay_tuner = atoi(optarg);
			break;
#endif
#ifdef WITH_SOAPYSDR
		case __OPT_SOAPYSDR:
			device = optarg;
			input = INPUT_SOAPYSDR;
			oversample = SOAPYSDR_OVERSAMPLE;
			break;
		case __OPT_DEVICE_SETTINGS:
			soapysdr_settings = strdup(optarg);
			break;
		case __OPT_SOAPY_ANTENNA:
			soapysdr_antenna = strdup(optarg);
			break;
		case __OPT_SOAPY_GAIN:
			soapysdr_gain = strdup(optarg);
			break;
#endif
#ifdef WITH_RTLSDR
		case __OPT_RTLSDR:
			device = optarg;
			input = INPUT_RTLSDR;
			oversample = RTL_OVERSAMPLE;
			break;
#endif
#if defined WITH_RTLSDR || defined WITH_MIRISDR || defined WITH_SOAPYSDR
		case __OPT_GAIN:
			gain = atof(optarg);
			break;
#endif
#if defined WITH_RTLSDR || defined WITH_MIRISDR || defined WITH_SDRPLAY || defined WITH_SOAPYSDR
		case __OPT_CORRECTION:
			correction = atoi(optarg);
			break;
#endif
		case __OPT_OUTPUT_FILE:
			outfile = strdup(optarg);
			break;
		case __OPT_OUTPUT_FORMAT_JSON:
			output_format_json = 1;
			break;
		case __OPT_OVERSAMPLE:
			oversample = atoi(optarg);
			break;
#ifdef WITH_STATSD
		case __OPT_STATSD:
			statsd_addr = strdup(optarg);
			statsd_enabled = 1;
			break;
#endif
		case __OPT_OUTPUT_ACARS_PP:
			pp_addr = strdup(optarg);
			break;
		case __OPT_MSG_FILTER:
			msg_filter = parse_msg_filterspec(optarg);
			break;
		case __OPT_HELP:
		default:
			usage();
		}
	}
	if(input == INPUT_UNDEF)
		usage();

	if(optind < argc) {
		num_channels = argc - optind;
		if(num_channels > MAX_CHANNELS) {
			fprintf(stderr, "Error: too many channels specified (%d > %d)\n", num_channels, MAX_CHANNELS);
			_exit(1);
		}
		freqs = XCALLOC(num_channels, sizeof(uint32_t));
		for(int i = 0; i < num_channels; i++)
			freqs[i] = strtoul(argv[optind+i], NULL, 10);
	} else {
		fprintf(stderr, "Warning: frequency not set - using VDL2 Common Signalling Channel as a default (%u Hz)\n", CSC_FREQ);
		num_channels = 1;
		freqs = XCALLOC(num_channels, sizeof(uint32_t));
		freqs[0] = CSC_FREQ;
	}

	if(outfile == NULL) {
		outfile = strdup("-");		// output to stdout by default
		hourly = daily = 0;		// stdout is not rotateable - ignore silently
	}
	if(outfile != NULL && hourly && daily) {
		fprintf(stderr, "Options: -H and -D are exclusive\n");
		fprintf(stderr, "Use -h for help\n");
		_exit(1);
	}
	sample_rate = SYMBOL_RATE * SPS * oversample;
	fprintf(stderr, "Sampling rate set to %u sps\n", sample_rate);
	if(centerfreq == 0) {
		centerfreq = calc_centerfreq(freqs, num_channels, sample_rate);
		if(centerfreq == 0) {
			fprintf(stderr, "Failed to calculate center frequency\n");
			_exit(2);
		}
	}

	memset(&ctx, 0, sizeof(vdl2_state_t));
	ctx.num_channels = num_channels;
	ctx.channels = XCALLOC(num_channels, sizeof(vdl2_channel_t *));
	for(int i = 0; i < num_channels; i++) {
		if((ctx.channels[i] = vdl2_channel_init(centerfreq, freqs[i], sample_rate, oversample)) == NULL) {
			fprintf(stderr, "Failed to initialize VDL channel\n");
			_exit(2);
		}
	}

	if(rs_init() < 0) {
		fprintf(stderr, "Failed to initialize RS codec\n");
		_exit(3);
	}
#ifdef WITH_STATSD
	if(statsd_enabled && input != INPUT_FILE) {
		if(statsd_initialize(statsd_addr) < 0) {
				fprintf(stderr, "Failed to initialize statsd client - disabling\n");
				XFREE(statsd_addr);
				statsd_enabled = 0;
		} else {
			for(int i = 0; i < num_channels; i++)
				statsd_initialize_counters(freqs[i]);
		}
	} else {
		XFREE(statsd_addr);
		statsd_enabled = 0;
	}
#endif
	if(init_output_file(outfile) < 0) {
		fprintf(stderr, "Failed to initialize output - aborting\n");
		_exit(4);
	}
	if(pp_addr && init_pp(pp_addr) < 0) {
		fprintf(stderr, "Failed to initialize output socket to Planeplotter - disabling it\n");
		XFREE(pp_addr);
	}
	setup_signals();
	sincosf_lut_init();
	input_lpf_init(sample_rate);
	demod_sync_init();
	setup_threads(&ctx);
	switch(input) {
	case INPUT_FILE:
		process_file(&ctx, infile, sample_fmt);
		pthread_barrier_wait(&demods_ready);
		break;
#ifdef WITH_RTLSDR
	case INPUT_RTLSDR:
		rtl_init(&ctx, device, centerfreq, gain, correction);
		break;
#endif
#ifdef WITH_MIRISDR
	case INPUT_MIRISDR:
		mirisdr_init(&ctx, device, mirisdr_hw_flavour, centerfreq, gain, correction, mirisdr_usb_xfer_mode);
		break;
#endif
#ifdef WITH_SDRPLAY
	case INPUT_SDRPLAY:
		sdrplay_init(&ctx, device, sdrplay_antenna, centerfreq, sdrplay_gr, correction,
		sdrplay_biast, sdrplay_notch_filter, sdrplay_agc, sdrplay_tuner);
		break;
#endif
#ifdef WITH_SOAPYSDR
	case INPUT_SOAPYSDR:
		soapysdr_init(&ctx, device, soapysdr_antenna, centerfreq, gain, correction,
		soapysdr_settings, soapysdr_gain);
		break;
#endif
	default:
		fprintf(stderr, "Unknown input type\n");
		_exit(5);
	}
	return(0);
}
