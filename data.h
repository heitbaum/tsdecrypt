#ifndef DATA_H
#define DATA_H

#include <pthread.h>

#include <openssl/aes.h>
#include <openssl/md5.h>

#include <dvbcsa/dvbcsa.h>

#include "libfuncs/libfuncs.h"
#include "libts/tsfuncs.h"

struct key {
	uint8_t				cw[16];
	int					is_valid_cw;
	struct dvbcsa_key_s	*csakey[2];
};

// 4 auth header, 20 header size, 256 max data size, 16 potential padding
#define CAMD35_HDR_LEN (20)
#define CAMD35_BUF_LEN (4 + CAMD35_HDR_LEN + 256 + 16)

struct camd35 {
	uint8_t			buf[CAMD35_BUF_LEN];

	int				server_fd;
	struct in_addr	server_addr;
	unsigned int	server_port;
	char			user[64];
	char			pass[64];

	AES_KEY			aes_encrypt_key;
	AES_KEY			aes_decrypt_key;

	uint32_t		auth_token;

	struct key		*key;

	pthread_t		thread;
	QUEUE			*queue;
};

enum io_type {
	FILE_IO,
	NET_IO,
	WTF_IO
};

struct io {
	int					fd;
	enum io_type		type;
	char				*fname;
	struct in_addr		addr;
	unsigned int		port;
	// Used only for output
	int					ttl;
	struct in_addr		intf;
};

struct ts {
	// Stream handling
	struct ts_pat		*pat, *curpat;
	struct ts_cat		*cat, *curcat;
	struct ts_pmt		*pmt, *curpmt;
	struct ts_privsec	*emm, *last_emm;
	struct ts_privsec	*ecm, *last_ecm;
	uint16_t			pmt_pid;
	uint16_t			service_id;
	uint16_t			emm_caid, emm_pid;
	uint16_t			ecm_caid, ecm_pid;
	uint16_t			ecm_counter;
	pidmap_t			pidmap;

	// CAMD handling
	struct key			key;
	struct camd35		camd35;

	// Config
	enum CA_system		req_CA_sys;

	int					emm_send;
	int					pid_filter;

	struct io			input;
	struct io			output;

	int					debug_level;

	int					camd_stop;
	int					is_cw_error;
};

enum msg_type { EMM_MSG, ECM_MSG };

struct camd_msg {
	enum msg_type	type;
	uint16_t		idx;
	uint16_t		ca_id;
	uint16_t		service_id;
	uint8_t			data_len;
	uint8_t			data[255];
	struct ts		*ts;
};

void data_init(struct ts *ts);
void data_free(struct ts *ts);

#endif