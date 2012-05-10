#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <linux/limits.h>

#include "parser.h"

#define LINE_MAX	1024
#define KEY_MAX		64

/*
 * for debug
 */
void display_config(struct config conf)
{
	printf("config: %p\n", &conf);
	printf("IFWI: %s\n", conf.ifwi);
	printf("FWR DnX: %s\n", conf.fwr_dnx);
	printf("NORMALOS: %s\n", conf.boot);
	printf("PREOS: %s\n", conf.preos);
	printf("PLATFORM: %s\n", conf.platform);
	printf("DATA: %s\n", conf.data);
	printf("CSA: %s\n", conf.csa);
}

/*
 * initialize config, get FWs according to version (ver)
 */
int parse_config(char *stream, size_t size, struct config *conf, char *ver)
{
	char *line, *end, *cp;
	char *key, *value;
	char key_ifwi[KEY_MAX];
	char key_fwr_dnx[KEY_MAX];
	int i = 0;

	if (!stream || !conf || !ver)
		return -1;

	snprintf(key_ifwi, sizeof(key_ifwi), "V%s_IFWI", ver);
	snprintf(key_fwr_dnx, sizeof(key_fwr_dnx), "V%s_FWR_DnX", ver);

	line = stream;

	while (line - stream < size) {
		end = strchr(line, '\n');
		if (!end) {
			fprintf(stderr, "no end of line\n");
			return -1;
		}
		*end = 0;
		if ((cp = strchr(line, '#')))
			*cp = 0;
		key = strtok(line, " =");
		value = strtok(NULL, " =");
		if (!key || !value) {
			line = end + 1;
			continue;
		}

		if (!strcmp(key, key_ifwi)) {
			strncpy(conf->ifwi, value, sizeof(conf->ifwi));
		}
		else if (!strcmp(key, key_fwr_dnx))
			strncpy(conf->fwr_dnx, value, sizeof(conf->fwr_dnx));
		else if (!strcmp(key, "NORMALOS"))
			strncpy(conf->boot, value, sizeof(conf->boot));
		else if (!strcmp(key, "PREOS"))
			strncpy(conf->preos, value, sizeof(conf->preos));
		else if (!strcmp(key, "PLATFORM"))
			strncpy(conf->platform, value, sizeof(conf->platform));
		else if (!strcmp(key, "DATA"))
			strncpy(conf->data, value, sizeof(conf->data));
		else if (!strcmp(key, "CSA"))
			strncpy(conf->csa, value, sizeof(conf->csa));
		line = end + 1;
	}

	return 0;
}
